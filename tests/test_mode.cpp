#include "../includes/Commands.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -std=c++98 -Wall -Wextra -Werror tests/test_mode.cpp srcs/Client.cpp srcs/Channel.cpp srcs/commands/Join.cpp srcs/commands/Mode.cpp -o test_mode

static std::vector<std::string> makeParams(const std::string &p0)
{
	std::vector<std::string> params;
	params.push_back(p0);
	return (params);
}

static std::vector<std::string> makeParams(const std::string &p0, const std::string &p1)
{
	std::vector<std::string> params;
	params.push_back(p0);
	params.push_back(p1);
	return (params);
}

static std::vector<std::string> makeParams(const std::string &p0, const std::string &p1, const std::string &p2)
{
	std::vector<std::string> params;
	params.push_back(p0);
	params.push_back(p1);
	params.push_back(p2);
	return (params);
}

static void clearChannels(std::map<std::string, Channel *> &channels)
{
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
	channels.clear();
}

int main()
{
	// 0. MODE avant que le client ne soit enregistre : 451
	{
		std::map<std::string, Channel *> channels;
		Client client(0);

		Commands::handleMode(client, makeParams("#general", "+t"), channels);
		assert(client.getWriteBuffer().find(" 451 ") != std::string::npos);
		clearChannels(channels);
	}
	std::cout << "0. MODE before registration rejected with 451." << std::endl;

	// 1. MODE sans parametre : 461
	{
		std::map<std::string, Channel *> channels;
		Client client(1);
		client.setNickname("ada");
		client.setRegistered(true);

		Commands::handleMode(client, std::vector<std::string>(), channels);
		assert(client.getWriteBuffer().find(" 461 ") != std::string::npos);
		clearChannels(channels);
	}
	std::cout << "1. MODE without parameters rejected with 461." << std::endl;

	// 2. MODE sur un channel inexistant : 403
	{
		std::map<std::string, Channel *> channels;
		Client client(2);
		client.setNickname("ada");
		client.setRegistered(true);

		Commands::handleMode(client, makeParams("#ghost", "+t"), channels);
		assert(client.getWriteBuffer().find(" 403 ") != std::string::npos);
		clearChannels(channels);
	}
	std::cout << "2. MODE on a non-existent channel rejected with 403." << std::endl;

	// 3. MODE sans mode fourni (requete des modes actuels) retourne les modes du channel
	{
		std::map<std::string, Channel *> channels;
		Client client(3);
		client.setNickname("ada");
		client.setRegistered(true);
		Commands::handleJoin(client, makeParams("#general"), channels);

		Commands::handleMode(client, makeParams("#general"), channels);
		assert(client.getWriteBuffer().find(" 324 ") != std::string::npos);
		clearChannels(channels);
	}
	std::cout << "3. MODE query returns channel modes with 324." << std::endl;

	// 4. MODE par un non-membre : 442
	{
		std::map<std::string, Channel *> channels;
		Client clientA(4);
		clientA.setNickname("ada");
		clientA.setRegistered(true);
		Commands::handleJoin(clientA, makeParams("#general"), channels);

		Client clientB(5);
		clientB.setNickname("bob");
		clientB.setRegistered(true);
		Commands::handleMode(clientB, makeParams("#general", "+t"), channels);
		assert(clientB.getWriteBuffer().find(" 442 ") != std::string::npos);
		clearChannels(channels);
	}
	std::cout << "4. MODE from a non-member rejected with 442." << std::endl;

	// 5. MODE sans privileges operateur : 482
	{
		std::map<std::string, Channel *> channels;
		Client clientA(6);
		clientA.setNickname("ada");
		clientA.setRegistered(true);
		Commands::handleJoin(clientA, makeParams("#general"), channels);

		Client clientB(7);
		clientB.setNickname("bob");
		clientB.setRegistered(true);
		Commands::handleJoin(clientB, makeParams("#general"), channels);

		Commands::handleMode(clientB, makeParams("#general", "+t"), channels);
		assert(clientB.getWriteBuffer().find(" 482 ") != std::string::npos);
		clearChannels(channels);
	}
	std::cout << "5. MODE modification without operator privileges rejected with 482." << std::endl;

	// 6. MODE +t (restreindre le topic aux operateurs) active correctement le mode
	{
		std::map<std::string, Channel *> channels;
		Client client(8);
		client.setNickname("ada");
		client.setRegistered(true);
		Commands::handleJoin(client, makeParams("#general"), channels);

		Commands::handleMode(client, makeParams("#general", "+t"), channels);
		assert(channels["#general"]->isTopicRestricted());
		clearChannels(channels);
	}
	std::cout << "6. MODE +t successfully sets topic restriction." << std::endl;

	// 7. MODE +i (invite-only) active correctement le mode
	{
		std::map<std::string, Channel *> channels;
		Client client(9);
		client.setNickname("ada");
		client.setRegistered(true);
		Commands::handleJoin(client, makeParams("#general"), channels);

		Commands::handleMode(client, makeParams("#general", "+i"), channels);
		assert(channels["#general"]->isInviteOnly());
		clearChannels(channels);
	}
	std::cout << "7. MODE +i successfully sets invite-only mode." << std::endl;

	// 8. MODE +k (cle du channel) defini correctement la cle
	{
		std::map<std::string, Channel *> channels;
		Client client(10);
		client.setNickname("ada");
		client.setRegistered(true);
		Commands::handleJoin(client, makeParams("#general"), channels);

		Commands::handleMode(client, makeParams("#general", "+k", "secretkey"), channels);
		assert(channels["#general"]->hasKey());
		assert(channels["#general"]->getKey() == "secretkey");
		clearChannels(channels);
	}
	std::cout << "8. MODE +k successfully sets channel key." << std::endl;

	// 9. MODE +o (donner le statut operateur) fonctionne correctement
	{
		std::map<std::string, Channel *> channels;
		Client clientA(11);
		clientA.setNickname("ada");
		clientA.setRegistered(true);
		Commands::handleJoin(clientA, makeParams("#general"), channels);

		Client clientB(12);
		clientB.setNickname("bob");
		clientB.setRegistered(true);
		Commands::handleJoin(clientB, makeParams("#general"), channels);

		Commands::handleMode(clientA, makeParams("#general", "+o", "bob"), channels);
		assert(channels["#general"]->isOperator(12));
		clearChannels(channels);
	}
	std::cout << "9. MODE +o successfully grants operator status." << std::endl;

	// 10. MODE +l (limite d'utilisateurs) definit correctement la limite
	{
		std::map<std::string, Channel *> channels;
		Client client(13);
		client.setNickname("ada");
		client.setRegistered(true);
		Commands::handleJoin(client, makeParams("#general"), channels);

		Commands::handleMode(client, makeParams("#general", "+l", "5"), channels);
		assert(channels["#general"]->hasUserLimit());
		assert(channels["#general"]->getUserLimit() == 5);
		clearChannels(channels);
	}
	std::cout << "10. MODE +l successfully sets user limit." << std::endl;

	std::cout << COLOR_GREEN << "\nAll MODE command tests passed!" << COLOR_RESET << std::endl;

	return 0;
}
