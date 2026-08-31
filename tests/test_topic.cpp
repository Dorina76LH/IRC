#include "../includes/Commands.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cassert>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -std=c++98 -Wall -Wextra -Werror tests/test_topic.cpp srcs/Client.cpp srcs/Channel.cpp srcs/commands/Join.cpp srcs/commands/Topic.cpp -o test_topic

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
    // Security : TOPIC avant que le client ne soit enregistre (451)
    {
        std::map<std::string, Channel *> channels;
        Client client(0);

        Commands::handleTopic(client, makeParams("#general"), channels);
        assert(client.getWriteBuffer().find(" 451 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "0. TOPIC before registration rejected with 451." << std::endl;

    // 1. TOPIC sans parametre : 461
    {
        std::map<std::string, Channel *> channels;
        Client client(1);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handleTopic(client, std::vector<std::string>(), channels);
        assert(client.getWriteBuffer().find(" 461 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "1. TOPIC without parameters rejected with 461." << std::endl;

    // 2. TOPIC sur un channel inexistant : 403
    {
        std::map<std::string, Channel *> channels;
        Client client(2);
        client.setNickname("ada");
        client.setRegistered(true);

        Commands::handleTopic(client, makeParams("#ghost"), channels);
        assert(client.getWriteBuffer().find(" 403 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "2. TOPIC on a non-existent channel rejected with 403." << std::endl;

    // 3. TOPIC alors qu'on n'est pas membre du channel : 442
    {
        std::map<std::string, Channel *> channels;
        Client clientA(3);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(4);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleTopic(clientB, makeParams("#general"), channels);
        assert(clientB.getWriteBuffer().find(" 442 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "3. TOPIC from a non-member rejected with 442." << std::endl;

    // 4. Consultation d'un topic non defini : 331
    {
        std::map<std::string, Channel *> channels;
        Client client(5);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#general"), channels);
        client.appendToWriteBuffer(""); // vide le buffer JOIN pour lire la reponse suivante seule
        std::string bufferBeforeTopic = client.getWriteBuffer();

        Commands::handleTopic(client, makeParams("#general"), channels);
        std::string newPart = client.getWriteBuffer().substr(bufferBeforeTopic.size());
        assert(newPart.find(" 331 ") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "4. Viewing an unset topic returns 331." << std::endl;

    // 5. Definir un topic (client simple, mode t desactive par defaut)
    {
        std::map<std::string, Channel *> channels;
        Client client(6);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#general"), channels);

        Commands::handleTopic(client, makeParams("#general", "Nouveau sujet"), channels);
        assert(channels["#general"]->getTopic() == "Nouveau sujet");
        clearChannels(channels);
    }
    std::cout << "5. Setting a topic updates the channel (mode t off by default)." << std::endl;

    // 6. Consultation d'un topic deja defini : 332
    {
        std::map<std::string, Channel *> channels;
        Client client(7);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#general"), channels);
        Commands::handleTopic(client, makeParams("#general", "Sujet existant"), channels);

        std::string bufferBeforeTopic = client.getWriteBuffer();
        Commands::handleTopic(client, makeParams("#general"), channels);
        std::string newPart = client.getWriteBuffer().substr(bufferBeforeTopic.size());
        assert(newPart.find(" 332 ") != std::string::npos);
        assert(newPart.find("Sujet existant") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "6. Viewing an already-set topic returns 332 with its content." << std::endl;

    // 7. Mode t actif : un non-operateur ne peut pas changer le topic (482)
    {
        std::map<std::string, Channel *> channels;
        Client clientA(8);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);
        channels["#general"]->setTopicRestricted(true);

        Client clientB(9);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        Commands::handleTopic(clientB, makeParams("#general", "Tentative"), channels);
        assert(clientB.getWriteBuffer().find(" 482 ") != std::string::npos);
        assert(channels["#general"]->getTopic().empty());
        clearChannels(channels);
    }
    std::cout << "7. Non-operator changing a restricted topic (+t) rejected with 482." << std::endl;

    // 8. Mode t actif : l'operateur peut changer le topic
    {
        std::map<std::string, Channel *> channels;
        Client client(10);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#general"), channels); // ada = createur = operateur
        channels["#general"]->setTopicRestricted(true);

        Commands::handleTopic(client, makeParams("#general", "Sujet operateur"), channels);
        assert(channels["#general"]->getTopic() == "Sujet operateur");
        clearChannels(channels);
    }
    std::cout << "8. Operator can change a restricted topic (+t)." << std::endl;

    // 9. Vider le topic avec un parametre vide (trailing param vide, ex: "TOPIC #chan :")
    {
        std::map<std::string, Channel *> channels;
        Client client(11);
        client.setNickname("ada");
        client.setRegistered(true);
        Commands::handleJoin(client, makeParams("#general"), channels);
        Commands::handleTopic(client, makeParams("#general", "Sujet a effacer"), channels);

        Commands::handleTopic(client, makeParams("#general", ""), channels);
        assert(channels["#general"]->getTopic().empty());
        clearChannels(channels);
    }
    std::cout << "9. Setting an empty topic clears it." << std::endl;

    // 10. Changer le topic diffuse le changement a tous les membres
    {
        std::map<std::string, Channel *> channels;
        Client clientA(12);
        clientA.setNickname("ada");
        clientA.setRegistered(true);
        Commands::handleJoin(clientA, makeParams("#general"), channels);

        Client clientB(13);
        clientB.setNickname("bob");
        clientB.setRegistered(true);
        Commands::handleJoin(clientB, makeParams("#general"), channels);

        std::string bufferBeforeTopic = clientB.getWriteBuffer();
        Commands::handleTopic(clientA, makeParams("#general", "Sujet partage"), channels);
        std::string newPart = clientB.getWriteBuffer().substr(bufferBeforeTopic.size());
        assert(newPart.find("TOPIC #general :Sujet partage") != std::string::npos);
        clearChannels(channels);
    }
    std::cout << "10. Changing the topic broadcasts the new topic to every member." << std::endl;

    std::cout << COLOR_GREEN << "\nAll TOPIC command tests passed!" << COLOR_RESET << std::endl;

    return 0;
}
