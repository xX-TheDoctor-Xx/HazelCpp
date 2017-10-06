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
#include "HazelException.hpp"
#include "Bytes.hpp"
#include "NetworkEndPoint.hpp"

namespace Hazel
{
	class Socket
	{
	public:
		Socket();
		Socket(SOCKET id);
		Socket(const NetworkEndPoint & adr);
		Socket(SOCKET id, const NetworkEndPoint & adr);
		Socket(const char* adr);

		virtual ~Socket();

		bool closed() const;

		SOCKET socket_id() const;

		const NetworkEndPoint & ip() const;
		void ip(const NetworkEndPoint & adr);
		void ip(const char* adr);

		long error() const;
		void error(long newerror);
		const std::string & error_text() const;
		void error_text(const char* text, bool overwrite = false);

		unsigned timeout_ms() const;
		void timeout_ms(unsigned newtimeout);

		int option(int level, int optname);
		void option(int level, int optname, int newval);
		int option_sendbuffer_size();
		int option_recvbuffer_size();
		bool option_error();

		void nodelay(bool enabled);
		bool nodelay();

		void keepalive(bool enabled);
		bool keepalive();

		void reuseaddress(bool enabled);
		bool reuseaddress();

		void nonblocking(bool enabled);
		bool nonblocking();

		bool create(SocketType type = SocketType::Tcp, AddressFamily family = AddressFamily::Any);

		bool connect();

		bool bind();
		bool listen(int max_listen_pending = 1);

		bool accept(Socket & acc);

		void close();

		bool wait(bool *is_recv = nullptr, bool *is_sent = nullptr, int timeout_ms = 100);

		long send(const std::string & text);
		long send(const void* data, size_t datasize);

		long recv(void* data, size_t datasize);
		long recv(std::string & s);
		long recvfrom(void* data, size_t datasize, NetworkEndPoint & ip);
		long sendto(const void* data, size_t datasize, NetworkEndPoint & ip);

		bool connect(int timeout);

	protected:
		long analyze_error(long fn_return) const;

		bool listens();

		static std::string syserr_text(long err);

	private:
		SOCKET id_;
		NetworkEndPoint ip_;
		long errno_;
		std::string errtext_;
		unsigned long timeout_;
		bool checksent_;
		bool listens_;
		int max_listen_pending_;
	};
}