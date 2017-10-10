#include "NetworkEndPoint.hpp"

namespace Hazel
{
	NetworkEndPoint::NetworkEndPoint() : size_(0), mask_(0)
	{
		clear();
	}

	NetworkEndPoint::NetworkEndPoint(const NetworkEndPoint & ip) : address_(ip.address_), size_(ip.size_), mask_(ip.mask_)
	{
	}

	NetworkEndPoint::NetworkEndPoint(sockaddr_storage adr, ushort port) : address_(adr), size_(0), mask_(0)
	{
		if (address_.ss_family == AF_INET6) 
			size_ = sizeof(struct sockaddr_in6);
		else if (address_.ss_family == AF_INET) 
			size_ = sizeof(struct sockaddr_in);
		this->port(port);
	}

	NetworkEndPoint::NetworkEndPoint(const char * s)
	{
		str(s);
	}

	NetworkEndPoint::NetworkEndPoint(const std::string& s)
	{
		str(s);
	}

	NetworkEndPoint::NetworkEndPoint(sockaddr_in sa)
	{
		operator =(sa);
	}

	NetworkEndPoint::NetworkEndPoint(sockaddr_in6 sa)
	{
		operator =(sa);
	}

	NetworkEndPoint::~NetworkEndPoint()
	{
	}

	void NetworkEndPoint::clear()
	{
		memset(&address_, 0, sizeof(sockaddr_storage));
		mask_ = 0;
		size_ = 0;
	}

	bool NetworkEndPoint::valid() const throw()
	{
		return address_.ss_family != 0 && size_ != 0;
	}

	bool NetworkEndPoint::is_v4() const throw()
	{
		return address_.ss_family == AF_INET;
	}

	inline bool NetworkEndPoint::is_v6() const throw()
	{
		return address_.ss_family == AF_INET6;
	}

	sockaddr_storage & NetworkEndPoint::address() throw()
	{
		return address_;
	}

	void NetworkEndPoint::address(const sockaddr_storage & adr) throw()
	{
		clear();
		if (adr.ss_family == AF_INET)
			size_ = sizeof(sockaddr_in);
		else if (adr.ss_family == AF_INET6)
			size_ = sizeof(sockaddr_in6);
		address_ = adr;
	}

	ulong NetworkEndPoint::family() const throw()
	{
		return address_.ss_family;
	}

	void NetworkEndPoint::family(ulong family__) throw()
	{
		address_.ss_family = family__;
	}

	socklen_t NetworkEndPoint::size() const throw()
	{
		return size_;
	}

	void NetworkEndPoint::size(socklen_t sz) throw()
	{
		size_ = sz;
	}

	ushort NetworkEndPoint::port() const throw()
	{
		if (address_.ss_family == AF_INET)
			return ntohs(((sockaddr_in*)&address_)->sin_port);
		if (address_.ss_family == AF_INET6)
			return ntohs(((sockaddr_in6*)&address_)->sin6_port);
		return 0;
	}

	bool NetworkEndPoint::port(ushort port__) throw()
	{
		if (address_.ss_family == AF_INET)
		{
			((sockaddr_in*)&address_)->sin_port = htons(port__);
			return true;
		}
		else if (address_.ss_family == AF_INET6)
		{
			((sockaddr_in6*)&address_)->sin6_port = htons(port__);
			return true;
		}
		return false;
	}

	bool NetworkEndPoint::subnet_mask(unsigned char msk) throw()
	{
		if (address_.ss_family == AF_INET && msk > 32 || address_.ss_family == AF_INET6 && msk > 128)
			return false;
		mask_ = msk;
		return true;
	}

	unsigned char NetworkEndPoint::subnet_mask() const throw()
	{
		return mask_;
	}

	std::string NetworkEndPoint::str() const
	{
		char s[INET6_ADDRSTRLEN + 2];
		s[sizeof(s) - 1] = '\0';
		if (address_.ss_family == AF_INET6)
			return !::inet_ntop(AF_INET6, &(((struct sockaddr_in6*)&address_)->sin6_addr), s, sizeof(s)) ? std::string() : s;
		else if (address_.ss_family == AF_INET)
			return !::inet_ntop(AF_INET, &(((struct sockaddr_in*)&address_)->sin_addr), s, sizeof(s)) ? std::string() : s;
		else
			return std::string();
	}

