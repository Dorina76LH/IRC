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

	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		delete it->second;
	}
	_channels.clear();

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

	for (std::map<std::string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt)
	{
		Channel *channel = chanIt->second;
		if (channel && channel->isMember(fd))
		{
			channel->removeMember(fd);
			channel->removeOperator(fd);
		}
	}

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

void Server::sendData(int fd, size_t index)
{
	Client* client = _clients[fd];
	if (!client)
		return;

	const std::string& buffer = client->getWriteBuffer();
	if (buffer.empty())
		return;

	ssize_t bytesSent = send(fd, buffer.c_str(), buffer.size(), 0);
	if (bytesSent < 0)
	{
		std::cerr << "Error : send() on FD " << fd << std::endl;
		disconnectClient(index);
		return;
	}

	client->clearSentData(bytesSent);
}

void Server::run()
{
	if (_pollfds.empty())
	{
		struct pollfd serverPfd;
		serverPfd.fd = _fdServer;
		serverPfd.events = POLLIN; // = 1 = y'a t'il des données a lire, si oui le dire dans revents
		serverPfd.revents = 0;
		_pollfds.push_back(serverPfd);
	}

	std::cout << "Server running..." << std::endl;
	while (g_running)
	{
		for (size_t i = 1; i < _pollfds.size(); i++)
		{
			Client* client = _clients[_pollfds[i].fd];
			if (client && client->hasDataToSend())
				_pollfds[i].events = POLLIN | POLLOUT;
			else
				_pollfds[i].events = POLLIN;
		}

		int ret = poll(_pollfds.data(), _pollfds.size(), -1); // -1 = attente infinie
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


			// POLLNVAL = 0010 0000 (valeur 32) = Descripteur invalide
			if (revents & (POLLNVAL))
			{
				if (fd == _fdServer)
					throw std::runtime_error("Error : POLLNVAL on server socket");

				disconnectClient(i);
				i--;
				current_size--;
				continue;
			}


			// POLLIN  = 0000 0001 (valeur 1) = Données prete a etre lu
			// Gérer la lecture (POLLIN)
			if (revents & POLLIN)
			{
				if (fd == _fdServer)
					acceptNewClient();
				else
				{
					bool clientDisconnected = receiveData(fd, i);
					if (clientDisconnected)
					{
						i--;
						current_size--;
						continue;
					}

					Client* client = _clients[fd];
					if (client)
					{
						while (client->hasCompleteLine())
						{
							std::string line = client->extractLine();
							processClientMessage(client, line);
							std::cout << "Message received from (FD:" << fd << ") : " << line << std::endl;
							if (_clients.find(fd) == _clients.end())
								break;
						}
					}
					if (_clients.find(fd) == _clients.end())
					{
						for (size_t idx = 0; idx < _pollfds.size(); ++idx)
						{
							if (_pollfds[idx].fd == fd)
							{
								disconnectClient(idx);
								i--;
								current_size--;
								break;
							}
						}
						continue;
					}
				}
			}

	
			// POLLOUT = 0000 0100 (valeur 4) = Socket prêt pour l'écriture
			// POLLERR = 0000 1000 (valeur 8) = Erreur systeme s'est produite
			// POLLHUP = 0001 0000 (valeur 16) = le client a fermé de son coté
			if (revents & (POLLERR | POLLHUP))
			{
				if (fd == _fdServer)
					throw std::runtime_error("Error : POLLERR on server socket");

				disconnectClient(i);
				i--;
				current_size--;
				continue;
			}


			// Gérer l'écriture (POLLOUT)
			if (revents & POLLOUT)
			{
				sendData(fd, i);

				Client* client = _clients[fd];
				if (client && client->getIsToDisconnect() && !client->hasDataToSend())
				{
					disconnectClient(i);
					i--;
					current_size--;
					continue;
				}
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

void Server::finalizeRegistrationIfReady(Client* client)
{
	if (!client || client->isRegistered())
		return;

	if (!client->isAuthenticated() || client->getNickname().empty() || client->getUsername().empty())
		return;

	client->setRegistered(true);
	client->appendToWriteBuffer(Commands::buildReply("001", client->getNickname(),
		"Welcome to the Internet Relay Network " + client->getNickname()));
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

	// La RFC 1459 ne precise pas la casse attendue pour <command> : on tolere
	// n'importe quelle casse en normalisant en majuscules pour le dispatch,
	// sans modifier "command" (qui reste tel que recu, pour l'echo dans les
	// erreurs comme 421). N'affecte pas irssi, qui envoie deja en majuscules.
	std::string upperCommand = command;
	for (size_t i = 0; i < upperCommand.size(); ++i)
		upperCommand[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(upperCommand[i])));

	if (upperCommand == "PASS")
	{
		Commands::handlePass(*client, params, _password);
	}
	else if (upperCommand == "NICK")
	{
		std::vector<std::string> activeNicknames = getAllNicknames();
		Commands::handleNick(*client, params, activeNicknames);
		finalizeRegistrationIfReady(client);
	}
	else if (upperCommand == "USER")
	{
		Commands::handleUser(*client, params);
		finalizeRegistrationIfReady(client);
	}
	else if (upperCommand == "PING")
		Commands::handlePing(*client, params);
	else if (upperCommand == "JOIN")
		Commands::handleJoin(*client, params, _channels);
	else if (upperCommand == "PRIVMSG")
		Commands::handlePrivMsg(*client, params, _channels, _clients);
	else if (upperCommand == "TOPIC")
		Commands::handleTopic(*client, params, _channels);
	else if (upperCommand == "MODE")
		Commands::handleMode(*client, params, _channels);
	else if (upperCommand == "QUIT")
		Commands::handleQuit(*client, params, *this);
	else if (upperCommand == "KICK")
		Commands::handleKick(*client, params, _channels, _clients);
	else if (upperCommand == "INVITE")
		Commands::handleInvite(*client, params, _channels, _clients);
	else
	{
		std::string target = client->getNickname().empty() ? "*" : client->getNickname(); // Nickname si existe, sinon * (RFC 1459)
		client->appendToWriteBuffer(Commands::buildReply("421", target, command, "Unknown command"));
	}
}