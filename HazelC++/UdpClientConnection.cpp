#include "UdpClientConnection.hpp"
#include "SocketException.hpp"
#include "Util.hpp"

namespace Hazel
{
	UdpClientConnection::UdpClientConnection(NetworkEndPoint remote_end_point, IPMode mode) : data_buffer(new byte[65535], 65535), mode(mode), UdpSocket()
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
				//con->EndSendTo();
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
				//con->SendToAsync();
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

	void hello_func(UdpClientConnection *con)
	{
		auto fn = [con]()
		{
			con->SetState(ConnectionState::Connected);
		};

		lock(con->socket_mutex, fn)
	}

	void UdpClientConnection::Connect(Bytes bytes, int timeout)
	{
		auto fn = [this, bytes, timeout]()
		{
			if (GetState() != ConnectionState::NotConnected)
				throw HazelException("Cannot connect as the Connection is already connected or connecting.");

			SetState(ConnectionState::Connecting);

			try
			{
				if (mode == IPMode::IPv4)
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

			SendHello(bytes, std::function<void(UdpClientConnection*)>(hello_func), this);

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

	void read_callback(UdpClientConnection *con, Bytes &bytes, bool has_error)
	{
		long bytes_received;

		try
		{
			auto fn = [con, bytes_received]()
			{
				//const_cast<long&>(bytes_received) = con->EndReceiveFrom();
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

		Bytes new_bytes(new byte[bytes_received], bytes_received);
		Util::BlockCopy(con->data_buffer.GetBytes(), 0, new_bytes.GetBytes(), 0, bytes_received);

		try
		{
			con->StartListeningForData();
		}
		catch (SocketException&)
		{
			con->HandleDisconnect(HazelException());
		}

		con->HandleReceive(new_bytes);
	}

	void UdpClientConnection::StartListeningForData()
	{
		auto fn = [this]()
		{

		};

		lock(socket_mutex, fn)
	}
}