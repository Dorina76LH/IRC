#include "../includes/Server.hpp"

void server::setupSocket()
{
	_fdServer = socket(AF_INET, SOCK_STREAM, 0); // AF_INET=IPV4 - SOCK_STREAM=TCP - 0=default(TCP)
	if (_fdServer == -1)
		throw std::runtime_error("Error : Socket");

}