#include "../includes/Server.hpp"

Server::Server(int port, const std::string& password)
: _fdServer(-1), _port(port), _password(password)
{
    setupSocket();
}

Server::~Server()
{
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

	int flags = fcntl(_fdServer, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Error : fcntl() F_GETFL");

	if (fcntl(_fdServer, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("Error : fcntl() F_SETFL");

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