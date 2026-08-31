#include "../includes/Commands.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -std=c++98 -Wall -Wextra -Werror tests/test_part.cpp srcs/Client.cpp srcs/Channel.cpp srcs/commands/Join.cpp srcs/commands/Part.cpp -o test_part

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

static void clearChannels(std::map<std::string, Channel *> &channels)
{
    for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
        delete it->second;
    channels.clear();
}

int main()
{
    // Security : PART avant que le client ne soit enregistre (451)
    {
        std::map<std::string, Channel *> channels;
        Client client(0);

        Commands::handlePart(client, makeParams("#general"), channels);
        assert(client.getWriteBuffer().find(" 451 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "0. PART before registration rejected with 451." << std::endl;

    // 1. PART sans parametre : 461
    {
        std::map<std::string, Channel *> channels;
        Client client(1);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handlePart(client, std::vector<std::string>(), channels);
        assert(client.getWriteBuffer().find(" 461 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "1. PART without parameters rejected with 461." << std::endl;

    // 2. PART sur un channel inexistant : 403
    {
        std::map<std::string, Channel *> channels;
        Client client(2);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handlePart(client, makeParams("#ghost"), channels);
        assert(client.getWriteBuffer().find(" 403 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "2. PART on a non-existent channel rejected with 403." << std::endl;

    // 3. PART alors qu'on n'est pas membre : 442
    {
        std::map<std::string, Channel *> channels;
        Client clientA(3);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(4);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handlePart(clientB, makeParams("#general"), channels);
        assert(clientB.getWriteBuffer().find(" 442 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "3. PART from a non-member rejected with 442." << std::endl;

    // 4. PART retire bien le client de la liste des membres
    {
        std::map<std::string, Channel *> channels;
        Client clientA(5);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(6);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        Commands::handlePart(clientB, makeParams("#general"), channels);
        assert(!channels["#general"]->isMember(6));
        assert(channels["#general"]->getMemberCount() == 1);
        clearChannels(channels);
    }
    std::cout << "4. PART removes the client from the channel's member list." << std::endl;

    // 5. Le dernier membre qui part detruit le channel
    {
        std::map<std::string, Channel *> channels;
        Client client(7);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#general"), channels);

        Commands::handlePart(client, makeParams("#general"), channels);
        assert(channels.find("#general") == channels.end());
    }
    std::cout << "5. The last member leaving destroys the channel." << std::endl;

    // 6. PART diffuse un message aux autres membres restants
    {
        std::map<std::string, Channel *> channels;
        Client clientA(8);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(9);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        std::string bufferBefore = clientA.getWriteBuffer();
        Commands::handlePart(clientB, makeParams("#general"), channels);
        std::string newPart = clientA.getWriteBuffer().substr(bufferBefore.size());
        assert(newPart.find("PART #general") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "6. PART broadcasts a message to the remaining members." << std::endl;

    // 7. PART avec un message de depart inclus dans la diffusion
    {
        std::map<std::string, Channel *> channels;
        Client clientA(10);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(11);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        std::string bufferBefore = clientA.getWriteBuffer();
        Commands::handlePart(clientB, makeParams("#general", "a plus tard"), channels);
        std::string newPart = clientA.getWriteBuffer().substr(bufferBefore.size());
        assert(newPart.find(":a plus tard") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "7. PART with a message includes it in the broadcast." << std::endl;

    // 8. PART sur plusieurs channels a la fois (liste separee par des virgules)
    {
        std::map<std::string, Channel *> channels;
        Client client(12);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#chan1,#chan2"), channels);

        Commands::handlePart(client, makeParams("#chan1,#chan2"), channels);
        assert(channels.empty()); // les deux channels sont detruits (dernier membre parti)
    }
    std::cout << "8. PART with a comma-separated list leaves every channel." << std::endl;

    // 9. Le channel survit si d'autres membres restent
    {
        std::map<std::string, Channel *> channels;
        Client clientA(13);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(14);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        Commands::handlePart(clientA, makeParams("#general"), channels);
        assert(channels.find("#general") != channels.end());
        assert(channels["#general"]->getMemberCount() == 1);
        clearChannels(channels);
    }
    std::cout << "9. The channel survives if other members remain." << std::endl;

    // 10. Le statut operateur est perdu quand on part, si on rejoint on ne le recupere pas automatiquement
    {
        std::map<std::string, Channel *> channels;
        Client client(15);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#general"), channels); // createur = operateur

        Client clientB(16);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels); // maintient le channel vivant

        Commands::handlePart(client, makeParams("#general"), channels);
        assert(!channels["#general"]->isOperator(15));
        clearChannels(channels);
    }
    std::cout << "10. Operator status is dropped when a member parts." << std::endl;

    std::cout << COLOR_GREEN << "\nAll PART command tests passed!" << COLOR_RESET << std::endl;

    return 0;
}
