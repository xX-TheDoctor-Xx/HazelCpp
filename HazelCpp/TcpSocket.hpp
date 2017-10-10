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
#include "Socket.hpp"

namespace Hazel
{
	class TcpSocket : public Socket
	{
	public:
		TcpSocket();

		TcpSocket(SOCKET id);

		TcpSocket(const NetworkEndPoint & adr);

		TcpSocket(SOCKET id, const NetworkEndPoint & adr);

		TcpSocket(const char* adr);

		virtual ~TcpSocket();

		const NetworkEndPoint & ip() const;

		void ip(const NetworkEndPoint & adr);

		void ip(const char* adr);

		long error() const throw ();

		const std::string & error_text() const;

		void error(long newerror);

		void error_text(const char* text);

		bool connect(const char* iptext);

		bool connect(const NetworkEndPoint & ip_addr);

		bool listen(const NetworkEndPoint & ip_addr, bool reuse_address = true, bool no_delay = true, int max_pending = 1);

		bool accept(TcpSocket & acc);

		void close();

		bool closed() const;
	};
}