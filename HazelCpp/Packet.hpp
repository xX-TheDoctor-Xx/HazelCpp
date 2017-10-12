#pragma once

#include "Hazel.hpp"
#include "Timer.hpp"
#include "Stopwatch.hpp"
#include "Bytes.hpp"
#include "ObjectPool.hpp"
#include "NetworkConnection.hpp"

//NetworkConnection here is always UdpConnection
namespace Hazel
{
	template<class TC>
	class Packet
	{
		static ObjectPool<Packet> object_pool;
		static Packet CreateObject();

		Bytes data;
		std::function<void()> ack_callback;
		std::atomic_int last_timeout;
		std::atomic_bool acknowledged;
		std::atomic_int retransmissions;
		Stopwatch stopwatch;

		NetworkConnection *con;

		Timer<TC> timer;
		std::mutex timer_mutex;

	public:
		Packet();
		Packet(const Packet &packet);

		~Packet();

		template<class T>
		void Set(Bytes &data, TC *resend_action, int timeout, std::function<void()> &ack_callback);
		Bytes GetData();
		int GetLastTimeout();
		void IncrementLastTimeout(int value);
		bool GetAcknowledged();
		void SetAcknowledged(bool value);
		int GetRetransmissions();
		void IncrementRetransmissions(int value);

		void InvokeAckCallback();
		void StopwatchStop();
		long long GetRoundTime();

		void Recycle();

		Timer<TC> &GetTimer();
		std::mutex &GetTimerMutex();

		static Packet &GetObject();
	};
}