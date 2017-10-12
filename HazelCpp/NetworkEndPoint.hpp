/**
* @package de.atwillys.cc.swl
* @license BSD (simplified)
* @author Stefan Wilhelm (stfwi)
*
* @socket.hh
* @ccflags
* @ldflags -lpthread
* @platform linux, bsd, windows
* @standard >= c++98
*
* -----------------------------------------------------------------------------
*
* Template based socket handling, contains classes that you can extend to implement:
*
*  - TCP sockets:
*      - class sw::sock::tcp_client    : Client class, connects to servers
*      - class sw::sock::tcp_acceptor  : Server class dealing with the requests
*      - template class sw::sock::tcp_server<acceptor_type>: Listening server
*
*  - UDP sockets:
*      - class sw::sock::udp_socket    : UDP socket class
*
* All you actually have to to is to overload callback methods. The rest of the socket
* handling is pretty much done in the base classes.
*
* REQUIRES header file "sw/thread.hh"
*
* -----------------------------------------------------------------------------
* +++ BSD license header +++
* Copyright (c) 2008-2014, Stefan Wilhelm (stfwi, <cerbero s@atwilly s.de>)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met: (1) Redistributions
* of source code must retain the above copyright notice, this list of conditions
* and the following disclaimer. (2) Redistributions in binary form must reproduce
* the above copyright notice, this list of conditions and the following disclaimer
* in the documentation and/or other materials provided with the distribution.
* (3) Neither the name of atwillys.de nor the names of its contributors may be
* used to endorse or promote products derived from this software without specific
* prior written permission. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
* AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
* BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
* WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
* -----------------------------------------------------------------------------
*/

#pragma once

#include "Hazel.hpp"

#include <string>
#include <algorithm>
#include <vector>

namespace Hazel
{
	class NetworkEndPoint
	{
		struct hostlookup_data_t {
			std::string saddr, shost; AddressFamily family; socklen_t size; sockaddr_storage adr;
		};

		typedef std::vector<hostlookup_data_t> hostlookup_list_t;

	public:

		NetworkEndPoint();
		NetworkEndPoint(const NetworkEndPoint & ip);
		NetworkEndPoint(sockaddr_storage adr, ushort port);
		NetworkEndPoint(const char *s);
		NetworkEndPoint(const std::string & s);
		NetworkEndPoint(struct sockaddr_in sa);
		NetworkEndPoint(struct sockaddr_in6 sa);

		virtual ~NetworkEndPoint();

	public:
		void clear();
		bool valid() const throw ();
		bool is_v4() const throw();
		bool is_v6() const throw();

		sockaddr_storage & address() throw ();
		void address(const sockaddr_storage & adr) throw ();

		ulong family() const throw ();
		void family(ulong family__) throw ();

		socklen_t size() const throw();
		void size(socklen_t sz) throw();

		ushort port() const throw ();
		bool port(ushort port__) throw ();

		bool subnet_mask(unsigned char msk) throw();
		unsigned char subnet_mask() const throw();

		std::string str() const;
		bool str(std::string adr);

	public:
		void operator=(const NetworkEndPoint & ip) throw ();
		void operator=(const sockaddr_in & adr) throw ();
		void operator=(const sockaddr_in6 & adr) throw ();
		void operator=(const std::string & ip);
		void operator=(const char* ip);

		bool operator==(NetworkEndPoint &ip);

		operator const struct sockaddr_in & () const throw ();
		operator const struct sockaddr_in6 & () const throw ();

	public:
		static bool parse_resource_address(std::string res_addr, NetworkEndPoint & ip, std::string & service, std::string & user, std::string & host, std::string & path, bool lookup_host = false, AddressFamily family = AddressFamily::Any, SocketType type = SocketType::Tcp);
		static hostlookup_list_t host_lookup(std::string host, std::string service = std::string(), AddressFamily family = AddressFamily::Any, SocketType type = SocketType::Any);
		static std::string service_by_port(ushort port__) throw ();
		static ushort port_by_service(const std::string & service__) throw ();
		static std::string my_hostname();

	private:
		sockaddr_storage address_;
		socklen_t size_;
		unsigned char mask_;
	};
}