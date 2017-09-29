#pragma once

#include "ConnectionListener.hpp"

namespace Hazel
{
	class NetworkConnectionListener : public ConnectionListener
	{
		NetworkEndPoint end_point;

	public:
		NetworkEndPoint GetEndPoint();

	protected:
		void SetEndPoint(NetworkEndPoint end_point);
	};
}