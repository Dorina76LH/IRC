#include "../includes/Commands.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -std=c++98 -Wall -Wextra -Werror tests/test_join.cpp srcs/Client.cpp srcs/Channel.cpp srcs/commands/Join.cpp -o test_join

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
    // Security : JOIN avant que le client ne soit enregistre (451)
    {
        std::map<std::string, Channel *> channels;
        Client client(0);

        Commands::handleJoin(client, makeParams("#general"), channels);
        assert(client.getWriteBuffer().find(" 451 ") != std::string::npos);
        assert(channels.empty());
        clearChannels(channels);
    }
    std::cout << "0. JOIN before registration rejected with 451." << std::endl;

    // 1. JOIN sans parametre : 461
    {
        std::map<std::string, Channel *> channels;
        Client client(1);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handleJoin(client, std::vector<std::string>(), channels);
        assert(client.getWriteBuffer().find(" 461 ") != std::string::npos);
        assert(channels.empty());
        clearChannels(channels);
    }
    std::cout << "1. JOIN without parameters rejected with 461." << std::endl;

    // 2. Premier client sur un nouveau channel : le channel est cree, le client devient membre + operateur
    {
        std::map<std::string, Channel *> channels;
        Client client(2);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handleJoin(client, makeParams("#general"), channels);
        assert(channels.find("#general") != channels.end());
        Channel *channel = channels["#general"];
        assert(channel->isMember(2));
        assert(channel->isOperator(2));
        assert(client.getWriteBuffer().find("JOIN :#general") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "2. First client on a new channel becomes member and operator." << std::endl;

    // 3. Nom de channel invalide : 476
    {
        std::map<std::string, Channel *> channels;
        Client client(3);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handleJoin(client, makeParams("general"), channels);
        assert(client.getWriteBuffer().find(" 476 ") != std::string::npos);
        assert(channels.empty());
        clearChannels(channels);
    }
    std::cout << "3. Invalid channel name (missing '#') rejected with 476." << std::endl;

    // 4. Deuxieme client rejoint un channel existant sans restriction
    {
        std::map<std::string, Channel *> channels;
        Client clientA(4);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(5);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        Channel *channel = channels["#general"];
        assert(channel->isMember(5));
        assert(!channel->isOperator(5)); // pas operateur, seul le createur l'est
        assert(channel->getMemberCount() == 2);
        clearChannels(channels);
    }
    std::cout << "4. Second client joins an existing channel as a regular member." << std::endl;

    // 5. Channel invite-only : rejet si non invite (473)
    {
        std::map<std::string, Channel *> channels;
        Client clientA(6);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#private"), channels);
        channels["#private"]->setInviteOnly(true);

        Client clientB(7);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#private"), channels);

        assert(!channels["#private"]->isMember(7));
        assert(clientB.getWriteBuffer().find(" 473 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "5. Invite-only channel rejects a non-invited client with 473." << std::endl;

    // 6. Channel invite-only : accepte si invite
    {
        std::map<std::string, Channel *> channels;
        Client clientA(8);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#private"), channels);
        channels["#private"]->setInviteOnly(true);
        channels["#private"]->addInvited(9);

        Client clientB(9);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#private"), channels);

        assert(channels["#private"]->isMember(9));
        clearChannels(channels);
    }
    std::cout << "6. Invite-only channel accepts an invited client." << std::endl;

    // 7. Channel avec cle (mode k) : mauvaise cle rejetee (475)
    {
        std::map<std::string, Channel *> channels;
        Client clientA(10);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#secret"), channels);
        channels["#secret"]->setKey("sesame");

        Client clientB(11);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#secret", "wrong"), channels);

        assert(!channels["#secret"]->isMember(11));
        assert(clientB.getWriteBuffer().find(" 475 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "7. Channel key mismatch rejected with 475." << std::endl;

    // 8. Channel avec cle (mode k) : bonne cle acceptee
    {
        std::map<std::string, Channel *> channels;
        Client clientA(12);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#secret"), channels);
        channels["#secret"]->setKey("sesame");

        Client clientB(13);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#secret", "sesame"), channels);

        assert(channels["#secret"]->isMember(13));
        clearChannels(channels);
    }
    std::cout << "8. Correct channel key accepted." << std::endl;

    // 9. Channel plein (mode l) : rejet (471)
    {
        std::map<std::string, Channel *> channels;
        Client clientA(14);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#full"), channels);
        channels["#full"]->setUserLimit(1);

        Client clientB(15);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#full"), channels);

        assert(!channels["#full"]->isMember(15));
        assert(clientB.getWriteBuffer().find(" 471 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "9. Full channel (user limit reached) rejected with 471." << std::endl;

    // 10. JOIN sur plusieurs channels a la fois (liste separee par des virgules)
    {
        std::map<std::string, Channel *> channels;
        Client client(16);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handleJoin(client, makeParams("#chan1,#chan2"), channels);
        assert(channels.find("#chan1") != channels.end());
        assert(channels.find("#chan2") != channels.end());
        assert(channels["#chan1"]->isMember(16));
        assert(channels["#chan2"]->isMember(16));
        clearChannels(channels);
    }
    std::cout << "10. JOIN with a comma-separated list joins every channel." << std::endl;

    std::cout << COLOR_GREEN << "\nAll JOIN command tests passed!" << COLOR_RESET << std::endl;

    return 0;
}
