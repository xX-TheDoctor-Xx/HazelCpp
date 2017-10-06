#include "Socket.hpp"
#include "SocketException.hpp"
#include "Stopwatch.hpp"

#undef lock
#include <future>

namespace Hazel
{
	Socket::Socket() : id_(INVALID_SOCKET), errno_(0), timeout_(100), checksent_(false), listens_(false)
	{
	}

	Socket::Socket(SOCKET id) : id_(id), errno_(0), timeout_(100), checksent_(false), listens_(false)
	{
	}

	Socket::Socket(const NetworkEndPoint & adr) : id_(INVALID_SOCKET), ip_(adr), errno_(0), timeout_(100), checksent_(false), listens_(false)
	{
	}

	Socket::Socket(SOCKET id, const NetworkEndPoint & adr) : id_(id), ip_(adr), errno_(0), timeout_(100), checksent_(false), listens_(false)
	{
	}

	Socket::Socket(const char * adr) : id_(INVALID_SOCKET), ip_(adr), errno_(0), timeout_(100), checksent_(false), listens_(false)
	{
	}

	Socket::~Socket()
	{
		close();
	}

	bool Socket::closed() const 	
	{
		return id_ == INVALID_SOCKET;
	}

	SOCKET Socket::socket_id() const
	{
		return id_;
	}

	const NetworkEndPoint & Socket::ip() const
	{
		return ip_;
	}

	void Socket::ip(const NetworkEndPoint & adr)
	{
		ip_ = adr;
	}

	void Socket::ip(const char * adr)
	{
		if (adr)
			ip_ = adr;
	}

	long Socket::error() const 
	{
		return errno_;
	}

	void Socket::error(long newerror)
	{
		errno_ = newerror;
		if (newerror == -1)
			return;
		if (newerror == 0)
		{
			errtext_ = "";
			return;
		}
		errtext_ = syserr_text(newerror);
	}

	const std::string & Socket::error_text() const
	{
		return errtext_;
	}

	void Socket::error_text(const char * text, bool overwrite)
	{
		if (!overwrite && !errtext_.empty()) return;
		errno_ = -1; errtext_ = text;
	}

	unsigned Socket::timeout_ms() const 
	{
		return timeout_;
	}

	void Socket::timeout_ms(unsigned newtimeout) 
	{
		timeout_ = newtimeout;
	}

	int Socket::option(int level, int optname)
	{
		int i = 0;
		socklen_t size = sizeof(i);
		if (::getsockopt(id_, level, optname, (char*)&i, &size) == SOCKET_ERROR) error(SOCKET_ERRNO);
		return i;
	}

	void Socket::option(int level, int optname, int newval)
	{
		int i = newval;
#ifndef _WIN32
		analyze_error(::setsockopt(id_, level, optname, (void*)&i, (socklen_t) sizeof(i))); 
#else
		analyze_error(::setsockopt(id_, level, optname, (const char*)&i, (socklen_t) sizeof(i)));
#endif
	}

	int Socket::option_sendbuffer_size()
	{
		return option(SOL_SOCKET, SO_SNDBUF);
	}

	int Socket::option_recvbuffer_size()
	{
		return option(SOL_SOCKET, SO_RCVBUF);
	}

	bool Socket::option_error()
	{
		if (option(SOL_SOCKET, SO_ERROR) != 0) { close(); return true; } return false;
	}

	void Socket::nodelay(bool enabled)
	{
		option(IPPROTO_TCP, TCP_NODELAY, enabled ? 1 : 0);
	}

	bool Socket::nodelay()
	{
		return option(IPPROTO_TCP, TCP_NODELAY) != 0;
	}

	void Socket::keepalive(bool enabled)
	{
		option(SOL_SOCKET, SO_KEEPALIVE, enabled ? 1 : 0);
	}

	bool Socket::keepalive()
	{
		return option(SOL_SOCKET, SO_KEEPALIVE) != 0;
	}

	void Socket::reuseaddress(bool enabled)
	{
		option(SOL_SOCKET, SO_REUSEADDR, enabled ? 1 : 0);
	}

	bool Socket::reuseaddress()
	{
		return option(SOL_SOCKET, SO_REUSEADDR) != 0;
	}

	void Socket::nonblocking(bool enabled)
	{
#ifdef _WIN32
		u_long d = enabled ? 1 : 0;
		ioctlsocket(id_, FIONBIO, &d);
#elif defined(O_NONBLOCK)
		int o = ::fcntl(id_, F_GETFL, 0);
		if (o < 0) return;
		if (enabled) o |= O_NONBLOCK; else o &= (~O_NONBLOCK);
		::fcntl(id_, F_SETFL, o);
#elif defined(SO_NONBLOCK)
		long d = enabled ? 1 : 0;
		::setsockopt(id_, SOL_SOCKET, SO_NONBLOCK, &d, sizeof(d));
#endif
	}

	bool Socket::nonblocking()
	{
#if defined(O_NONBLOCK)
		int o = ::fcntl(id_, F_GETFL, 0);
		return (o > 0) && (o & O_NONBLOCK) != 0;
#elif defined(SO_NONBLOCK)
		long d;
		::getsockopt(id_, SOL_SOCKET, SO_NONBLOCK, &d, sizeof(d));
		return d != 0;
#endif
		return false;
	}

