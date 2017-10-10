#include "ConnectionStatistics.hpp"

namespace Hazel
{
	ConnectionStatistics::ConnectionStatistics()
	{
	}

	ConnectionStatistics::ConnectionStatistics(const ConnectionStatistics &stats)
	{
		MessagesSent.exchange(stats.MessagesSent);
		UnreliableMessagesSent.exchange(stats.UnreliableMessagesSent);
		ReliableMessagesSent.exchange(stats.ReliableMessagesSent);
		FragmentedMessagesSent.exchange(stats.FragmentedMessagesSent);
		AcknowledgementMessagesSent.exchange(stats.AcknowledgementMessagesSent);
		HelloMessagesSent.exchange(stats.HelloMessagesSent);
		DataBytesSent.exchange(stats.DataBytesSent);
		TotalBytesSent.exchange(stats.TotalBytesSent);
		MessagesReceived.exchange(stats.MessagesReceived);
		UnreliableMessagesReceived.exchange(stats.UnreliableMessagesReceived);
		ReliableMessagesReceived.exchange(stats.ReliableMessagesReceived);
		FragmentedMessagesReceived.exchange(stats.FragmentedMessagesReceived);
		AcknowledgementMessagesReceived.exchange(stats.AcknowledgementMessagesReceived);
		HelloMessagesReceived.exchange(stats.HelloMessagesReceived);
		DataBytesReceived.exchange(stats.DataBytesReceived);
		TotalBytesReceived.exchange(stats.TotalBytesReceived);
	}

	long ConnectionStatistics::GetMessagesSent()
	{
		return MessagesSent.load();
	}

	long ConnectionStatistics::GetUnreliableMessagesSent()
	{
		return UnreliableMessagesSent.load();
	}

	long ConnectionStatistics::GetReliableMessagesSent()
	{
		return ReliableMessagesSent.load();
	}

	long ConnectionStatistics::GetFragmentedMessagesSent()
	{
		return FragmentedMessagesSent.load();
	}

	long ConnectionStatistics::GetAcknowledgementMessagesSent()
	{
		return AcknowledgementMessagesSent.load();
	}

	long ConnectionStatistics::GetHelloMessagesSent()
	{
		return HelloMessagesSent.load();
	}

	long ConnectionStatistics::GetDataBytesSent()
	{
		return DataBytesSent.load();
	}

	long ConnectionStatistics::GetTotalBytesSent()
	{
		return TotalBytesSent.load();
	}

	long ConnectionStatistics::GetMessagesReceived()
	{
		return MessagesReceived.load();
	}

	long ConnectionStatistics::GetUnreliableMessagesReceived()
	{
		return UnreliableMessagesReceived.load();
	}

	long ConnectionStatistics::GetReliableMessagesReceived()
	{
		return ReliableMessagesReceived.load();
	}

	long ConnectionStatistics::GetFragmentedMessagesReceived()
	{
		return FragmentedMessagesReceived.load();
	}

	long ConnectionStatistics::GetAcknowledgementMessagesReceived()
	{
		return AcknowledgementMessagesReceived.load();
	}

	long ConnectionStatistics::GetHelloMessagesReceived()
	{
		return HelloMessagesReceived.load();
	}

	long ConnectionStatistics::GetDataBytesReceived()
	{
		return DataBytesReceived.load();
	}

	long ConnectionStatistics::GetTotalBytesReceived()
	{
		return TotalBytesReceived.load();
	}

	void ConnectionStatistics::LogUnreliableSend(int dataLength, int totalLength)
	{
		++UnreliableMessagesSent;
		DataBytesSent += dataLength;
		TotalBytesSent += totalLength;
	}

	void ConnectionStatistics::LogReliableSend(int dataLength, int totalLength)
	{
		++ReliableMessagesSent;
		DataBytesSent += dataLength;
		TotalBytesSent += totalLength;
	}

	void ConnectionStatistics::LogFragmentedSend(int dataLength, int totalLength)
	{
		++FragmentedMessagesSent;
		DataBytesSent += dataLength;
		TotalBytesSent += totalLength;
	}

	void ConnectionStatistics::LogAcknowledgementSend(int totalLength)
	{
		++AcknowledgementMessagesSent;
		TotalBytesSent += totalLength;
	}

	void ConnectionStatistics::LogHelloSend(int totalLength)
	{
		++HelloMessagesSent;
		TotalBytesSent += totalLength;
	}

	void ConnectionStatistics::LogUnreliableReceive(int dataLength, int totalLength)
	{
		++UnreliableMessagesReceived;
		DataBytesReceived += dataLength;
		TotalBytesReceived += totalLength;
	}

	void ConnectionStatistics::LogReliableReceive(int dataLength, int totalLength)
	{
		++ReliableMessagesReceived;
		DataBytesReceived += dataLength;
		TotalBytesReceived += totalLength;
	}

	void ConnectionStatistics::LogFragmentedReceive(int dataLength, int totalLength)
	{
		++FragmentedMessagesReceived;
		DataBytesReceived += dataLength;
		TotalBytesReceived += totalLength;
	}

	void ConnectionStatistics::LogAcknowledgementReceive(int totalLength)
	{
		++AcknowledgementMessagesReceived;
		TotalBytesReceived += totalLength;
	}

	void ConnectionStatistics::LogHelloReceive(int totalLength)
	{
		++HelloMessagesReceived;
		TotalBytesReceived += totalLength;
	}
}