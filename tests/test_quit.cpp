#include "../includes/Commands.hpp"
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <map>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -Wall -Wextra -Werror -std=c++98 tests/test_quit.cpp tests/globals.cpp srcs/Server.cpp srcs/Client.cpp srcs/Parser.cpp srcs/Channel.cpp srcs/Bot.cpp srcs/commands/*.cpp -o test_quit

int main()
{
	// 0. QUIT sans parametre utilise la raison par defaut et marque le client a deconnecter
	{
		Server server(6699, "mypass");
		Client client(0);
		client.setNickname("ada");
		client.setUsername("ada_u");

		std::vector<std::string> params;
		Commands::handleQuit(client, params, server);
		assert(client.getIsToDisconnect() == true);
		assert(client.getWriteBuffer().find("ERROR :Closing Link: localhost (Client quit)") != std::string::npos);
	}
	std::cout << "0. QUIT without parameters sets default reason and disconnect flag." << std::endl;

	// 1. QUIT avec un message personnalise utilise la raison specifiee
	{
		Server server(6699, "mypass");
		Client client(1);
		client.setNickname("bob");
		client.setUsername("bob_u");

		std::vector<std::string> params;
		params.push_back("Leaving");
		params.push_back("the");
		params.push_back("server");

		Commands::handleQuit(client, params, server);
		assert(client.getIsToDisconnect() == true);
		assert(client.getWriteBuffer().find("ERROR :Closing Link: localhost (Leaving the server)") != std::string::npos);
	}
	std::cout << "1. QUIT with custom message uses specified reason." << std::endl;

	std::cout << COLOR_GREEN << "\nAll QUIT command tests passed!" << COLOR_RESET << std::endl;

	return 0;
}