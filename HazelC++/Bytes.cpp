#include "Bytes.hpp"
#include "HazelException.hpp"

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
		return (length >= 0) ? bytes : nullptr;
	}

	int Bytes::GetLength()
	{
		return length;
	}

	void Bytes::SetBytes(byte * bytes, int length)
	{
		Clear();
	}

	byte & Bytes::operator[](int index)
	{
		if (index > GetLength())
			throw HazelException("Index out of range");

		return GetBytes()[index];
	}

	Bytes & Bytes::operator=(const Bytes & other)
	{
		Clear();

		bytes = other.bytes;
		length = other.length;

		return *this;
	}

	bool Bytes::IsValid()
	{
		return GetLength() >= 0;
	}

	void Bytes::Clear()
	{
		if (bytes)
		{
			delete[] bytes;
			bytes = nullptr;
		}
	}
}