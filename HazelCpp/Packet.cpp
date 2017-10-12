#include "Packet.hpp"

namespace Hazel
{
	template<class TC>
	Packet<TC> Packet<TC>::CreateObject()
	{
		return Packet();
	}

	template<class TC>
	Packet<TC>::Packet() : data(nullptr, -1), ack_callback(std::function<void()>())
	{
		object_pool.AssignObjectFactory(CreateObject);
	}

	template<class TC>
	Packet<TC>::Packet(const Packet &packet) : ack_callback(packet.ack_callback)
	{
		data = packet.data;
		last_timeout.exchange(packet.last_timeout);
		acknowledged.exchange(packet.acknowledged);
		retransmissions.exchange(packet.retransmissions);
		stopwatch = packet.stopwatch;
		object_pool.AssignObjectFactory(CreateObject);
	}

	template<class TC>
	Packet<TC>::~Packet()
	{
		Recycle();
	}

	template<class TC>
	void Packet<TC>::Set(Bytes &data, TC *resend_action, int timeout, std::function<void()> &ack_callback)
	{
		this->data = data;

		timer.SetInterval(timeout); // i dont think this is right
		timer.Start(resend_action);

		last_timeout = timeout;
		this->ack_callback = ack_callback;

		stopwatch.Start();
	}

	template<class TC>
	Bytes Packet<TC>::GetData()
	{
		return data;
	}

	template<class TC>
	int Packet<TC>::GetLastTimeout()
	{
		return last_timeout.load();
	}

	template<class TC>
	void Packet<TC>::IncrementLastTimeout(int value)
	{
		last_timeout += value;
	}

	template<class TC>
	bool Packet<TC>::GetAcknowledged()
	{
		return acknowledged.load();
	}

	template<class TC>
	void Packet<TC>::SetAcknowledged(bool value)
	{
		acknowledged = value;
	}

	template<class TC>
	int Packet<TC>::GetRetransmissions()
	{
		return retransmissions.load();
	}

	template<class TC>
	void Packet<TC>::IncrementRetransmissions(int value)
	{
		retransmissions += value;
	}

	template<class TC>
	void Packet<TC>::InvokeAckCallback()
	{
		ack_callback();
	}

	template<class TC>
	void Packet<TC>::StopwatchStop()
	{
		stopwatch.Stop();
	}

	template<class TC>
	long long Packet<TC>::GetRoundTime()
	{
		return stopwatch.GetElapsedMilliseconds();
	}

	template<class TC>
	void Packet<TC>::Recycle()
	{
		lock_mutex(timer_mutex, [this]()
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

	template<class TC>
	Timer<TC> &Packet<TC>::GetTimer()
	{
		return timer;
	}

	template<class TC>
	std::mutex & Packet<TC>::GetTimerMutex()
	{
		return timer_mutex;
	}

	template<class TC>
	Packet<TC> & Packet<TC>::GetObject()
	{
		return object_pool.GetObject();
	}
}