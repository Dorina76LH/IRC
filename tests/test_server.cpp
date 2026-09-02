#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -Wall -Wextra -Werror -std=c++98 tests/test_server.cpp tests/globals.cpp srcs/Server.cpp srcs/Client.cpp srcs/Parser.cpp srcs/Channel.cpp srcs/Bot.cpp srcs/commands/*.cpp -o test_server

int main()
{
	// 0. Constructeur et setupSocket
	{
		Server server(6699, "mypass");
	}
	std::cout << "0. Server constructor and setupSocket initialized successfully." << std::endl;

	// 1. Test des méthodes de récupération de pseudos sur un serveur vide
	{
		Server server(6699, "mypass");
		std::vector<std::string> nicknames = server.getAllNicknames();
		assert(nicknames.empty() == true);

		Client* nonExistent = server.getClientByNickname("TestNick");
		assert(nonExistent == NULL);
	}
	std::cout << "1. Empty server nickname lookups handled correctly." << std::endl;

	// 2. Test de la logique de finalisation d'enregistrement (finalizeRegistrationIfReady)
	{
		Server server(6699, "mypass");
		Client client(42);
		
		server.finalizeRegistrationIfReady(&client);
		assert(client.isRegistered() == false);

		client.setAuthenticated(true);
		server.finalizeRegistrationIfReady(&client);
		assert(client.isRegistered() == false);

		client.setNickname("Alice");
		client.setUsername("alice_u");
		server.finalizeRegistrationIfReady(&client);
		assert(client.isRegistered() == true);
		assert(client.getWriteBuffer().find("001") != std::string::npos);
	}
	std::cout << "2. Registration flow and finalizeRegistrationIfReady logic work properly." << std::endl;

	// 3. Test du traitement des messages inconnus (processClientMessage)
	{
		Server server(6699, "mypass");
		Client client(43);
		client.setAuthenticated(true);
		client.setNickname("Bob");
		client.setRegistered(true);

		server.processClientMessage(&client, "UNKNOWN_CMD param1\r\n");
		assert(client.getWriteBuffer().find("421") != std::string::npos);
	}
	std::cout << "3. Unknown message processing generates 421 error." << std::endl;

	std::cout << COLOR_GREEN << "\nAll Server command tests passed!" << COLOR_RESET << std::endl;
	return 0;
}