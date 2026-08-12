#pragma once

#include <iostream>
#include <string>

#include <sys/socket.h> // socket()
#include <netinet/in.h> // AF_INET
#include <fcntl.h> // fcntl(), F_GETFL, F_SETFL, O_NONBLOCK
#include <arpa/inet.h> // htons()

class Server
{
	private:
		int				_fdServer;
		int				_port;
		std::string		_password;

		void setupSocket();
	public:
		Server(int port, const std::string &password);
		~Server();

		void run();
};