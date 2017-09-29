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
	void packet_resend_action(UdpConnection *conn, Packet &packet);

	class UdpConnection : public NetworkConnection
	{
		friend void packet_resend_action(UdpConnection *conn, Packet &packet); // so that i can access the inacessible methods

		//KeepAlive
		int keep_alive_interval = 10000;
		Timer<UdpConnection*> keep_alive_timer;
		std::mutex keep_alive_timer_mutex;
		bool keep_alive_timer_stopped;

		void InitializeKeepAliveTimer();
		void ResetKeepAliveTimer();
		void StopKeepAliveTimer();

		//Fragmented
		std::atomic_ushort last_fragment_id_allocated;
		std::map<unsigned short, FragmentedMessage> fragmented_messages_received;
		std::mutex fragmented_messages_received_mutex;

		void FragmentedSend(Bytes data);
		FragmentedMessage GetFragmentedMessage(unsigned short messageId);
		void FragmentedStartMessageReceive(Bytes buffer);
		void FragmentedMessageReceive(Bytes buffer);
		void FinalizeFragmentedMessage(FragmentedMessage &message);

		//Reliable
		std::map<unsigned short, Packet> reliable_data_packets_sent;
		std::mutex reliable_data_packets_sent_mutex;

		std::atomic_int resend_timeout = 0;
		std::atomic_ushort last_id_allocated;

		std::vector<unsigned short> reliable_data_packets_missing; // can i replace C#'s HashSet with std::vector?
		std::mutex reliable_data_packets_missing_mutex;

		std::atomic_ushort reliable_receive_last;
		std::atomic_bool has_received_something;
		std::atomic_long total_round_time;
		std::atomic_long total_reliable_messages;
		std::atomic_int resends_before_disconnect = 3;

		void AttachReliableID(Bytes buffer, int offset, GenericFunction<void> &ack_callback = GenericFunction<void>()); // is ackCallback of type std::function<void()>? or do i need params?
		void ReliableSend(byte send_option, Bytes data, GenericFunction<void> &ack_callback = GenericFunction<void>());
		void ReliableMessageReceive(Bytes buffer);
		bool ProcessReliableReceive(Bytes bytes, int offset);
		void AcknowledgementMessageReceive(Bytes bytes);
		void SendAck(byte byte1, byte byte2);

		//Udp Connection 
		void UnreliableSend(Bytes data, byte send_option);

		void InvokeDataReceived(SendOption send_option, Bytes buffer, int data_offset);
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

		void SendBytes(Bytes bytes, SendOption send_option = SendOption::None) override;

	protected:
		//Udp Connection
		virtual void WriteBytesToConnection(Bytes bytes) = 0;

		void HandleSend(Bytes data, byte send_option, GenericFunction<void> &ack_callback = GenericFunction<void>());
		void HandleReceive(Bytes buffer);

		void SendHello(Bytes bytes, GenericFunction<void> &acknowledge_callback);
		virtual void HandleDisconnect(HazelException &e = HazelException()) = 0;
		void SendDisconnect();
	};
}