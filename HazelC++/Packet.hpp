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
		GenericFunction<void> ack_callback;
		std::atomic_int last_timeout;
		std::atomic_bool acknowledged;
		std::atomic_int retransmissions;
		Stopwatch stopwatch;

		UdpConnection *con;

	public:
		Timer<UdpConnection*, Packet&> Timer;
		std::mutex TimerMutex;

		Packet();
		Packet(const Packet &packet);

		~Packet();

		void Set(Bytes data, UdpConnection *con, GenericFunction<void, UdpConnection*, Packet&> &resend_action, int timeout, GenericFunction<void> &ack_callback);
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

		static Packet &GetObject();
	};
}