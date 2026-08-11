#include "../includes/Server.hpp"

void server::setupSocket()
{
	_fdServer = socket(AF_INET, SOCK_STREAM, 0);
	// - AF_INET = IPV4
	// - SOCK_STREAM = TCP
	// - 0 = default(TCP)
	if (_fdServer == -1)
		throw std::runtime_error("Error : socket()");

	int optval = 1;
	if (setsockopt(_fdServer, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
	{
		// - SOL_SOCKET= niveau de l'option qu'on veux changer
		// - SO_REUSEADDR= autorise le socket a se lier a nouveau rapidement apres un arret du server
		// - pointeur vers la valeur (1 = activer l'option, 0 = desactiver)
		// - sizeof car taille en octet peux varier en fonction de l'option
		throw std::runtime_error("Error : setsockopt()");
	}
}