#include "../includes/Server.hpp"

Server::Server(int port, const std::string& password)
: _fdServer(-1), _port(port), _password(password)
{
	setupSocket();
}

Server::~Server()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}

	_clients.clear();

	if (_fdServer != -1)
		close(_fdServer);
}

void Server::setupSocket()
{
	_fdServer = socket(AF_INET, SOCK_STREAM, 0);
	// - AF_INET = IPV4
	// - SOCK_STREAM = TCP
	// - 0 = default (ici TCP)
	if (_fdServer == -1)
		throw std::runtime_error("Error : socket()");

	int optval = 1;
	if (setsockopt(_fdServer, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		// - SOL_SOCKET= niveau de l'option qu'on veux changer
		// - SO_REUSEADDR= autorise le socket a se lier a nouveau rapidement apres un arret du server
		// - pointeur vers la valeur (1 = activer l'option, 0 = desactiver)
		// - sizeof car taille en octet peux varier en fonction de l'option
		throw std::runtime_error("Error : setsockopt()");
	}

	if (fcntl(_fdServer, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error : fcntl()");

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; // IPV4
	addr.sin_addr.s_addr = INADDR_ANY; // ip sur laquelle ecouter, ici INADDR_ANY = toute les ip de la machine
	addr.sin_port = htons(_port); // convertit au format standard le port

	if (bind(_fdServer, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("Error : bind()");

	if (listen(_fdServer, SOMAXCONN) == -1) // SOMAXCONN = valeur max possible de ce systeme
		throw std::runtime_error("Error : listen()");
}

void Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	// accepter la connexion
	int clientFd = accept(_fdServer, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientFd == -1)
	{
		std::cerr << "Error : accept() client" << std::endl;
		return;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error : fcntl() client" << std::endl;
		close(clientFd);
		return;
	}

	_clients[clientFd] = new Client(clientFd);

	struct pollfd clientPfd;
	clientPfd.fd = clientFd;
	clientPfd.events = POLLIN;
	clientPfd.revents = 0;

	_pollfds.push_back(clientPfd);

	std::cout << "New client : (FD:" << clientFd << ")" << std::endl;
}

void Server::disconnectClient(size_t index)
{
	if (index >= _pollfds.size())
		return;

	int fd = _pollfds[index].fd;

	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
	{
		delete it->second;
		_clients.erase(it);
	}

	close(fd);

	_pollfds.erase(_pollfds.begin() + index);

	std::cout << "Client (FD:" << fd << ") disconnected." << std::endl;
}

bool Server::receiveData(int fd, size_t index)
{
	char buffer[1024];

	std::memset(buffer, 0, sizeof(buffer));

	ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead <= 0)
	{
		if (bytesRead < 0)
			std::cerr << "Error : recv() " << fd << std::endl;

		disconnectClient(index);
		return true;
	}

	Client *client = _clients[fd];

	client->appendToReadBuffer(std::string(buffer, bytesRead));

	return false;
}

void Server::run()
{
	if (_pollfds.empty())
	{
		struct pollfd serverPfd;
		serverPfd.fd = _fdServer;
		serverPfd.events = POLLIN; // = 1 = y'a t'il des donnée a lire, si oui le dire dans revents
		serverPfd.revents = 0;
		_pollfds.push_back(serverPfd);
	}

	std::cout << "Server running..." << std::endl;
	while (g_running)
	{
		int ret = poll(_pollfds.data(), _pollfds.size(), -1); // quels fds ecouter, -1 = attend indefiniment
		if (ret == -1)
		{
			if (!g_running)
			{
				std::cout << "\nSignal detected." << std::endl;
				return;
			}
			// CTRL+C / CTRL+'\'
			throw std::runtime_error("Error : poll()");
		}

		size_t current_size = _pollfds.size();

		for (size_t i = 0; i < current_size; i++)
		{
			short revents = _pollfds[i].revents;
			int fd = _pollfds[i].fd;

			if (revents == 0)
				continue;

			bool clientDisconnected = false;

			if (revents & POLLIN) // nouvelle connexion / données entrantes
			{
				if (fd == _fdServer)
					acceptNewClient();
				else
				{
					clientDisconnected = receiveData(fd, i);
					if (!clientDisconnected)
					{
						Client* client = _clients[fd];
						if (client)
						{
							while (client->hasCompleteLine())
							{
								std::string line = client->extractLine();
								processClientMessage(client, line);
								std::cout << "Message received from (FD:" << fd << ") : " << line << std::endl;
							}
						}
					}
				}
			}

			// POLLIN  = 0000 0001 (valeur 1) = Données prete a etre lu
			// POLLERR = 0000 1000 (valeur 8) = Erreur systeme s'est produite
			// POLLHUP = 0001 0000 (valeur 16) = le client a fermé de son coté
			// POLLNVAL = 0010 0000 (valeur 32) = Descripteur invalide
			if (!clientDisconnected && (revents & (POLLERR | POLLHUP | POLLNVAL))) // (| = OU binaire) (& = ET binaire)
			{
				if (fd == _fdServer)
					throw std::runtime_error("Error : POLLERR");
				else
				{
					disconnectClient(i);
					clientDisconnected = true;
				}
			}
			if (clientDisconnected)
			{
				i--;
				current_size--;
			}
		}
	}
}

Client* Server::getClientByNickname(const std::string& nickname)
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second && it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

std::vector<std::string> Server::getAllNicknames() const
{
	std::vector<std::string> nicknames;
	for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second && !it->second->getNickname().empty())
			nicknames.push_back(it->second->getNickname());
	}
	return nicknames;
}

void Server::processClientMessage(Client* client, const std::string& line)
{
	if (!client || line.empty())
		return;

	std::string command;
	std::vector<std::string> params;

	Parser::parseLine(line, command, params);

	if (command.empty())
		return;

	if (command == "PASS")
	{
		std::cout << "PASS command detected" << std::endl;
		Commands::handlePass(*client, params, _password);
	}
	else if (command == "NICK")
	{
		std::cout << "NICK command detected" << std::endl;
		std::vector<std::string> activeNicknames = getAllNicknames();
		Commands::handleNick(*client, params, activeNicknames);
	}
	else if (command == "USER")
	{
		std::cout << "USER command detected" << std::endl;
		Commands::handleUser(*client, params);
	}
	// else if (command == "JOIN")
	// 	Commands::handleJoin(*client, params);
	// else if (command == "PRIVMSG")
	// 	Commands::handlePrivmsg(*client, params);
	// else if (command == "KICK")
	// 	Commands::handleKick(*client, params);
	// else if (command == "INVITE")
	// 	Commands::handleInvite(*client, params);
	// else if (command == "TOPIC")
	// 	Commands::handleTopic(*client, params);
	// else if (command == "MODE")
	// 	Commands::handleMode(*client, params);
}