	bool NetworkEndPoint::str(std::string adr)
	{
		clear();
		if (adr.length() < 2)
			return false;
		std::string port, subn;
		std::string::size_type p;
		if ((p = adr.find('[')) != std::string::npos)
		{
			std::string::size_type p1 = adr.find(']');
			if (p != 0 || p1 == std::string::npos || p1 < p)
				return false;
			if (p1 > 0 && (p1 < adr.length() - 1))
				port = adr.substr(p1 + 1);
			adr.resize(p1);
			adr = adr.substr(p + 1);
		}
		else if (adr.find(']') != std::string::npos)
			return false;

		if ((p = adr.find('/')) != std::string::npos)
		{
			if (p >= adr.length() - 1)
				return false;
			subn = adr.substr(p + 1);
			std::string::size_type p1 = subn.find(':');
			if (p1 != std::string::npos)
			{
				if (!port.empty() || (p1 == 0 || p1 >= subn.length() - 1))
					return false;
				port = subn.substr(p1 + 1);
				subn.resize(p1);
			}
			adr.resize(p);
		}
		else if ((p = adr.find(':')) != std::string::npos)
		{
			if (adr.rfind(':') == p)
			{
				if (!port.empty() || (p == 0 || p >= subn.length() - 1))
					return false;
				port = adr.substr(p + 1);
				adr.resize(p);
			}
		}
		if (!port.empty() && port[0] == ':')
			port = port.substr(1);
		if ((!port.empty() && (unsigned)std::count_if(port.begin(), port.end(), ::isdigit) < port.length()) || subn.length() > 3 || (!subn.empty() && (unsigned)std::count_if(subn.begin(), subn.end(), ::isdigit)<subn.length()))
			return false;
		if (adr.find(':') != std::string::npos)
		{
			struct sockaddr_in6 addr;
			memset(&addr, 0, sizeof(struct sockaddr_in6));
			addr.sin6_family = AF_INET6;
			addr.sin6_port = htons(0);
			if (::inet_pton(AF_INET6, adr.c_str(), &(addr.sin6_addr)) != 1)
				return false;
			size_ = sizeof(struct sockaddr_in6);
			address_.ss_family = AF_INET6;
			((struct sockaddr_in6*)&address_)->sin6_addr = addr.sin6_addr;
		}
		else
		{
			struct sockaddr_in addr;
			memset(&addr, 0, sizeof(struct sockaddr_in));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(0);
			if (::inet_pton(AF_INET, adr.c_str(), &(addr.sin_addr)) != 1)
				return false;
			size_ = sizeof(struct sockaddr_in);
			address_.ss_family = AF_INET;
			((struct sockaddr_in*)&address_)->sin_addr = addr.sin_addr;
		}
		if (!subn.empty() && !subnet_mask((unsigned char)(::atol(subn.c_str()) & 0xff)))
			return false;
		if (!port.empty())
			NetworkEndPoint::port((ushort)(::atol(port.c_str())));
		return true;
	}

	void NetworkEndPoint::operator=(const NetworkEndPoint & ip) throw()
	{
		address_ = ip.address_; size_ = ip.size_; mask_ = ip.mask_;
	}

	void NetworkEndPoint::operator=(const sockaddr_in & adr) throw()
	{
		*((sockaddr_in*)&address_) = adr; address_.ss_family = AF_INET; size_ = sizeof(struct sockaddr_in);
	}

	void NetworkEndPoint::operator=(const sockaddr_in6 & adr) throw()
	{
		*((sockaddr_in6*)&address_) = adr; address_.ss_family = AF_INET6; size_ = sizeof(struct sockaddr_in6);
	}

	void NetworkEndPoint::operator=(const std::string & ip)
	{
		str(ip);
	}

	void NetworkEndPoint::operator=(const char * ip)
	{
		str(ip);
	}

	bool NetworkEndPoint::operator==(NetworkEndPoint & ip)
	{
		sockaddr_storage &addr = address();

		return addr.ss_family == ip.address().ss_family &&
			addr.__ss_pad1 == ip.address().__ss_pad1 &&
			addr.__ss_pad2 == ip.address().__ss_pad2 &&
			addr.__ss_align == ip.address().__ss_align &&
			size_ == ip.size() &&
			mask_ == subnet_mask();
	}

	NetworkEndPoint::operator const struct sockaddr_in&() const throw()
	{
		return *((sockaddr_in*)&address_);
	}

	NetworkEndPoint::operator const struct sockaddr_in6&() const throw()
	{
		return *((sockaddr_in6*)&address_);
	}

