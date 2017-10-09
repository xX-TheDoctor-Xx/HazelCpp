#pragma once

#include "Hazel.hpp"
#include "Bytes.hpp"

namespace Hazel
{
	class Fragment
	{
	public:
		int FragmentID;
		Bytes Data;
		int Offset;

		Fragment(int fragment_id, Bytes data, int offset);

		bool operator<(Fragment &other);
	};
}