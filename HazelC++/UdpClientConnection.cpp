#include "UdpClientConnection.hpp"
#include "SocketException.hpp"

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

	void write_bytes_to_connection_callback(UdpClientConnection *con)
	{
		try
		{
			auto fn = [con]()
			{
				con->EndSendTo();
			};

			lock(con->socket_mutex, fn);
		}
		catch (SocketException&)
		{
			con->HandleDisconnect(HazelException("Could not send data a sa SocketException occured"));
		}
	}

	void UdpClientConnection::WriteBytesToConnection(Bytes bytes)
	{
		auto fn = [this, bytes]()
		{
			if (GetState() != ConnectionState::Connected && GetState() != ConnectionState::Connecting)
				throw HazelException("Could not send data as this Connection is not connected and is not connecting. Did you disconnect?");

			try
			{
				std::function<void(UdpClientConnection*)> func(write_bytes_to_connection_callback);
				Socket::BeginSendTo(bytes, GetEndpoint(), func, this);
			}
			catch (SocketException&)
			{
				HazelException ex("Could not send data as a SocketException occured.");
				HandleDisconnect(ex);
				throw ex;
			}
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
				throw HazelException("Cannot connect as the Connection is already connected.");

			SetState(ConnectionState::Connecting);

			try
			{
				UdpSocket::bind(std::string("0.0.0.0:0"));
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

			GenericFunction<void> func;
			func.Set(hello_func);
			SendHello(bytes, func);

			if (!WaitOnConnect(timeout))
			{
				Close();
				throw HazelException("Connection attempt timed out.");
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

	void read_callback(UdpClientConnection *con)
	{
		long bytes_received;

		try
		{
			auto fn = [con, bytes_received]()
			{
				const_cast<long&>(bytes_received) = con->EndReceiveFrom();
			};

			lock(con->socket_mutex, fn);
		}
		catch (SocketException&)
		{
			con->HandleDisconnect(HazelException("A socket exception occured while reading data."));
			return;
		}

		if (bytes_received == 0)
		{
			con->HandleDisconnect(HazelException());
			return;
		}

		Bytes bytes(new byte[bytes_received], bytes_received);
		Util::BlockCopy(con->data_buffer.GetBytes(), 0, bytes.GetBytes(), 0, bytes_received);

		try
		{
			con->StartListeningForData();
		}
		catch (SocketException&)
		{
			con->HandleDisconnect(HazelException());
		}

		con->HandleReceive(bytes);
	}

	void UdpClientConnection::StartListeningForData()
	{
		auto fn = [this]()
		{
			std::function<void(UdpClientConnection*)> func(read_callback);
			Socket::BeginReceiveFrom(data_buffer.GetBytes(), data_buffer.GetLength(), GetEndpoint(), func, this);
		};

		lock(socket_mutex, fn)
	}
}