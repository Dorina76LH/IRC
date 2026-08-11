#include "../includes/Server.hpp"

int fdserver = socket(AF_INET, SOCK_STREAM, 0); // AF_INET=IPV4 - SOCK_STREAM=TCP - 0=default(TCP)
if (fdserver == -1)
	throw std::runtime_error("Error : Socket");