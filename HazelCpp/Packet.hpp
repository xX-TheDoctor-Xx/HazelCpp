#pragma once

#include "Hazel.hpp"
#include "Timer.hpp"
#include "Stopwatch.hpp"
#include "Bytes.hpp"
#include "ObjectPool.hpp"

#include <atomic>

namespace Hazel
{
	class UdpConnection;

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

		UdpConnection *con;

		Timer<UdpConnection*, Packet&> timer;
		std::mutex timer_mutex;

	public:
		Packet();
		Packet(const Packet &packet);

		~Packet();

		void Set(Bytes &data, UdpConnection *con, std::function<void(UdpConnection*, Packet&)> &resend_action, int timeout, std::function<void()> &ack_callback);
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

		Timer<UdpConnection*, Packet&> &GetTimer();
		std::mutex &GetTimerMutex();

		static Packet &GetObject();
	};
}