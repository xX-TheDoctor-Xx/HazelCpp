#include "Packet.hpp"

namespace Hazel
{
	Packet Packet::CreateObject()
	{
		return Packet();
	}

	Packet::Packet() : data(nullptr, -1)
	{
		object_pool.AssignObjectFactory(CreateObject);
	}

	Packet::Packet(const Packet &packet) : data(nullptr, -1)
	{
		data = packet.data;
		last_timeout.exchange(packet.last_timeout);
		ack_callback = packet.ack_callback;
		acknowledged.exchange(packet.acknowledged);
		retransmissions.exchange(packet.retransmissions);
		stopwatch = packet.stopwatch;
		object_pool.AssignObjectFactory(CreateObject);
	}

	Packet::~Packet()
	{
		lock(TimerMutex)
		{
			Timer.Reset();
		}
		
		data.Clear();
		ack_callback.Set(std::function<void()>());
		last_timeout = 0;
		acknowledged = false;
		retransmissions = 0;
		//stopwatch is auto reseted when start is called
		object_pool.PutObject(*this);
	}

	void Packet::Set(Bytes data, UdpConnection *con, GenericFunction<void, UdpConnection*, Packet&> &resend_action, int timeout, GenericFunction<void> &ack_callback)
	{
		this->data = data;

		Timer.SetInterval(timeout); // i dont think this is right
		Timer.callback = resend_action;
		Timer.Start(con, *this);

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

	Packet & Hazel::Packet::GetObject()
	{
		return object_pool.GetObject();
	}
}