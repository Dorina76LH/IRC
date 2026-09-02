#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "Commands.hpp"
#include "Parser.hpp"
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <map>
#include <set>
#include <sys/socket.h>		// socket()
#include <netinet/in.h>		// AF_INET
#include <fcntl.h>			// fcntl(), F_GETFL, F_SETFL, O_NONBLOCK
#include <arpa/inet.h>		// htons()
#include <poll.h>			// poll()
#include <csignal>
#include <cerrno>

extern volatile bool g_running;

class Server
{
	private:
		int								_fdServer;
		int								_port;
		std::string						_password;
		std::vector<struct pollfd>		_pollfds;
		std::map<int, Client*>			_clients;
		std::map<std::string, Channel*>	_channels;

		void setupSocket();
		void acceptNewClient();
		bool receiveData(int fd, size_t index);
		void sendData(int fd, size_t index);

	public:
		Server(int port, const std::string &password);
		~Server();
		void processClientMessage(Client* client, const std::string& line);
		void disconnectClient(size_t index);
		void broadcastToSharedChannels(int clientFd, const std::string &message);
		Client* getClientByNickname(const std::string &nickname);
		std::vector<std::string> getAllNicknames() const;
		void finalizeRegistrationIfReady(Client* client);
	
		void run();
};