#include "../includes/Commands.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -std=c++98 -Wall -Wextra -Werror tests/test_privmsg.cpp srcs/Client.cpp srcs/Channel.cpp srcs/commands/Join.cpp srcs/commands/PrivMsg.cpp -o test_privmsg

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
    // 1. PRIVMSG sans cible : 411
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client client(1);
        client.setNickname("ada");
        clients[1] = &client;

        Commands::handlePrivMsg(client, std::vector<std::string>(), channels, clients);
        assert(client.getWriteBuffer().find(" 411 ") != std::string::npos);
    }
    std::cout << "1. PRIVMSG without target rejected with 411." << std::endl;

    // 2. PRIVMSG sans message : 412
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client client(2);
        client.setNickname("ada");
        clients[2] = &client;

        Commands::handlePrivMsg(client, makeParams("bob"), channels, clients);
        assert(client.getWriteBuffer().find(" 412 ") != std::string::npos);
    }
    std::cout << "2. PRIVMSG without a message rejected with 412." << std::endl;

    // 3. PRIVMSG vers un nickname inconnu : 401
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client client(3);
        client.setNickname("ada");
        clients[3] = &client;

        Commands::handlePrivMsg(client, makeParams("ghost", "salut"), channels, clients);
        assert(client.getWriteBuffer().find(" 401 ") != std::string::npos);
    }
    std::cout << "3. PRIVMSG to an unknown nickname rejected with 401." << std::endl;

    // 4. PRIVMSG vers un nickname existant : le destinataire recoit le message
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client clientA(4);
        clientA.setNickname("ada");
        clients[4] = &clientA;

        Client clientB(5);
        clientB.setNickname("bob");
        clients[5] = &clientB;

        Commands::handlePrivMsg(clientA, makeParams("bob", "salut bob"), channels, clients);
        assert(clientB.getWriteBuffer().find("PRIVMSG bob :salut bob") != std::string::npos);
        assert(clientA.getWriteBuffer().empty()); // aucune erreur renvoyee a l'expediteur
    }
    std::cout << "4. PRIVMSG to an existing nickname delivers the message." << std::endl;

    // 5. PRIVMSG vers un channel inexistant : 403
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client client(6);
        client.setNickname("ada");
        clients[6] = &client;

        Commands::handlePrivMsg(client, makeParams("#ghost", "salut"), channels, clients);
        assert(client.getWriteBuffer().find(" 403 ") != std::string::npos);
    }
    std::cout << "5. PRIVMSG to a non-existent channel rejected with 403." << std::endl;

    // 6. PRIVMSG vers un channel dont on n'est pas membre : 404
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client clientA(7);
        clientA.setNickname("ada");
        clients[7] = &clientA;
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(8);
        clientB.setNickname("bob");
        clients[8] = &clientB;

        Commands::handlePrivMsg(clientB, makeParams("#general", "salut"), channels, clients);
        assert(clientB.getWriteBuffer().find(" 404 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "6. PRIVMSG to a channel you're not on rejected with 404." << std::endl;

    // 7. PRIVMSG vers un channel : tous les autres membres recoivent le message
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client clientA(9);
        clientA.setNickname("ada");
        clients[9] = &clientA;
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(10);
        clientB.setNickname("bob");
        clients[10] = &clientB;
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        std::string bufferBefore = clientB.getWriteBuffer();
        Commands::handlePrivMsg(clientA, makeParams("#general", "salut tout le monde"), channels, clients);
        std::string newPart = clientB.getWriteBuffer().substr(bufferBefore.size());
        assert(newPart.find("PRIVMSG #general :salut tout le monde") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "7. PRIVMSG to a channel reaches every other member." << std::endl;

    // 8. PRIVMSG vers un channel : l'expediteur ne recoit pas son propre message
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client clientA(11);
        clientA.setNickname("ada");
        clients[11] = &clientA;
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        std::string bufferBeforeSend = clientA.getWriteBuffer();
        Commands::handlePrivMsg(clientA, makeParams("#general", "salut"), channels, clients);
        assert(clientA.getWriteBuffer() == bufferBeforeSend); // rien de plus ajoute
    }
    std::cout << "8. PRIVMSG sender does not receive their own channel message." << std::endl;

    // 9. PRIVMSG vers plusieurs cibles a la fois (liste separee par des virgules)
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client clientA(12);
        clientA.setNickname("ada");
        clients[12] = &clientA;

        Client clientB(13);
        clientB.setNickname("bob");
        clients[13] = &clientB;

        Client clientC(14);
        clientC.setNickname("carl");
        clients[14] = &clientC;

        Commands::handlePrivMsg(clientA, makeParams("bob,carl", "salut a tous"), channels, clients);
        assert(clientB.getWriteBuffer().find("salut a tous") != std::string::npos);
        assert(clientC.getWriteBuffer().find("salut a tous") != std::string::npos);
    }
    std::cout << "9. PRIVMSG with a comma-separated list reaches every target." << std::endl;

    // 10. Le message envoye est prefixe par le nickname et le username de l'expediteur
    {
        std::map<std::string, Channel *> channels;
        std::map<int, Client *> clients;
        Client clientA(15);
        clientA.setNickname("ada");
        clientA.setUsername("ada_user");
        clients[15] = &clientA;

        Client clientB(16);
        clientB.setNickname("bob");
        clients[16] = &clientB;

        Commands::handlePrivMsg(clientA, makeParams("bob", "salut"), channels, clients);
        assert(clientB.getWriteBuffer().find(":ada!ada_user@localhost PRIVMSG bob :salut") != std::string::npos);
    }
    std::cout << "10. The message is prefixed with the sender's nickname and username." << std::endl;

    std::cout << COLOR_GREEN << "\nAll PRIVMSG command tests passed!" << COLOR_RESET << std::endl;

    return 0;
}
