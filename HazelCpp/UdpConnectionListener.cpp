#include "UdpConnectionListener.hpp"
#include "SocketException.hpp"
#include "HazelUtil.hpp"

namespace Hazel
{
	UdpConnectionListener::UdpConnectionListener(NetworkEndPoint end_point) : UdpSocket()
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

	void udp_read_callback_listener(Socket *listener)
	{
		long bytes_received = 0;
		NetworkEndPoint remote_end_point;
		if (((UdpConnectionListener*)(listener))->GetEndPoint().is_v4())
			remote_end_point = NetworkEndPoint("0.0.0.0:0");
		else
			remote_end_point = NetworkEndPoint("[::]:0");
		Bytes bytes;

		try
		{
			bytes = listener->EndReceiveFrom();
			return;
		}
		catch (HazelException&)
		{

		}

		if (bytes_received == 0)
			return;

		((UdpConnectionListener*)(listener))->StartListeningForData();

		bool aware;
		UdpServerConnection *connection;

		auto fn = [listener, aware, connection, remote_end_point, bytes]()
		{
			std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>> &nonconst_connections = const_cast<std::map<int, std::pair<NetworkEndPoint, UdpServerConnection*>>&>(((UdpConnectionListener*)(listener))->connections);
			UdpServerConnection* nonconst_connection = const_cast<UdpServerConnection*>(connection);

			int index = HazelUtil::LookForEndPoint(nonconst_connections, const_cast<NetworkEndPoint&>(remote_end_point));
			const_cast<bool&>(aware) = nonconst_connections.find(index) != nonconst_connections.end();

			if (aware)
				nonconst_connection = nonconst_connections.at(index).second;
			else
			{
				if (const_cast<Bytes&>(bytes)[0] != (byte)UdpSendOption::Hello)
					return;

				nonconst_connection = new UdpServerConnection(((UdpConnectionListener*)(listener)), const_cast<NetworkEndPoint&>(remote_end_point));
				nonconst_connections.emplace(nonconst_connections.size(), std::make_pair(remote_end_point, nonconst_connection)); // idk if this will work
			}
		};

		lock_mutex(((UdpConnectionListener*)(listener))->connections_mutex, fn)

		connection->HandleReceive(bytes);

		if (!aware)
		{
			//Bytes new_data_buffer(new byte[bytes.GetLength() - 1], bytes.GetLength() - 1);
			//Util::BlockCopy(bytes.GetBytes(), 1, new_data_buffer.GetBytes(), 0, bytes.GetLength() - 1);
			((UdpConnectionListener*)(listener))->InvokeNewConnection(bytes, connection);
		}
	}

	void UdpConnectionListener::StartListeningForData()
	{
		auto fn = [this]()
		{
			BeginReceiveFrom(65535, GetEndPoint(), std::function<void(Socket*)>(udp_read_callback_listener), (Socket*)this);
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

	void send_data_callback_listener(Socket *soc)
	{
		soc->EndSendTo();
	}

	void UdpConnectionListener::SendData(Bytes bytes, NetworkEndPoint end_point)
	{
		try
		{
			auto fn = [this, bytes, end_point]()
			{
				std::function<void(Socket*)> func(send_data_callback_listener);
				Bytes &nonconst_bytes = const_cast<Bytes&>(bytes);
				BeginSendTo(nonconst_bytes.GetBytes(), nonconst_bytes.GetLength(), const_cast<NetworkEndPoint&>(end_point), func, (Socket*)this);
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
			int index = HazelUtil::LookForEndPoint(connections, const_cast<NetworkEndPoint&>(end_point));
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