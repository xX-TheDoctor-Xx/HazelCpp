#pragma once

#include "Hazel.hpp"

namespace Hazel
{
	class Bytes
	{
		byte *bytes;
		int length = -1;

	public:
		Bytes();
		Bytes(byte *bytes, int length);

		byte *GetBytes();
		int GetLength();

		void SetBytes(byte *bytes, int length);

		byte &operator[](int index);
		Bytes &operator=(const Bytes &other);

		bool IsValid();
	};
}