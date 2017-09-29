#include "UdpConnection.hpp"

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

		//Update timer
		ResetKeepAliveTimer();
	}

	void KeepAliveTimerCallback(UdpConnection *con)
	{
		
	}

	void UdpConnection::InitializeKeepAliveTimer()
	{
		lock(keep_alive_timer_mutex)
		{
			keep_alive_timer.SetInterval(keep_alive_interval);
			keep_alive_timer.callback.Set(KeepAliveTimerCallback);
			keep_alive_timer.Start();
		}
	}

	void UdpConnection::ResetKeepAliveTimer()
	{
		lock(keep_alive_timer_mutex)
		{
			keep_alive_timer.Stop();
			keep_alive_timer.SetInterval(keep_alive_interval);
			keep_alive_timer.Start();
		}
	}

	void UdpConnection::StopKeepAliveTimer()
	{
		lock(keep_alive_timer_mutex)
		{
			if (!keep_alive_timer_stopped)
				keep_alive_timer.Stop();
			keep_alive_timer_stopped = true;
		}
	}

	//Fragmented

	int UdpConnection::GetFragmentSize()
	{
		return 65500;
	}

	void UdpConnection::FragmentedSend(Bytes data)
	{
		unsigned short id = ++last_fragment_id_allocated;

		for (unsigned short i = 0; i < ceil(data.GetLength() / GetFragmentSize()); i++)
		{
			int buffer_size = std::min(data.GetLength() - (GetFragmentSize() * i), GetFragmentSize()) + 7;
			Bytes buffer(new byte[buffer_size](), buffer_size);

			buffer[0] = i == 0 ? (byte)SendOption::FragmentedReliable : (byte)UdpSendOption::Fragment;

			// big endian, litle endian check?
			buffer[1] = (byte)((id >> 8) & 0xFF);
			buffer[2] = (byte)id;

			if (i == 0)
			{
				unsigned short fragments = (unsigned short)ceil(data.GetLength() / GetFragmentSize());
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

	FragmentedMessage UdpConnection::GetFragmentedMessage(unsigned short messageId)
	{
		FragmentedMessage message;
		lock(fragmented_messages_received_mutex)
		{
			if (fragmented_messages_received.find(messageId) != fragmented_messages_received.end())
				message = fragmented_messages_received.at(messageId);
			else
				fragmented_messages_received.insert(std::make_pair(messageId, message));
		}
		return message;
	}

	void UdpConnection::FragmentedStartMessageReceive(Bytes buffer)
	{
		if (!ProcessReliableReceive(buffer, 5))
			return;

		unsigned short id = (unsigned short)((buffer[1] << 8) + buffer[2]);
		unsigned short length = (unsigned short)((buffer[3] << 8) + buffer[4]);

		FragmentedMessage message;
		bool message_complete;

		lock(fragmented_messages_received_mutex)
		{
			message = GetFragmentedMessage(id);
			message.received.emplace_back(Fragment(0, buffer, 7));
			message.no_fragments = length;

			message_complete = message.no_fragments == message.received.size();
		}

		if (message_complete)
			FinalizeFragmentedMessage(message);
	}

	void UdpConnection::FragmentedMessageReceive(Bytes buffer)
	{
		if (!ProcessReliableReceive(buffer, 5))
			return;

		unsigned short id = (unsigned short)((buffer[1] << 8) + buffer[2]);
		unsigned short fragmentID = (unsigned short)((buffer[3] << 8) + buffer[4]);

		FragmentedMessage message;
		bool messageComplete;

		lock(fragmented_messages_received_mutex);
		{
			message = GetFragmentedMessage(id);
			message.received.emplace_back(Fragment(fragmentID, buffer, 7));

			messageComplete = message.no_fragments == message.received.size();
		}

		if (messageComplete)
			FinalizeFragmentedMessage(message);
	}

	void UdpConnection::FinalizeFragmentedMessage(FragmentedMessage &message)
	{
		std::vector<Fragment> ordered_fragments;

		std::sort(message.received.begin(), message.received.end(), [](Fragment &right, Fragment &left)
		{
			return right.FragmentID < left.FragmentID;
		}); // untested

		int complete_data_size = (ordered_fragments.size() - 1) * GetFragmentSize() + ordered_fragments.at(ordered_fragments.size()).Data.GetLength();
		Bytes complete_data(new byte[complete_data_size](), complete_data_size); // same problem with sizeof
		int ptr = 0;
		for (unsigned int i = 0; i < ordered_fragments.size(); i++)
		{
			Fragment &fragment = ordered_fragments.at(i);
			Util::BlockCopy(fragment.Data.GetBytes(), fragment.Offset, complete_data.GetBytes(), ptr, fragment.Data.GetLength() - fragment.Offset);
			//memcpy(complete_data.GetBytes() + ptr, fragment.Data.GetBytes() + fragment.Offset, fragment.Data.GetLength() - fragment.Offset); // omg this is so messed up
			ptr += fragment.Data.GetLength() - fragment.Offset;
		}

		Connection::InvokeDataReceived(complete_data, SendOption::FragmentedReliable);
	}

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

	void Hazel::packet_resend_action(UdpConnection *conn, Packet &packet)
	{
		lock(packet.TimerMutex)
		{
			if (packet.GetAcknowledged())
			{
				packet.Timer.Stop();
				packet.IncrementLastTimeout(packet.GetLastTimeout() * 2);
				packet.Timer.SetInterval(packet.GetLastTimeout());
				packet.Timer.Start();
				packet.IncrementRetransmissions(1); // does it get incremented in Hazel C#?
				if (packet.GetRetransmissions() > conn->GetResendsBeforeDisconnect())
				{
					conn->HandleDisconnect();
					packet.SetAcknowledged(true);
					return;
				}

				try
				{
					conn->WriteBytesToConnection(packet.GetData());
				}
				catch (HazelException&)
				{
					conn->HandleDisconnect(); // hazel exception in C#
				}
			}
		}
	}

	void UdpConnection::AttachReliableID(Bytes buffer, int offset, GenericFunction<void> &ack_callback)
	{
		lock(reliable_data_packets_sent_mutex)
		{
			unsigned short id;

			do
			{
				id = last_id_allocated.load() + 1;
			} while (reliable_data_packets_sent.find(id) != reliable_data_packets_sent.end());

			//bid endian or litle? do i need that?
			buffer[offset] = (byte)((id >> 8) & 0xFF);
			buffer[offset + 1] = (byte)id;

			Packet packet;
			GenericFunction<void, UdpConnection*, Packet&> func;
			func.Set(std::bind(&packet_resend_action, this, packet));

			packet.Set(buffer, func, resend_timeout.load() > 0 ? resend_timeout.load() : (GetAveragePing() != 0 ? GetAveragePing() * 4 : 200), ack_callback);

			reliable_data_packets_sent.insert(std::make_pair(id, packet));
		}
	}

	void UdpConnection::ReliableSend(byte send_option, Bytes data, GenericFunction<void> &ack_callback)
	{
		Bytes bytes(new byte[data.GetLength() + 3](), data.GetLength() + 3);

		bytes[0] = send_option;

		AttachReliableID(bytes, 1, ack_callback);

		//memcpy(bytes.GetBytes() + (bytes.GetLength() - data.GetLength()), data.GetBytes(), data.GetLength());
		Util::BlockCopy(data.GetBytes(), 0, bytes.GetBytes(), bytes.GetLength() - data.GetLength(), data.GetLength());

		WriteBytesToConnection(bytes);

		Statistics.LogReliableSend(data.GetLength(), bytes.GetLength());
	}

	void UdpConnection::ReliableMessageReceive(Bytes buffer)
	{
		if (ProcessReliableReceive(buffer, 1))
			InvokeDataReceived(SendOption::Reliable, buffer, 3);

		Statistics.LogReliableReceive(buffer.GetLength() - 3, buffer.GetLength());
	}

	bool UdpConnection::ProcessReliableReceive(Bytes bytes, int offset)
	{
		unsigned short id = (unsigned short)((bytes[offset] << 8) + bytes[offset + 1]);

		SendAck(bytes[offset], bytes[offset + 1]);

		lock(reliable_data_packets_missing_mutex)
		{
			unsigned short overwrite_pointer = (unsigned short)(reliable_receive_last.load() - 32768);

			bool is_new;
			is_new = (overwrite_pointer < reliable_receive_last.load()) ? id > reliable_receive_last.load() || id <= overwrite_pointer : id > reliable_receive_last.load() && id <= overwrite_pointer;

			if (is_new || !has_received_something.load())
			{
				for (unsigned short i = (unsigned short)(reliable_receive_last.load() + 1); i < id; i++)
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
		}

		return true;
	}

	void UdpConnection::AcknowledgementMessageReceive(Bytes bytes)
	{
		unsigned short id = (unsigned short)((bytes[1] << 8) + bytes[2]);

		lock(reliable_data_packets_sent_mutex)
		{
			if (reliable_data_packets_sent.find(id) != reliable_data_packets_sent.end())
			{
				Packet packet = reliable_data_packets_sent.at(id);
				packet.SetAcknowledged(true);

				packet.InvokeAckCallback();

				packet.StopwatchStop();
				total_round_time += packet.GetRoundTime();
				++total_reliable_messages;

				reliable_data_packets_sent.erase(id);
			}
		}

		Statistics.LogReliableReceive(0, bytes.GetLength());
	}

	void UdpConnection::SendAck(byte byte1, byte byte2)
	{
		Bytes bytes(new byte[3](), 3);
		bytes[0] = (byte)UdpSendOption::Acknowledgement;
		bytes[1] = byte1;
		bytes[2] = byte2;
		WriteBytesToConnection(bytes);
	}

	void UdpConnection::SendBytes(Bytes bytes, SendOption send_option)
	{
		if (State != ConnectionState::Connected)
		{
			// throw new InvalidOperationException("Could not send data as this Connection is not connected. Did you disconnect?");
		}

		HandleSend(bytes, (byte)send_option);
	}

	void UdpConnection::HandleSend(Bytes data, byte send_option, GenericFunction<void> &ack_callback)
	{
		ResetKeepAliveTimer();

		switch (send_option)
		{
			case (byte)SendOption::Reliable:
			{
				ReliableSend(send_option, data, ack_callback);
				break;
			}
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
				Statistics.LogHelloReceive(buffer.GetLength());
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
				Statistics.LogUnreliableReceive(buffer.GetLength() - 1, buffer.GetLength());
				break;
			}
		}
	}

	void UdpConnection::UnreliableSend(Bytes data, byte send_option)
	{
		Bytes bytes(new byte[data.GetLength() + 1](), data.GetLength() + 1);

		bytes[0] = send_option;

		//memcpy(bytes.GetBytes() + (bytes.GetLength() - data.GetLength()), data.GetBytes(), data.GetLength());
		Util::BlockCopy(data.GetBytes(), 0, bytes.GetBytes(), bytes.GetLength() - data.GetLength(), data.GetLength());

		WriteBytesToConnection(bytes);

		Statistics.LogUnreliableSend(data.GetLength(), bytes.GetLength());
	}

	void UdpConnection::InvokeDataReceived(SendOption send_option, Bytes buffer, int offset)
	{
		Bytes bytes(new byte[buffer.GetLength() - offset](), buffer.GetLength() - offset);
		//memcpy(bytes.GetBytes(), buffer.GetBytes() + offset, bytes.GetLength());
		Util::BlockCopy(buffer.GetBytes(), offset, bytes.GetBytes(), 0, bytes.GetLength());

		Connection::InvokeDataReceived(bytes, send_option);
	}

	void UdpConnection::SendHello(Bytes bytes, GenericFunction<void> &acknowledge_callback)
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