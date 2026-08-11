#pragma once

#include <iostream>
#include <string>

#include <sys/socket.h> // socket()
#include <netinet/in.h> // AF_INET

class server
{
	private:
		int				_fdserver;
		int				_port;
		std::string		_password;
	public:
		
};