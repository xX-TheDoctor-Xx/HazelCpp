#include "UdpConnection.hpp"
#include "Util.hpp"

#include <algorithm>

namespace Hazel
{
	UdpConnection::UdpConnection()
	{
		InitializeKeepAliveTimer();
	}

	// KeepAlive

	int UdpConnection::GetKeepAliveInterval()
	{
		return keep_alive_interval;
	}

	void UdpConnection::SetKeepAliveInterval(int value)
	{
		keep_alive_interval = value;
		ResetKeepAliveTimer();
	}

	void KeepAliveTimerCallback(UdpConnection *con)
	{
		con->SendHello(Bytes(nullptr, -1), std::function<void()>());
	}

	void UdpConnection::InitializeKeepAliveTimer()
	{
		auto fn = [this]()
		{
			keep_alive_timer.SetInterval(keep_alive_interval);
			keep_alive_timer.Callback = KeepAliveTimerCallback;
			keep_alive_timer.Start(this);
		};

		lock(keep_alive_timer_mutex, fn)
	}

	void UdpConnection::ResetKeepAliveTimer()
	{
		auto fn = [this]()
		{
			keep_alive_timer.Stop();
			keep_alive_timer.Start(this);
		};

		lock(keep_alive_timer_mutex, fn)
	}

	void UdpConnection::StopKeepAliveTimer()
	{
		auto fn = [this]()
		{
			if (!keep_alive_timer_stopped)
				keep_alive_timer.Stop();
			keep_alive_timer_stopped = true;
		};

		lock(keep_alive_timer_mutex, fn)
	}

	//Fragmented

	int UdpConnection::GetFragmentSize()
	{
		return 65500;
	}

	void UdpConnection::FragmentedSend(Bytes data)
	{
		ushort id = ++last_fragment_id_allocated;

		for (ushort i = 0; i < ceil(data.GetLength() / GetFragmentSize()); i++)
		{
			int buffer_size = min(data.GetLength() - (GetFragmentSize() * i), GetFragmentSize()) + 7;
			Bytes buffer(new byte[buffer_size](), buffer_size);

			buffer[0] = (i == 0 ? (byte)SendOption::FragmentedReliable : (byte)UdpSendOption::Fragment);

			buffer[1] = (byte)((id >> 8) & 0xFF);
			buffer[2] = (byte)id;

			if (i == 0)
			{
				ushort fragments = (ushort)ceil(data.GetLength() / GetFragmentSize());
				buffer[3] = (byte)((fragments >> 8) & 0xFF);
				buffer[4] = (byte)fragments;
			}
			else
			{
				buffer[3] = (byte)((i >> 8) & 0xFF);
				buffer[4] = (byte)i;
			}

			AttachReliableID(data, 5);

			Util::BlockCopy(data.GetBytes(), GetFragmentSize() * i, buffer.GetBytes(), 7, buffer.GetLength() - 7);
			//memcpy(buffer.GetBytes() + 7, data.GetBytes() + GetFragmentSize() * i, buffer.GetLength() - 7); // will this work?

			WriteBytesToConnection(buffer);
		}
	}

	FragmentedMessage UdpConnection::GetFragmentedMessage(ushort message_id)
	{
		FragmentedMessage message;
		auto fn = [this, message, message_id]()
		{
			if (fragmented_messages_received.find(message_id) != fragmented_messages_received.end())
				const_cast<FragmentedMessage&>(message) = fragmented_messages_received.at(message_id);
			else
				fragmented_messages_received.insert(std::make_pair(message_id, message));
		};

		lock(fragmented_messages_received_mutex, fn)
		return message;
	}

	void UdpConnection::FragmentedStartMessageReceive(Bytes buffer)
	{
		if (!ProcessReliableReceive(buffer, 5))
			return;

		ushort id = (ushort)((buffer[1] << 8) + buffer[2]);
		ushort length = (ushort)((buffer[3] << 8) + buffer[4]);

		FragmentedMessage message;
		bool message_complete;

		auto fn = [this, message, id, buffer, length, message_complete]()
		{
			FragmentedMessage &nonconst_message = const_cast<FragmentedMessage&>(message);
			nonconst_message = GetFragmentedMessage(id);
			nonconst_message.received.emplace_back(Fragment(0, buffer, 7));
			nonconst_message.no_fragments = length;

			const_cast<bool&>(message_complete) = message.no_fragments == message.received.size();
		};

		lock(fragmented_messages_received_mutex, fn)

		if (message_complete)
			FinalizeFragmentedMessage(message);
	}

