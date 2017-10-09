#include "DataReceivedEventArgs.hpp"

namespace Hazel
{
	DataReceivedEventArgs::DataReceivedEventArgs()
	{
		object_pool.AssignObjectFactory(CreateObject);
	}

	DataReceivedEventArgs::DataReceivedEventArgs(const DataReceivedEventArgs &other)
	{
		bytes = other.bytes;
		send_option = other.send_option;
	}

	DataReceivedEventArgs::~DataReceivedEventArgs()
	{
		send_option = SendOption::None;
		object_pool.PutObject(*this);
	}

	DataReceivedEventArgs DataReceivedEventArgs::CreateObject()
	{
		return DataReceivedEventArgs();
	}

	Bytes DataReceivedEventArgs::GetBytes()
	{
		return bytes;
	}

	SendOption DataReceivedEventArgs::GetSendOption()
	{
		return send_option;
	}

	void DataReceivedEventArgs::Set(Bytes &bytes, SendOption send_option)
	{
		this->bytes = bytes;
		this->send_option = send_option;
	}

	DataReceivedEventArgs &DataReceivedEventArgs::GetObject()
	{
		return object_pool.GetObject();
	}
}