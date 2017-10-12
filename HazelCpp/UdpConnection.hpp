#pragma once

#include "NetworkConnection.hpp"
#include "Timer.hpp"
#include "FragmentedMessages.hpp"
#include "Packet.hpp"
#include "HazelException.hpp"
#include "Bytes.hpp"

#include <map>

namespace Hazel
{
	class KeepAliveTimerCallback;
	class PacketResendActionTimerCallback;

	class UdpConnection : public NetworkConnection
	{
		friend class KeepAliveTimerCallback;
		friend class PacketResendActionTimerCallback;

		//KeepAlive
		int keep_alive_interval = 10000;
		Timer<KeepAliveTimerCallback> keep_alive_timer;
		std::mutex keep_alive_timer_mutex;
		bool keep_alive_timer_stopped;

		void InitializeKeepAliveTimer();
		void ResetKeepAliveTimer();
		void StopKeepAliveTimer();

		//Fragmented
		std::atomic_ushort last_fragment_id_allocated;
		std::map<ushort, FragmentedMessage> fragmented_messages_received;
		std::mutex fragmented_messages_received_mutex;

		void FragmentedSend(Bytes &data);
		FragmentedMessage GetFragmentedMessage(ushort message_id);
		void FragmentedStartMessageReceive(Bytes &buffer);
		void FragmentedMessageReceive(Bytes &buffer);
		void FinalizeFragmentedMessage(FragmentedMessage &message);

		//Reliable
		std::map<ushort, Packet<PacketResendActionTimerCallback>> reliable_data_packets_sent;
		std::mutex reliable_data_packets_sent_mutex;

		std::atomic_int resend_timeout = 0;
		std::atomic_ushort last_id_allocated;

		std::vector<ushort> reliable_data_packets_missing; // can i replace C#'s HashSet with std::vector?
		std::mutex reliable_data_packets_missing_mutex;

		std::atomic_ushort reliable_receive_last;
		std::atomic_bool has_received_something;
		std::atomic_long total_round_time;
		std::atomic_long total_reliable_messages;
		std::atomic_int resends_before_disconnect = 3;

		template<typename ...Args>
		void AttachReliableID(Bytes &buffer, int offset, std::function<void(Args...)> &ack_callback = empty_fn<Args...>, ...); // is ackCallback of type std::function<void()>? or do i need params?

		template<typename ...Args>
		void ReliableSend(byte send_option, Bytes &data, std::function<void(Args...)> &ack_callback = empty_fn<Args...>, ...);

		void ReliableMessageReceive(Bytes &buffer);
		bool ProcessReliableReceive(Bytes &bytes, int offset);
		void AcknowledgementMessageReceive(Bytes &bytes);
		void SendAck(byte byte1, byte byte2);

		//Udp Connection 
		void UnreliableSend(Bytes &data, byte send_option);

		void InvokeDataReceived(SendOption send_option, Bytes &buffer, int data_offset);
	public:
		UdpConnection();

		//KeepAlive
		int GetKeepAliveInterval();
		void SetKeepAliveInterval(int value);

		//Fragmented
		int GetFragmentSize();

		//Reliable
		int GetResendTimeout();
		void SetResendTimeout(int timeout);

		int GetResendsBeforeDisconnect();
		void SetResendsBeforeDisconnect(int times);

		double GetAveragePing();

		//Udp Connection

		void SendBytes(Bytes &bytes, SendOption send_option = SendOption::None) override;

		virtual void HandleDisconnect(HazelException &e = HazelException()) = 0;

		void HandleReceive(Bytes &buffer);

		void Stop();

	protected:
		//Udp Connection
		virtual void WriteBytesToConnection(Bytes &bytes) = 0;

		template<typename ...Args>
		void HandleSend(Bytes &data, byte send_option, std::function<void(Args...)> &ack_callback = empty_fn<Args...>, ...);

		template<typename ...Args>
		void SendHello(Bytes &bytes, std::function<void(Args...)> &acknowledge_callback, ...);

		void SendDisconnect();
	};

	class KeepAliveTimerCallback : public TimerCallback
	{
	public:
		KeepAliveTimerCallback(UdpConnection *con) : con(con)
		{
		}

		virtual void run()
		{
			con->SendHello(Bytes(nullptr, -1), std::function<void()>());
		}

	private:
		UdpConnection *con;
	};

	class PacketResendActionTimerCallback : public TimerCallback
	{
	public:
		PacketResendActionTimerCallback(UdpConnection *con, Packet<PacketResendActionTimerCallback> *packet) : con(con), packet(packet)
		{
		}

		virtual void run()
		{
			auto fn = [this]()
			{
				if (!packet->GetAcknowledged())
				{
					packet->GetTimer().Stop();
					packet->IncrementLastTimeout(packet->GetLastTimeout() * 2); //this is probably not right
					packet->GetTimer().SetInterval(packet->GetLastTimeout()); //this is probably not right
					//packet->GetTimer().Start<PacketResendActionTimerCallback>(this);
					packet->IncrementRetransmissions(1);
					if (packet->GetRetransmissions() > con->GetResendsBeforeDisconnect())
					{
						con->HandleDisconnect();
						packet->SetAcknowledged(true);
						packet->Recycle();

						return;
					}

					try
					{
						con->WriteBytesToConnection(packet->GetData());
					}
					catch (HazelException&)
					{
						con->HandleDisconnect(HazelException("Could not resend data as connection is no longer connected"));
					}
				}
			};

			lock_mutex(packet->GetTimerMutex(), fn)
		}

	private:
		UdpConnection *con;
		Packet<PacketResendActionTimerCallback> *packet;
	};
}