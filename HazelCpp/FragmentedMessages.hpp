#pragma once

#include "Fragment.hpp"

#include <vector>

namespace Hazel
{
	class FragmentedMessage
	{
	public:
		int no_fragments = -1;

		std::vector<Fragment> received;
	};
}