	void UdpConnection::FragmentedMessageReceive(Bytes buffer)
	{
		if (!ProcessReliableReceive(buffer, 5))
			return;

		ushort id = (ushort)((buffer[1] << 8) + buffer[2]);
		ushort fragment_id = (ushort)((buffer[3] << 8) + buffer[4]);

		FragmentedMessage message;
		bool message_complete;

		auto fn = [this, message, id, fragment_id, buffer, message_complete]()
		{
			FragmentedMessage &nonconst_message = const_cast<FragmentedMessage&>(message);
			nonconst_message = GetFragmentedMessage(id);
			nonconst_message.received.emplace_back(Fragment(fragment_id, buffer, 7));

			const_cast<bool&>(message_complete) = message.no_fragments == message.received.size();
		};

		lock(fragmented_messages_received_mutex, fn)

		if (message_complete)
			FinalizeFragmentedMessage(message);
	}

	void UdpConnection::FinalizeFragmentedMessage(FragmentedMessage &message)
	{
		std::vector<Fragment> ordered_fragments = message.received;

		std::sort(ordered_fragments.begin(), ordered_fragments.end()); // untested

		int complete_data_size = (ordered_fragments.size() - 1) * GetFragmentSize() + ordered_fragments.at(ordered_fragments.size() - 1).Data.GetLength();
		Bytes complete_data(new byte[complete_data_size](), complete_data_size);
		int ptr = 0;
		for (Fragment &fragment : ordered_fragments)
		{
			Util::BlockCopy(fragment.Data.GetBytes(), fragment.Offset, complete_data.GetBytes(), ptr, fragment.Data.GetLength() - fragment.Offset);
			//memcpy(complete_data.GetBytes() + ptr, fragment.Data.GetBytes() + fragment.Offset, fragment.Data.GetLength() - fragment.Offset); // omg this is so messed up
			ptr += fragment.Data.GetLength() - fragment.Offset;
		}

		Connection::InvokeDataReceived(complete_data, SendOption::FragmentedReliable);
	}

	//reliable

	int UdpConnection::GetResendTimeout()
	{
		return resend_timeout.load();
	}

	void UdpConnection::SetResendTimeout(int timeout)
	{
		resend_timeout = timeout;
	}

	int UdpConnection::GetResendsBeforeDisconnect()
	{
		return resends_before_disconnect.load();
	}

	void UdpConnection::SetResendsBeforeDisconnect(int times)
	{
		resends_before_disconnect = times;
	}

	double UdpConnection::GetAveragePing()
	{
		long t = total_reliable_messages.load();
		return (t == 0) ? 0 : total_round_time.load() / t / 2;
	}

	void packet_resend_action(UdpConnection *conn, Packet &packet)
	{
		auto fn = [conn, packet]()
		{
			Packet &nonconst_packet = const_cast<Packet&>(packet);

			if (!nonconst_packet.GetAcknowledged())
			{
				nonconst_packet.GetTimer().Stop();
				nonconst_packet.IncrementLastTimeout(nonconst_packet.GetLastTimeout() * 2); //this is probably not right
				nonconst_packet.GetTimer().SetInterval(nonconst_packet.GetLastTimeout()); //this is probably not right
				nonconst_packet.GetTimer().Start(conn, nonconst_packet);
				nonconst_packet.IncrementRetransmissions(1);
				if (nonconst_packet.GetRetransmissions() > conn->GetResendsBeforeDisconnect())
				{
					conn->HandleDisconnect();
					nonconst_packet.SetAcknowledged(true);
					nonconst_packet.Recycle();

					return;
				}

				try
				{
					conn->WriteBytesToConnection(nonconst_packet.GetData());
				}
				catch (HazelException&)
				{
					conn->HandleDisconnect(HazelException("Could not resend data as connection is no longer connected"));
				}
			}
		};

		lock(packet.GetTimerMutex(), fn)
	}

