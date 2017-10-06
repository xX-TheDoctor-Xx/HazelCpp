#include "Packet.hpp"

namespace Hazel
{
	Packet Packet::CreateObject()
	{
		return Packet();
	}

	Packet::Packet() : data(nullptr, -1), ack_callback(std::function<void()>())
	{
		object_pool.AssignObjectFactory(CreateObject);
	}

	Packet::Packet(const Packet &packet) : ack_callback(packet.ack_callback)
	{
		data = packet.data;
		last_timeout.exchange(packet.last_timeout);
		acknowledged.exchange(packet.acknowledged);
		retransmissions.exchange(packet.retransmissions);
		stopwatch = packet.stopwatch;
		object_pool.AssignObjectFactory(CreateObject);
	}

	Packet::~Packet()
	{
		Recycle();
	}

	void Packet::Set(Bytes data, UdpConnection *con, std::function<void(UdpConnection*, Packet&)> &resend_action, int timeout, std::function<void()> &ack_callback)
	{
		this->data = data;

		timer.SetInterval(timeout); // i dont think this is right
		timer.callback = resend_action;
		timer.Start(con, *this);

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
		ack_callback();
	}

	void Packet::StopwatchStop()
	{
		stopwatch.Stop();
	}

	long long Packet::GetRoundTime()
	{
		return stopwatch.GetElapsedMilliseconds();
	}

	void Packet::Recycle()
	{
		lock(timer_mutex, [this]()
		{
			timer.Stop();
		});

		data.Clear();
		ack_callback = 0;
		last_timeout = 0;
		acknowledged = false;
		retransmissions = 0;
		//stopwatch is auto reseted when start is called
		object_pool.PutObject(*this);
	}

	Timer<UdpConnection*, Packet&>& Packet::GetTimer()
	{
		return timer;
	}

	std::mutex & Packet::GetTimerMutex()
	{
		return timer_mutex;
	}

	Packet & Hazel::Packet::GetObject()
	{
		return object_pool.GetObject();
	}
}