#include "UdpClientConnection.hpp"

namespace Hazel
{
	UdpClientConnection::UdpClientConnection(NetworkEndPoint remote_end_point) : data_buffer(new byte[65535], 65535)
	{
		lock(socket_mutex)
		{
			SetEndPoint(remote_end_point);

			if ((soc_ptr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != INVALID_SOCKET)
			{
				//throw whatever
			}
		}
	}

	void UdpClientConnection::WriteBytesToConnection(Bytes bytes)
	{
		lock(socket_mutex)
		{
			if (GetState() != ConnectionState::Connected && GetState() != ConnectionState::Connecting)
			{
				//throw new InvalidOperationException("Could not send data as this Connection is not connected and is not connecting. Did you disconnect?");
			}

			//int sent = send()
		}
	}

	void UdpClientConnection::Connect(Bytes bytes, int timeout)
	{
		lock(socket_mutex)
		{
			if (GetState() != ConnectionState::NotConnected)
			{
				//throw new InvalidOperationException("Cannot connect as the Connection is already connected.");
			}

			SetState(ConnectionState::Connecting);



			try
			{
				StartListeningForData();
			}
			catch (std::exception&)
			{

			}

			auto func = []()
			{
				// set connection state to connected
			};

			//SendHello(bytes, GenericFunction<void>(func));

			bool timed_out = !WaitOnConnect(timeout);

			if (timed_out)
			{
				// throw new HazelException("Connection attempt timed out.");
			}
		}
	}

	void UdpClientConnection::StartListeningForData()
	{
		lock(socket_mutex)
		{
			//socket.BeginReceive(dataBuffer, 0, dataBuffer.Length, SocketFlags.None, ReadCallback, dataBuffer);
		}
	}
}