	template<typename ...Args>
	void UdpConnection::AttachReliableID(Bytes buffer, int offset, std::function<void(Args...)> &ack_callback, ...)
	{
		auto fn = [this, buffer, offset, ack_callback]()
		{
			Bytes &nonconst_bytes = const_cast<Bytes&>(buffer);

			ushort id;

			do
				id = last_id_allocated.load() + 1;
			while (reliable_data_packets_sent.find(id) != reliable_data_packets_sent.end());

			nonconst_bytes[offset] = (byte)((id >> 8) & 0xFF);
			nonconst_bytes[offset + 1] = (byte)id;

			Packet packet = Packet::GetObject();

			packet.Set(buffer, this, std::function<void(UdpConnection*, Packet&)>(packet_resend_action), resend_timeout.load() > 0 ? resend_timeout.load() : (GetAveragePing() != 0 ? GetAveragePing() * 4 : 200), const_cast<std::function<void(Args...)>&>(ack_callback));

			reliable_data_packets_sent.insert(std::make_pair(id, packet));
		};

		lock(reliable_data_packets_sent_mutex, fn)
	}

	template<typename ...Args>
	void UdpConnection::ReliableSend(byte send_option, Bytes data, std::function<void(Args...)> &ack_callback, ...)
	{
		Bytes bytes(new byte[data.GetLength() + 3](), data.GetLength() + 3);

		bytes[0] = send_option;

		AttachReliableID(bytes, 1, ack_callback);

		//memcpy(bytes.GetBytes() + (bytes.GetLength() - data.GetLength()), data.GetBytes(), data.GetLength());
		Util::BlockCopy(data.GetBytes(), 0, bytes.GetBytes(), bytes.GetLength() - data.GetLength(), data.GetLength());

		WriteBytesToConnection(bytes);

		GetStatistics().LogReliableSend(data.GetLength(), bytes.GetLength());
	}

	void UdpConnection::ReliableMessageReceive(Bytes buffer)
	{
		if (ProcessReliableReceive(buffer, 1))
			InvokeDataReceived(SendOption::Reliable, buffer, 3);

		GetStatistics().LogReliableReceive(buffer.GetLength() - 3, buffer.GetLength());
	}

	bool UdpConnection::ProcessReliableReceive(Bytes bytes, int offset)
	{
		ushort id = (ushort)((bytes[offset] << 8) + bytes[offset + 1]);

		SendAck(bytes[offset], bytes[offset + 1]);

		auto fn = [this, id]()
		{
			ushort overwrite_pointer = (ushort)(reliable_receive_last.load() - 32768);

			bool is_new;
			is_new = (overwrite_pointer < reliable_receive_last.load()) ? id > reliable_receive_last.load() || id <= overwrite_pointer : id > reliable_receive_last.load() && id <= overwrite_pointer;

			if (is_new || !has_received_something.load())
			{
				for (ushort i = (ushort)(reliable_receive_last.load() + 1); i < id; i++)
					reliable_data_packets_missing.emplace_back(i);

				reliable_receive_last = id;
				has_received_something = true;
			}
			else
			{
				if (std::find(reliable_data_packets_missing.begin(), reliable_data_packets_missing.end(), id) != reliable_data_packets_missing.end())
					reliable_data_packets_missing.erase(std::remove(reliable_data_packets_missing.begin(), reliable_data_packets_missing.end(), id), reliable_data_packets_missing.end());
				else return true;
			}
		};

		lock(reliable_data_packets_missing_mutex, fn)

		return true;
	}

	void UdpConnection::AcknowledgementMessageReceive(Bytes bytes)
	{
		ushort id = (ushort)((bytes[1] << 8) + bytes[2]);

		auto fn = [this, id]()
		{
			if (reliable_data_packets_sent.find(id) != reliable_data_packets_sent.end())
			{
				Packet packet = reliable_data_packets_sent.at(id);
				packet.SetAcknowledged(true);

				packet.InvokeAckCallback();

				packet.StopwatchStop();
				total_round_time += packet.GetRoundTime();
				total_reliable_messages++;

				packet.Recycle();

				reliable_data_packets_sent.erase(id);
			}
		};

		lock(reliable_data_packets_sent_mutex, fn)

		GetStatistics().LogReliableReceive(0, bytes.GetLength());
	}

	void UdpConnection::SendAck(byte byte1, byte byte2)
	{
		Bytes bytes(new byte[3](), 3);
		bytes[0] = (byte)UdpSendOption::Acknowledgement;
		bytes[1] = byte1;
		bytes[2] = byte2;
		WriteBytesToConnection(bytes);
	}

