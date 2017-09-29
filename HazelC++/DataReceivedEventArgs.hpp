#pragma once

#include <queue>

#include "Hazel.hpp"
#include "ObjectPool.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class DataReceivedEventArgs
	{
		Bytes bytes;
		SendOption send_option;

	public:
		static ObjectPool<DataReceivedEventArgs> ObjectPool;
		static DataReceivedEventArgs CreateObject();

		DataReceivedEventArgs();
		DataReceivedEventArgs(const DataReceivedEventArgs &other);

		~DataReceivedEventArgs();

		Bytes GetBytes();
		SendOption GetSendOption();
		DataReceivedEventArgs &GetObject(); // reference to the object taken from the pool

		void Set(Bytes bytes, SendOption send_option);
	};
}