#include "UdpClientConnection.hpp"
#include "SocketException.hpp"
#include "Util.hpp"

namespace Hazel
{
	UdpClientConnection::UdpClientConnection(NetworkEndPoint &remote_end_point) :  UdpSocket()
	{
		auto fn = [this, remote_end_point]()
		{
			SetEndPoint(const_cast<NetworkEndPoint&>(remote_end_point));

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

		lock_mutex(socket_mutex, fn)
	}

	void udp_write_bytes_to_connection_callback(Socket *con)
	{
		auto fn = [con]()
		{
			try
			{
				con->EndSendTo();
			}
				catch (SocketException&)
			{
				((UdpClientConnection*)(con))->HandleDisconnect(HazelException("Could not send data a sa SocketException occured"));
			}
		};

		lock_mutex(con->socket_mutex, fn)
	}

	void UdpClientConnection::WriteBytesToConnection(Bytes &bytes)
	{
		auto fn = [this, bytes]()
		{
			if (GetState() != ConnectionState::Connected && GetState() != ConnectionState::Connecting)
				throw HazelException("Could not send data as this Connection is not connected and is not connecting. Did you disconnect?");

			try
			{
				Bytes &nonconst_bytes = const_cast<Bytes&>(bytes);
				UdpSocket::BeginSendTo(nonconst_bytes.GetBytes(), nonconst_bytes.GetLength(), GetEndPoint(), std::function<void(Socket *con)>(udp_write_bytes_to_connection_callback), (Socket*)this);
			}
			catch (SocketException&)
			{
				HazelException ex("Could not send data as a SocketException occured.");
				HandleDisconnect(ex);
				throw ex;
			}
		};

		lock_mutex(socket_mutex, fn)
	}

	void udp_hello_func(UdpClientConnection *con)
	{
		auto fn = [con]()
		{
			con->SetState(ConnectionState::Connected);
		};

		lock_mutex(con->socket_mutex, fn)
	}

	void UdpClientConnection::Connect(Bytes &bytes, int timeout)
	{
		auto fn = [this, bytes, timeout]()
		{
			if (GetState() != ConnectionState::NotConnected)
				throw HazelException("Cannot connect as the Connection is already connected or connecting.");

			SetState(ConnectionState::Connecting);

			try
			{
				if (GetEndPoint().is_v4())
					UdpSocket::bind(std::string("0.0.0.0:0"));
				else
					UdpSocket::bind(std::string("[::]:0"));
			}
			catch (HazelException&)
			{
				SetState(ConnectionState::NotConnected);
				throw HazelException("A socket exception occured while binding to the port.");
			}

			try
			{
				StartListeningForData();
			}
			catch (HazelException&)
			{
				Close();
				throw HazelException("A Socket exception occured while initiating a receive operation.");
			}

			SendHello(const_cast<Bytes&>(bytes), std::function<void(UdpClientConnection*)>(udp_hello_func), this);

			if (!WaitOnConnect(timeout))
			{
				Close();
				throw HazelException("Connection attempt timed out.");
			}
		};

		lock_mutex(socket_mutex, fn)
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

		lock_mutex(socket_mutex, fn)

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

		lock_mutex(socket_mutex, fn)
	}

	void udp_read_callback_client(Socket *con)
	{
		long bytes_received;
		Bytes bytes;

		try
		{
			auto fn = [con, bytes_received, bytes]()
			{
				const_cast<Bytes&>(bytes) = con->EndReceiveFrom();
			};

			lock_mutex(con->socket_mutex, fn);
		}
		catch (SocketException&)
		{
			((UdpClientConnection*)con)->HandleDisconnect(HazelException("A socket exception occured while reading data."));
			return;
		}

		if (bytes_received == 0)
		{
			((UdpClientConnection*)con)->HandleDisconnect(HazelException());
			return;
		}

		try
		{
			((UdpClientConnection*)con)->StartListeningForData();
		}
		catch (SocketException&)
		{
			((UdpClientConnection*)con)->HandleDisconnect(HazelException());
		}

		((UdpClientConnection*)con)->HandleReceive(bytes);
	}

	void UdpClientConnection::StartListeningForData()
	{
		auto fn = [this]()
		{
			BeginReceiveFrom(65535, GetEndPoint(), std::function<void(Socket*)>(udp_read_callback_client), (Socket*)this);
		};

		lock_mutex(socket_mutex, fn)
	}
}