	//UdpConnection

	void UdpConnection::SendBytes(Bytes bytes, SendOption send_option)
	{
		if (GetState() != ConnectionState::Connected)
			throw HazelException("Could not send data as this Connection is not connected. Did you disconnect?");

		HandleSend(bytes, (byte)send_option);
	}

	template<typename ...Args>
	void UdpConnection::HandleSend(Bytes data, byte send_option, std::function<void(Args...)> &ack_callback, ...)
	{
		ResetKeepAliveTimer();

		switch (send_option)
		{
			case (byte)SendOption::Reliable:
			case (byte)UdpSendOption::Hello:
			{
				ReliableSend(send_option, data, ack_callback);
				break;
			}
			case (byte)SendOption::FragmentedReliable:
			{
				FragmentedSend(data);
				break;
			}
			default:
			{
				UnreliableSend(data, send_option);
				break;
			}
		}
	}

	void UdpConnection::HandleReceive(Bytes buffer)
	{
		ResetKeepAliveTimer();

		switch (buffer[0])
		{
			case (byte)SendOption::Reliable:
			{
				ReliableMessageReceive(buffer);
				break;
			}
			case (byte)UdpSendOption::Acknowledgement:
			{
				AcknowledgementMessageReceive(buffer);
				break;
			}
			case (byte)UdpSendOption::Hello:
			{
				ProcessReliableReceive(buffer, 1);
				GetStatistics().LogHelloReceive(buffer.GetLength());
				break;
			}
			case (byte)UdpSendOption::Disconnect:
			{
				HandleDisconnect();
				break;
			}
			case (byte)SendOption::FragmentedReliable:
			{
				FragmentedStartMessageReceive(buffer);
				break;
			}
			case (byte)UdpSendOption::Fragment:
			{
				FragmentedMessageReceive(buffer);
				break;
			}
			default:
			{
				InvokeDataReceived(SendOption::None, buffer, 1);
				GetStatistics().LogUnreliableReceive(buffer.GetLength() - 1, buffer.GetLength());
				break;
			}
		}
	}

	void UdpConnection::Stop()
	{
		StopKeepAliveTimer();
	}

	void UdpConnection::UnreliableSend(Bytes data, byte send_option)
	{
		Bytes bytes(new byte[data.GetLength() + 1](), data.GetLength() + 1);

		bytes[0] = send_option;

		//memcpy(bytes.GetBytes() + (bytes.GetLength() - data.GetLength()), data.GetBytes(), data.GetLength());
		Util::BlockCopy(data.GetBytes(), 0, bytes.GetBytes(), bytes.GetLength() - data.GetLength(), data.GetLength());

		WriteBytesToConnection(bytes);

		GetStatistics().LogUnreliableSend(data.GetLength(), bytes.GetLength());
	}

	void UdpConnection::InvokeDataReceived(SendOption send_option, Bytes buffer, int offset)
	{
		Bytes bytes(new byte[buffer.GetLength() - offset](), buffer.GetLength() - offset);
		//memcpy(bytes.GetBytes(), buffer.GetBytes() + offset, bytes.GetLength());
		Util::BlockCopy(buffer.GetBytes(), offset, bytes.GetBytes(), 0, bytes.GetLength());

		Connection::InvokeDataReceived(bytes, send_option);
	}

	template<typename ...Args>
	void UdpConnection::SendHello(Bytes bytes, std::function<void(Args...)> &acknowledge_callback, ...)
	{
		Bytes actual_bytes;
		if (!bytes.IsValid())
			actual_bytes.SetBytes(new byte[1](), 1);
		else
		{
			actual_bytes.SetBytes(new byte[bytes.GetLength() + 1](), bytes.GetLength() + 1);
			//memcpy(actual_bytes.GetBytes() + 1, bytes.GetBytes(), bytes.GetLength());
			Util::BlockCopy(bytes.GetBytes(), 0, actual_bytes.GetBytes(), 1, bytes.GetLength());
		}

		HandleSend(actual_bytes, (byte)UdpSendOption::Hello, acknowledge_callback);
	}

	void UdpConnection::SendDisconnect()
	{
		HandleSend(Bytes(new byte[0](), 0), (byte)UdpSendOption::Disconnect);
	}
}