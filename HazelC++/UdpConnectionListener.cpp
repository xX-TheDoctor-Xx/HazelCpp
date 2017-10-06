#include "UdpConnectionListener.hpp"
#include "SocketException.hpp"

namespace Hazel
{
	UdpConnectionListener::UdpConnectionListener(NetworkEndPoint end_point) : data_buffer(new byte[65535](), 65535), UdpSocket()
	{
		SetEndPoint(end_point);
		//this.listener.SetSocketOption(SocketOptionLevel.IPv6, (SocketOptionName)27, false);????
	}

	void UdpConnectionListener::Start()
	{
		auto fn = [this]()
		{
			UdpSocket::bind(GetEndPoint());
		};

		try
		{
			lock(listener_mutex, fn)
		}
		catch (SocketException&)
		{
			throw HazelException("Could not start listening as a SocketException occured");
		}

		StartListeningForData();
	}

	void read_callback(UdpConnectionListener * listener, Bytes &bytes)
	{
		long bytes_received;
		NetworkEndPoint remote_end_point("0.0.0.0:0");

		try
		{
			auto fn = [listener, bytes_received]()
			{
				const_cast<long&>(bytes_received) = listener->EndReceiveFrom();
			};

			lock(listener->listener_mutex, fn);
		}
		catch (SocketException&)
		{
			listener->StartListeningForData();
			return;
		}

		if (bytes_received == 0)
			return;

		Bytes buffer(new byte[bytes_received], bytes_received);
		Util::BlockCopy(bytes.GetBytes(), 0, buffer.GetBytes(), 0, bytes_received);

		listener->StartListeningForData();

		bool aware;
		UdpServerConnection *connection;

		std::map<NetworkEndPoint, UdpServerConnection*> &connections = listener->connections;

		auto fn = [listener, aware, connection, remote_end_point, connections, buffer]()
		{
			std::map<NetworkEndPoint, UdpServerConnection*> &nonconst_connections = const_cast<std::map<NetworkEndPoint, UdpServerConnection*>&>(connections);
			UdpServerConnection* nonconst_connection = const_cast<UdpServerConnection*>(connection);

			const_cast<bool&>(aware) = nonconst_connections.find(remote_end_point) != nonconst_connections.end();

			if (aware)
				nonconst_connection = nonconst_connections.at(remote_end_point);
			else
			{
				if (const_cast<Bytes&>(buffer)[0] != (byte)UdpSendOption::Hello)
					return;

				nonconst_connection = new UdpServerConnection(listener, remote_end_point);
				nonconst_connections.insert(std::make_pair(remote_end_point, nonconst_connection));
			}
		};

		lock(listener->connections_mutex, fn)

		connection->HandleReceive(buffer);

		if (!aware)
		{
			Bytes new_data_buffer(new byte[buffer.GetLength() - 1], buffer.GetLength() - 1);
			Util::BlockCopy(buffer.GetBytes(), 1, new_data_buffer.GetBytes(), 0, buffer.GetLength() - 1);
			listener->InvokeNewConnection(new_data_buffer, connection);
		}
	}

	void UdpConnectionListener::StartListeningForData()
	{
		auto fn = [this]()
		{
			std::function<void(UdpConnectionListener*, Bytes &bytes)> func(read_callback);
			Socket::BeginReceiveFrom(data_buffer.GetBytes(), data_buffer.GetLength(), GetEndPoint(), func, this, data_buffer);
		};

		try
		{
			lock(listener_mutex, fn);
		}
		catch (SocketException&)
		{
			StartListeningForData();
			return;
		}
	}

	void send_data_callback(UdpConnectionListener *listener)
	{
		listener->EndSendTo();
	}

	void UdpConnectionListener::SendData(Bytes bytes, NetworkEndPoint end_point)
	{
		try
		{
			auto fn = [this, bytes, end_point]()
			{
				std::function<void(UdpConnectionListener*)> func(send_data_callback);
				Socket::BeginSendTo(const_cast<Bytes&>(bytes), const_cast<NetworkEndPoint&>(end_point), func, this);
			};

			lock(listener_mutex, fn)
		}
		catch (SocketException&)
		{
			throw HazelException("Could not send data as a SocketException occured.");
		}
	}

	void UdpConnectionListener::RemoveConnectionTo(NetworkEndPoint end_point)
	{
		auto fn = [this, end_point]()
		{
			connections.erase(end_point);
		};

		lock(connections_mutex, fn)
	}

	void UdpConnectionListener::Close()
	{
		auto fn = [this]()
		{
			UdpSocket::close();
		};

		lock(listener_mutex, fn)
	}
}