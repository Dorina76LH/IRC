#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#include <sys/socket.h>		// socket()
#include <netinet/in.h>		// AF_INET
#include <fcntl.h>			// fcntl(), F_GETFL, F_SETFL, O_NONBLOCK
#include <arpa/inet.h>		// htons()
#include <poll.h>			// poll()

class Server
{
	private:
		int							_fdServer;
		int							_port;
		std::string					_password;
		std::vector<struct pollfd>	_pollfds;

		void setupSocket();
	public:
		Server(int port, const std::string &password);
		~Server();

		void run();
};