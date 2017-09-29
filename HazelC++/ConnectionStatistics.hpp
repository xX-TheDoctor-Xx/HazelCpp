#pragma once

#include <atomic>

namespace Hazel
{
	class ConnectionStatistics
	{
	private:
		std::atomic<long> MessagesSent;
		std::atomic<long> UnreliableMessagesSent;
		std::atomic<long> ReliableMessagesSent;
		std::atomic<long> FragmentedMessagesSent;
		std::atomic<long> AcknowledgementMessagesSent;
		std::atomic<long> HelloMessagesSent;
		std::atomic<long> DataBytesSent;
		std::atomic<long> TotalBytesSent;
		std::atomic<long> MessagesReceived;
		std::atomic<long> UnreliableMessagesReceived;
		std::atomic<long> ReliableMessagesReceived;
		std::atomic<long> FragmentedMessagesReceived;
		std::atomic<long> AcknowledgementMessagesReceived;
		std::atomic<long> HelloMessagesReceived;
		std::atomic<long> DataBytesReceived;
		std::atomic<long> TotalBytesReceived;

	public:
		ConnectionStatistics();
		ConnectionStatistics(const ConnectionStatistics &stats);

		long GetMessagesSent();
		long GetUnreliableMessagesSent();
		long GetReliableMessagesSent();
		long GetFragmentedMessagesSent();
		long GetAcknowledgementMessagesSent();
		long GetHelloMessagesSent();
		long GetDataBytesSent();
		long GetTotalBytesSent();
		long GetMessagesReceived();
		long GetUnreliableMessagesReceived();
		long GetReliableMessagesReceived();
		long GetFragmentedMessagesReceived();
		long GetAcknowledgementMessagesReceived();
		long GetHelloMessagesReceived();
		long GetDataBytesReceived();
		long GetTotalBytesReceived();

		void LogUnreliableSend(int dataLength, int totalLength);
		void LogReliableSend(int dataLength, int totalLength);
		void LogFragmentedSend(int dataLength, int totalLength);
		void LogAcknowledgementSend(int totalLength);
		void LogHelloSend(int totalLength);
		void LogUnreliableReceive(int dataLength, int totalLength);
		void LogReliableReceive(int dataLength, int totalLength);
		void LogFragmentedReceive(int dataLength, int totalLength);
		void LogAcknowledgementReceive(int totalLength);
		void LogHelloReceive(int totalLength);
	};
}