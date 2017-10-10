#include "TcpConnection.hpp"
#include "SocketException.hpp"
#include "Util.hpp"

namespace Hazel
{
	TcpConnection::TcpConnection(const TcpClient &client) : TcpClient(client)
	{
		auto fn = [this, client]()
		{
			TcpClient &nonconst_client = const_cast<TcpClient&>(client);
			SetEndPoint(const_cast<NetworkEndPoint&>(nonconst_client.ip()));
			nonconst_client.nodelay(true);

			SetState(ConnectionState::Connected);
		};

		lock_mutex(socket_mutex, fn)
	}

	TcpConnection::TcpConnection(NetworkEndPoint end_point)
	{
		auto fn = [this, end_point]()
		{
			if (GetState() != ConnectionState::NotConnected)
			{
				//throw new InvalidOperationException("Cannot connect as the Connection is already connected.");
			}

			SetEndPoint(const_cast<NetworkEndPoint&>(end_point));

			SetState(ConnectionState::Connected);
		};

		lock_mutex(socket_mutex, fn)
	}

	void TcpConnection::Connect(Bytes & bytes, int timeout)
	{
		auto fn = [this, bytes, timeout]()
		{
			SetState(ConnectionState::Connecting);

			Bytes &nonconst_bytes = const_cast<Bytes&>(bytes);

			try
			{
				//connect
			}
			catch (SocketException&)
			{
				throw HazelException("Could not connect as an exception occured.");
			}

			try
			{
				StartWaitingForHeader(body_read_callback_client);
			}
			catch (SocketException&)
			{
				throw HazelException("An exception occured while initiating the first receive operation.");
			}

			Bytes actual_bytes;
			if (nonconst_bytes.IsValid())
				actual_bytes = Bytes(new byte[1](), 1);
			else
			{
				actual_bytes = Bytes(new byte[nonconst_bytes.GetLength() + 1], nonconst_bytes.GetLength() + 1);
				Util::BlockCopy(nonconst_bytes.GetBytes(), 0, actual_bytes.GetBytes(), 1, nonconst_bytes.GetLength());
			}

			SetState(ConnectionState::Connected);
			TcpConnection::SendBytes(actual_bytes);
		};

		lock_mutex(socket_mutex, fn)
	}

	void TcpConnection::SendBytes(Bytes & bytes, SendOption send_option)
	{
		Bytes full_bytes = AppendLengthHeader(bytes);

		auto fn = [this, bytes, send_option]()
		{
			if (GetState() != ConnectionState::Connected)
				throw HazelException("Could not send data as this Connection is not connected. Did you disconnect?");

			try
			{
				//socket begin send
			}
			catch (SocketException&)
			{
				HazelException ex("");
				HandleDisconnect(ex);
				throw ex;
			}
		};

		lock_mutex(socket_mutex, fn)

		GetStatistics().LogFragmentedSend(bytes.GetLength(), full_bytes.GetLength());
	}

	void TcpConnection::HeaderReadCallback(Bytes &bytes, std::function<void(Bytes&)> &callback)
	{
		int length = GetLengthFromBytes(bytes);

		try
		{

		}
		catch (SocketException&)
		{
			HandleDisconnect(HazelException("An exception occured while initiating a body receive operation."));
		}
	}

	void TcpConnection::BodyReadCallback(Bytes &bytes)
	{
		try
		{
			StartWaitingForHeader(BodyReadCallback);
		}
		catch (SocketException&)
		{
			HandleDisconnect(HazelException("An exception occured while initiating a header receive operation."));
		}

		GetStatistics().LogFragmentedReceive(bytes.GetLength(), bytes.GetLength() + 4);

		InvokeDataReceived(bytes, SendOption::FragmentedReliable);
	}

	void TcpConnection::StartReceiving()
	{
		try
		{

		}
		catch (SocketException&)
		{
			HandleDisconnect(HazelException("An exception occured while initiating the first receive operation."));
		}
	}

	void start_waiting_for_handshake_fn(Bytes &bytes, std::function<void(Bytes&)> &callback)
	{
		Bytes data_bytes(new byte[bytes.GetLength() - 1](), bytes.GetLength() - 1);
		Util::BlockCopy(bytes.GetBytes(), 1, data_bytes.GetBytes(), 0, bytes.GetLength() - 1);

		callback(data_bytes);
	}

	void TcpConnection::StartWaitingForHandshake(std::function<void(TcpConnection*, Bytes&)> &callback)
	{
		try
		{
			std::function<void(Bytes&, std::function<void(Bytes&)>&)> fn(start_waiting_for_handshake_fn);
			StartWaitingForHeader(fn);
		}
		catch (SocketException&)
		{
			HandleDisconnect(HazelException("An exception occured while initiating the first receive operation."));
		}
	}
}