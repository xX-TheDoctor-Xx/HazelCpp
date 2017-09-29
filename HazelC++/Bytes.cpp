#include "Bytes.hpp"

namespace Hazel
{
	Bytes::Bytes() : bytes(nullptr), length(-1)
	{
	}

	Bytes::Bytes(byte * bytes, int length) : bytes(bytes), length(length)
	{
	}

	byte * Bytes::GetBytes()
	{
		if (length >= 0)
			return bytes;
		return nullptr;
	}

	int Bytes::GetLength()
	{
		return length;
	}

	void Bytes::SetBytes(byte * bytes, int length)
	{
		if (bytes)
		{
			delete[] bytes;
			bytes = nullptr;
		}

		this->bytes = bytes;
		this->length = length;
	}

	byte & Bytes::operator[](int index)
	{
		if (index > GetLength())
		{
			//do something
		}

		return GetBytes()[index];
	}

	Bytes & Bytes::operator=(const Bytes & other)
	{
		if (bytes)
		{
			delete[] bytes;
			bytes = nullptr;
		}

		bytes = other.bytes;
		length = other.length;

		return *this;
	}

	bool Bytes::IsValid()
	{
		return GetLength() >= 0;
	}
}