	bool Socket::create(SocketType type, AddressFamily family)
	{
		if (family == AddressFamily::Any) {
			if (!ip_.valid()) {
				error_text("basic_socket::create(): Cannot create socket from invalid ip", true);
				return false;
			}
			family = (AddressFamily)ip_.family();
		}
		id_ = ::socket((int)family, (int)type, 0);
		if (id_ == INVALID_SOCKET) {
			analyze_error(SOCKET_ERRNO);
			error_text("basic_socket::create(): Failed to create socket");
			return false;
		}
		return true;
	}

	bool Socket::connect()
	{
		if (!ip_.valid()) return false;
		typename sockaddr_storage adr = ip_.address();
		analyze_error(::connect(id_, (sockaddr*)(&adr), (socklen_t)ip_.size()));
		if (error()) { close(); return false; }
		nonblocking(false);
		return true;
	}

	bool Socket::bind()
	{
		if (!ip_.valid()) return false;
		typename sockaddr_storage adr = ip_.address();
		analyze_error(::bind(id_, (sockaddr*)(&adr), (socklen_t)ip_.size()));
		if (error()) { close(); return false; }
		return true;
	}

	bool Socket::listen(int max_listen_pending)
	{
		analyze_error(::listen(id_, max_listen_pending));
		if (error()) { close(); return false; }
		nonblocking(true);
		listens_ = true;
		return true;
	}

	bool Socket::accept(Socket & acc)
	{
		if (!acc.closed()) acc.close();
		acc.ip_.clear();
		NetworkEndPoint tip;
		typename sockaddr_storage sa;
		typename socklen_t lsa = sizeof(sa);
		SOCKET id = ::accept(id_, (sockaddr*)(&sa), &lsa);
		if (id == INVALID_SOCKET || !sa.ss_family || !lsa) return false;
		acc.id_ = id;
		if (sa.ss_family == AF_INET) {
			acc.ip_ = *((sockaddr_in*)&sa);
		}
		else if (sa.ss_family == AF_INET6) {
			acc.ip_ = *((sockaddr_in6*)&sa);
		}
		else {
			acc.ip_.address(sa);
			acc.ip_.size(lsa);
		}
		nonblocking(false);
		return true;
	}

	void Socket::close()
	{
		if (id_ == INVALID_SOCKET) return;
#ifndef _WIN32
		::close(id_);
#else
		shutdown(id_, SD_BOTH); closesocket(id_);
#endif
		id_ = INVALID_SOCKET;
	}

	bool Socket::wait(bool * is_recv, bool * is_sent, int timeout_ms)
	{
		if (closed() || ((!is_recv) && (!is_sent))) return false;
		if (is_recv) *is_recv = false;
		if (is_sent) *is_sent = false;
		if (!checksent_) is_sent = nullptr;
		checksent_ = false;
#ifndef _WIN32
		struct pollfd fd;
		fd.fd = id_;
		fd.events = (is_recv ? POLLIN : 0) | ((checksent_ && is_sent) ? POLLOUT : 0);
		fd.revents = 0;
		checksent_ = false;
		if (!fd.events) return false;
		int r = ::poll(&fd, 1, timeout_ms);
		if (r<0) { analyze_error(r); return false; }
		if (!r || !fd.revents) return false;
		if (is_recv && (fd.revents & POLLIN)) *is_recv = true;
		if (is_sent && (fd.revents & POLLOUT)) *is_sent = true;
#else
		struct timeval to;
		struct fd_set ifd, ofd;
		to.tv_sec = timeout_ms / 1000;
		to.tv_usec = timeout_ms % 1000;
		FD_ZERO(&ifd); FD_ZERO(&ofd);
		if (is_recv) FD_SET(id_, &ifd);
		if (is_sent) FD_SET(id_, &ofd);
		int r = ::select(id_ + 1, is_recv ? (&ifd) : 0, is_sent ? (&ofd) : 0, 0, &to);
		if (r<0) { analyze_error(r); return false; }
		if ((!r) || (!FD_ISSET(id_, &ifd) && (!FD_ISSET(id_, &ofd)))) return false;
		if (is_recv && FD_ISSET(id_, &ifd)) *is_recv = true;
		if (is_sent && FD_ISSET(id_, &ofd)) *is_sent = true;
#endif
		return true;
	}

	long Socket::send(const std::string & text)
	{
		if (text.empty()) return 0; return send(text.data(), text.length());
	}

	long Socket::send(const void * data, size_t datasize)
	{
		if (!data || !datasize) return 0;
		checksent_ = true;
		const char *p = (const char*)data;
		long l = (long)datasize;
		while (l > 0) {
			long r = ::send(id_, p, (unsigned)l, 0);
			if (r > 0) {
				l -= r;
				p += r;
				continue;
			}
			else if ((r = analyze_error(r)) != EWOULDBLOCK) {
				return r;
			}
			break;
		}
		return (long)datasize - l;
	}

