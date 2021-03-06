#include "Fragment.hpp"

namespace Hazel
{
	Fragment::Fragment(int fragment_id, Bytes &data, int offset) : FragmentID(fragment_id), Data(data), Offset(offset)
	{
	}

	bool Fragment::operator<(Fragment &other)
	{
		return FragmentID < other.FragmentID;
	}
}