	bool NetworkEndPoint::parse_resource_address(std::string res_addr, NetworkEndPoint & ip, std::string & service, std::string & user, std::string & host, std::string & path, bool lookup_host, AddressFamily family, SocketType type)
	{
		for (std::string::const_iterator it = res_addr.begin(); it != res_addr.end(); ++it)
		{
			if (!isprint(*it))
				return false;
		}

		std::string::size_type pss, ps;
		if (res_addr.empty())
			return false;
		pss = res_addr.find("://"); ps = res_addr.find('/');
		if (pss != 0 && pss != std::string::npos && ps > pss)
		{
			service = res_addr.substr(0, pss);
			res_addr = res_addr.substr(pss + 3);
		}

		if ((ps = res_addr.find('/')) != std::string::npos)
		{
			if (ps == 0) return false;
			path = res_addr.substr(ps);
			res_addr.resize(res_addr.length() - path.length());
		}

		if ((pss = res_addr.find('@')) != std::string::npos)
		{
			if (pss == 0) return false;
			user = res_addr.substr(0, pss);
			res_addr = res_addr.substr(pss + 1);
		}

		std::string port;
		if (res_addr.find('[') != std::string::npos)
		{
			if ((pss = res_addr.find("]:")) != std::string::npos)
			{
				if (pss >= res_addr.length() - 2)
					return false;
				port = res_addr.substr(pss + 2);
				res_addr.resize(pss + 1);
				if (std::count_if(port.begin(), port.end(), ::isdigit) < (int)port.length())
					return false;
			}
		}
		else
		{
			if ((pss = res_addr.find(":")) != std::string::npos)
			{
				if (pss >= res_addr.length() - 1)
					return false;
				port = res_addr.substr(pss + 1);
				res_addr.resize(pss);
				if (std::count_if(port.begin(), port.end(), ::isdigit) < (int)port.length())
					return false;
			}
		}

		host = res_addr;
		ip.str(res_addr);
		if (!ip.valid() && lookup_host)
		{
			hostlookup_list_t lst = host_lookup(res_addr, service, family, type);
			for (typename hostlookup_list_t::const_iterator it = lst.begin(); it != lst.end(); ++it)
			{
				if (it->family == AddressFamily::IPv4)
				{
					ip = *((struct sockaddr_in*)&it->adr);
					break;
				}
				else if (it->family == AddressFamily::IPv6)
				{
					ip = *((struct sockaddr_in6*)&it->adr);
					break;
				}
				else
					continue;
			}
		}

		if (port.length() > 0)
			ip.port((ushort)(::atol(port.c_str())));
		if (ip.port() == 0 && !service.empty())
			ip.port(port_by_service(service));
		return false;
	}

	NetworkEndPoint::hostlookup_list_t NetworkEndPoint::host_lookup(std::string host, std::string service, AddressFamily family, SocketType type)
	{
		hostlookup_list_t lst;
		if (host.empty())
			return lst;
		struct addrinfo *ai = 0, *aii = 0, hints;
		char str_addr[INET6_ADDRSTRLEN + 1];
		memset(str_addr, 0, sizeof(str_addr));
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = (int)family;
		hints.ai_socktype = (int)type;
		hints.ai_flags = AI_CANONNAME;
		if (::getaddrinfo(host.c_str(), (service.empty() ? nullptr : service.c_str()), &hints, &aii) || !aii)
			return lst;
		unsigned n_entries = 0;
		for (ai = aii; ai; ai = ai->ai_next) ++n_entries;
		if (n_entries > 0)
		{
			try
			{
				lst.reserve(n_entries);
				for (ai = aii; ai; ai = ai->ai_next)
				{
					str_addr[0] = 0;
					struct hostlookup_data_t data;
					if (!ai->ai_addrlen || ai->ai_addrlen > sizeof(sockaddr_storage))
						continue;
					if (ai->ai_family == AF_INET6)
					{
						if (ai->ai_addr->sa_family != AF_INET6 || ai->ai_addrlen != sizeof(struct sockaddr_in6) || !::inet_ntop(AF_INET6, &((struct sockaddr_in6*)ai->ai_addr)->sin6_addr, str_addr, ai->ai_addrlen))
							continue;
					}
					else if (ai->ai_family == AF_INET)
					{
						if (ai->ai_addr->sa_family != AF_INET || ai->ai_addrlen != sizeof(struct sockaddr_in) || !::inet_ntop(AF_INET, &((struct sockaddr_in*)ai->ai_addr)->sin_addr, str_addr, ai->ai_addrlen))
							continue;
					}
					else
						continue;

					data.saddr = str_addr;
					data.family = (AddressFamily)ai->ai_family;
					data.size = ai->ai_addrlen;
					data.shost = ai->ai_canonname ? ai->ai_canonname : "";
					memcpy(&data.adr, ai->ai_addr, ai->ai_addrlen);
					lst.push_back(data);
				}
			}
			catch (const std::exception & e)
			{
				if (aii)
					freeaddrinfo(aii);
				throw e;
			}
			catch (...)
			{
				lst.clear();
			}
		}

		if (aii)
			freeaddrinfo(aii);
		return lst;
	}

	std::string NetworkEndPoint::service_by_port(ushort port__) throw()
	{
		struct servent* sa = ::getservbyport(htons(port__), nullptr);
		return (!sa || !sa->s_name) ? std::string() : std::string(sa->s_name);
	}

	ushort NetworkEndPoint::port_by_service(const std::string & service__) throw()
	{
		struct servent* sa = ::getservbyname(service__.c_str(), nullptr);
		return (!sa || !sa->s_name) ? 0 : ntohs(sa->s_port);
	}

	std::string NetworkEndPoint::my_hostname()
	{
		char s[128]; s[sizeof(s) - 1] = '\0';
		return (::gethostname(s, sizeof(s) - 1) < 0) ? std::string() : std::string(s);
	}
}