	long Socket::recv(void * data, size_t datasize)
	{
		if (!data || !datasize) {
			close();
			return false;
		}
		long l = ::recv(id_, (char*)data, datasize, MSG_DONTWAIT);
		if (l > 0) return l;
		if (l == 0)
		{
			close();
			return 0;
		}
		analyze_error(l);
		if (error())
		{
			//ctracem("basic_socket::recv() ERROR");
		}
		return 0;
	}

	long Socket::recv(std::string & s)
	{
		unsigned n = 256;
		s.clear();
		char *buffer;
		try {
			buffer = new char[n];
		}
		catch (...) {
			error_text("basic_socket::recv(str_t&) intermediate data memory allocation failed)", true);
			return -1;
		}
		if (buffer) {
			long nr = 0;
			if ((nr = recv(buffer, n)) > 0) {
				s.reserve(s.length() + nr);
				for (long i = 0; i<nr; ++i) s += buffer[i];
			}
			delete[] buffer;
		}
		else {
			error_text("basic_socket::recv(str_t&) failed (no buffer allocated)", true);
		}
		return (long)s.size();
	}

	long Socket::recvfrom(void * data, size_t datasize, NetworkEndPoint & ip)
	{
		typename sockaddr_storage sa;
		typename socklen_t lsa = sizeof(sa);
		long r = ::recvfrom(id_, (char*)data, datasize, 0, (sockaddr*)&sa, &lsa);
		if (r == SOCKET_ERROR && analyze_error(r)) return -1;
		if (sa.ss_family == AF_INET) {
			ip = *((sockaddr_in*)&sa);
		}
		else if (sa.ss_family == AF_INET6) {
			ip = *((sockaddr_in6*)&sa);
		}
		else {
			ip.address(sa);
			ip.size(lsa);
		}
		return r;
	}

	long Socket::sendto(const void * data, size_t datasize, const NetworkEndPoint & ip)
	{
		if (!ip.valid()) {
			error_text("basic_socket::sendto(): IP address is invalid", true);
			return -1;
		}
		typename sockaddr_storage sa = ip.address();
		typename socklen_t lsa = ip.size();
		long r = ::sendto(id_, (const char*)data, datasize, 0, (struct sockaddr*) (&sa), lsa);
		if (r == SOCKET_ERROR && analyze_error(r))
			return -1;
		checksent_ = true;
		return r;
	}

	bool Socket::connect(int timeout)
	{
		Stopwatch clock;
		clock.Start();
		bool ret = false;
		while (clock.GetElapsedMilliseconds() != timeout && (ret = connect()));
		return true;
	}

	long Socket::analyze_error(long fn_return) const
	{
		if (fn_return == SOCKET_ERROR) {
			long e = SOCKET_ERRNO;
			switch (e)
			{
			case EWOULDBLOCK:
				break;
			case EINTR:
				break;
			case ETIMEDOUT:
				break;
			default:
				const_cast<long&>(errno_) = (long)e;
				const_cast<std::string&>(errtext_) = syserr_text(e);
				if (id_ != INVALID_SOCKET) {
#ifdef _WIN32
					::shutdown(id_, SD_BOTH); ::closesocket(id_);
#else
					::close(id_);
#endif
				}
				const_cast<SOCKET&>(id_) = INVALID_SOCKET;
			}
			return e;
		}
		return -1;
	}

	bool Socket::listens()
	{
		return listens_;
	}

	std::string Socket::syserr_text(long err)
	{
#ifdef _WIN32
		char buffer[64];
		if (FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr, (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buffer, 63, nullptr)) {
			buffer[63] = '\0';
			return std::string(buffer);
		}
		return std::string("unknown error");
#else
		return ::strerror(err);
#endif
	}

	bool send_to_finished = false;

	void send_to_threaded(Socket *soc, Bytes bytes, NetworkEndPoint &ip)
	{
		soc->sendto(bytes.GetBytes(), bytes.GetLength(), ip);

		send_to_finished = true;
	}

	template<typename ...Args>
	void Socket::BeginSendTo(Bytes bytes, NetworkEndPoint &ip, std::function<void(Args...)> callback, ...)
	{
		std::thread send_to_thread(send_to_threaded, this, bytes, ip);
		std::thread callback_thread(callback, ...);
	}

	void Socket::EndSendTo()
	{
		while (!send_to_finished);
	}

	volatile bool receive_from_finished = false;
	volatile long receive_from_received = 0;

	void receive_from_threaded(Socket *soc, byte *bytes, int size, NetworkEndPoint &ip)
	{
		receive_from_received = soc->recvfrom(bytes, size, ip);
		receive_from_finished = true;
	}

	template<typename ...Args>
	void Socket::BeginReceiveFrom(byte * bytes, int size, NetworkEndPoint &ip, std::function<void(Args...)> callback, ...)
	{
		std::thread receive_from_thread(receive_from_threaded, this, bytes, size, ip);
		std::thread callback_thread(callback, ...);
	}

	long Socket::EndReceiveFrom()
	{
		while (!receive_from_finished);
		return receive_from_received;
	}
}