#include "UdpClientConnection.hpp"

namespace Hazel
{
	UdpClientConnection::UdpClientConnection(NetworkEndPoint remote_end_point) : data_buffer(new byte[65535], 65535), UdpSocket()
	{
		auto fn = [this, remote_end_point]()
		{
			SetEndPoint(remote_end_point);

			if (remote_end_point.is_v4())
			{
				Socket::create(SocketType::Udp, AddressFamily::IPv4);
			}
			else
			{
				Socket::create(SocketType::Udp, AddressFamily::IPv6);
				//Socket::option(41, 27, false); // no idea
			}
		};

		lock(socket_mutex, fn)
	}

	void UdpClientConnection::WriteBytesToConnection(Bytes bytes)
	{
		auto fn = [this]()
		{
			if (GetState() != ConnectionState::Connected && GetState() != ConnectionState::Connecting)
			{
				//throw new InvalidOperationException("Could not send data as this Connection is not connected and is not connecting. Did you disconnect?");
			}

			//int sent = send()
		};

		lock(socket_mutex, fn)
	}

	void hello_func()
	{
		/*lock(socket_mutex)
		{
			SetState(ConnectionState::Connected);
		}*/
	}

	void UdpClientConnection::Connect(Bytes bytes, int timeout)
	{
		auto fn = [this, bytes, timeout]()
		{
			if (GetState() != ConnectionState::NotConnected)
			{
				//throw new InvalidOperationException("Cannot connect as the Connection is already connected.");
			}

			SetState(ConnectionState::Connecting);

			try
			{
				UdpSocket::bind(std::string("0.0.0.0") + '0');
			}
			catch (HazelException&)
			{
				SetState(ConnectionState::NotConnected);
				//throw new HazelException("A socket exception occured while binding to the port.", e);
			}

			try
			{
				StartListeningForData();
			}
			catch (HazelException&)
			{
				SetState(ConnectionState::NotConnected);
				//throw new HazelException("A Socket exception occured while initiating a receive operation.", e);
			}

			GenericFunction<void> func;
			func.Set(hello_func);
			SendHello(bytes, func);

			bool timed_out = !WaitOnConnect(timeout);

			if (timed_out)
			{
				// throw new HazelException("Connection attempt timed out.");
			}
		};

		lock(socket_mutex, fn)
	}

	void UdpClientConnection::HandleDisconnect(HazelException &e)
	{
		bool invoke = false;

		auto fn = [this, invoke]()
		{
			if (GetState() == ConnectionState::Connected)
			{
				SetState(ConnectionState::Disconnecting);
				const_cast<bool&>(invoke) = true;
			}
		};

		lock(socket_mutex, fn)

		if (invoke)
		{
			InvokeDisconnected(e);
			Close();
		}
	}

	void UdpClientConnection::Close()
	{
		if (GetState() == ConnectionState::Connected)
			SendDisconnect();

		auto fn = [this]()
		{
			SetState(ConnectionState::NotConnected);
			UdpSocket::close();
		};

		lock(socket_mutex, fn)
	}

	void UdpClientConnection::StartListeningForData()
	{
		auto fn = [this]()
		{
			//socket.BeginReceive(dataBuffer, 0, dataBuffer.Length, SocketFlags.None, ReadCallback, dataBuffer);
		};

		lock(socket_mutex, fn)
	}
}