#include "Packet.hpp"

namespace Hazel
{
	Packet::Packet() : data(nullptr, -1)
	{
	}

	Packet::Packet(const Packet &packet) : data(nullptr, -1)
	{
		data = packet.data;
		last_timeout.exchange(packet.last_timeout);
		ack_callback = packet.ack_callback;
		acknowledged.exchange(packet.acknowledged);
		retransmissions.exchange(packet.retransmissions);
		stopwatch = packet.stopwatch;
	}

	Packet::~Packet()
	{
		lock(TimerMutex)
		{
			Timer.Stop();
		}
	}

	void Packet::Set(Bytes data, GenericFunction<void, UdpConnection*, Packet&> &resend_action, int timeout, GenericFunction<void> &ack_callback)
	{
		this->data = data;

		Timer.SetInterval(timeout); // i dont think this is right
		Timer.callback = resend_action;
		Timer.params.emplace_back(this);
		Timer.Start();

		last_timeout = timeout;
		this->ack_callback = ack_callback;

		stopwatch.Start();
	}

	Bytes Packet::GetData()
	{
		return data;
	}

	int Packet::GetLastTimeout()
	{
		return last_timeout.load();
	}

	void Packet::IncrementLastTimeout(int value)
	{
		last_timeout += value;
	}

	bool Packet::GetAcknowledged()
	{
		return acknowledged.load();
	}

	void Packet::SetAcknowledged(bool value)
	{
		acknowledged = value;
	}

	int Packet::GetRetransmissions()
	{
		return retransmissions.load();
	}

	void Packet::IncrementRetransmissions(int value)
	{
		retransmissions += value;
	}

	void Packet::InvokeAckCallback()
	{
		ack_callback.Call();
	}

	void Packet::StopwatchStop()
	{
		stopwatch.Stop();
	}

	long long Packet::GetRoundTime()
	{
		return stopwatch.GetElapsedMilliseconds();
	}
}