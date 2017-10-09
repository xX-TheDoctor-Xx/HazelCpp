#include "UdpConnectionListener.hpp"
#include "SocketException.hpp"
#include "Util.hpp"

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
			lock_mutex(listener_mutex, fn)
		}
		catch (SocketException&)
		{
			throw HazelException("Could not start listening as a SocketException occured");
		}

		StartListeningForData();
	}

	void read_callback(UdpConnectionListener * listener, Bytes &bytes)
	{
		long bytes_received = 0;
		NetworkEndPoint remote_end_point("0.0.0.0:0");

		try
		{
			listener->StartListeningForData();
			return;
		}
		catch (HazelException&)
		{

		}

		if (bytes_received == 0)
			return;

		Bytes buffer(new byte[bytes_received], bytes_received);
		Util::BlockCopy(bytes.GetBytes(), 0, buffer.GetBytes(), 0, bytes_received);

		listener->StartListeningForData();

		bool aware;
		UdpServerConnection *connection;

		auto fn = [listener, aware, connection, remote_end_point, buffer]()
		{
			std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> &nonconst_connections = const_cast<std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>>&>(listener->connections);
			UdpServerConnection* nonconst_connection = const_cast<UdpServerConnection*>(connection);

			int index = Util::LookForEndPoint(nonconst_connections, const_cast<NetworkEndPoint&>(remote_end_point));
			const_cast<bool&>(aware) = nonconst_connections.find(index) != nonconst_connections.end();

			if (aware)
				nonconst_connection = nonconst_connections.at(index).second;
			else
			{
				if (const_cast<Bytes&>(buffer)[0] != (byte)UdpSendOption::Hello)
					return;

				nonconst_connection = new UdpServerConnection(const_cast<UdpConnectionListener*>(listener), const_cast<NetworkEndPoint&>(remote_end_point));
				nonconst_connections.emplace(nonconst_connections.size(), std::make_pair(remote_end_point, nonconst_connection)); // idk if this will work
			}
		};

		lock_mutex(listener->connections_mutex, fn)

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

		};

		try
		{
			lock_mutex(listener_mutex, fn);
		}
		catch (SocketException&)
		{
			StartListeningForData();
			return;
		}
	}

	void UdpConnectionListener::SendData(Bytes bytes, NetworkEndPoint end_point)
	{
		try
		{
			auto fn = [this, bytes, end_point]()
			{
				//std::function<void(UdpConnectionListener*)> func(send_data_callback);
				//Socket::BeginSendTo(const_cast<Bytes&>(bytes), const_cast<NetworkEndPoint&>(end_point), func, this);
			};

			lock_mutex(listener_mutex, fn)
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
			int index = Util::LookForEndPoint(connections, const_cast<NetworkEndPoint&>(end_point));
			if (index != -1)
				connections.erase(index);
		};

		lock_mutex(connections_mutex, fn)
	}

	void UdpConnectionListener::Close()
	{
		auto fn = [this]()
		{
			UdpSocket::close();
		};

		lock_mutex(listener_mutex, fn)
	}
}