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

		static ObjectPool<DataReceivedEventArgs> object_pool;
		static DataReceivedEventArgs CreateObject();

	public:
		DataReceivedEventArgs();
		DataReceivedEventArgs(const DataReceivedEventArgs &other);

		~DataReceivedEventArgs();

		Bytes GetBytes();
		SendOption GetSendOption();
		static DataReceivedEventArgs &GetObject(); // reference to the object taken from the pool

		void Set(Bytes &bytes, SendOption send_option);
	};
}