#include "../includes/Parser.hpp"
#include <iostream>
#include <cassert>

#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[1;32m"

// c++ -std=c++98 -Wall -Wextra -Werror tests/test_parser.cpp srcs/Parser.cpp -o test_parser

static void checkParsing(const std::string &line, const std::string &expectedCommand, const std::vector<std::string> &expectedParams)
{
    std::string command;
    std::vector<std::string> params;

    Parser::parseLine(line, command, params);

    assert(command == expectedCommand);
    assert(params.size() == expectedParams.size());
    for (size_t i = 0; i < expectedParams.size(); i++)
        assert(params[i] == expectedParams[i]);
}

static std::vector<std::string> makeParams()
{
    return (std::vector<std::string>());
}

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

int main()
{
    // 1. commande simple avec un seul parametre
    checkParsing("NICK ada", "NICK", makeParams("ada"));

    // 2. plusieurs parametres separes par un seul espace
    checkParsing("JOIN #chan1 #chan2", "JOIN", makeParams("#chan1", "#chan2"));

    // 3. dernier parametre avec des espaces (trailing param)
    checkParsing("PRIVMSG #chan :salut tout le monde", "PRIVMSG", makeParams("#chan", "salut tout le monde"));

    // 4. commande seule, sans aucun parametre
    checkParsing("PASS", "PASS", makeParams());

    // 5. espaces multiples consecutifs entre les parametres
    checkParsing("JOIN   #chan1     #chan2", "JOIN", makeParams("#chan1", "#chan2"));

    // 6. trailing param vide (le ':' est le dernier caractere de la ligne)
    checkParsing("TOPIC #chan :", "TOPIC", makeParams("#chan", ""));

    // 7. un ':' qui n'est pas en debut de mot ne doit rien declencher
    checkParsing("PRIVMSG #chan :salut: comment ca va", "PRIVMSG", makeParams("#chan", "salut: comment ca va"));

    // 8. ligne completement vide
    checkParsing("", "", makeParams());

    // 9. ligne qui ne contient que des espaces
    checkParsing("   ", "", makeParams());

    // 10. espace en fin de ligne, sans trailing param
    checkParsing("JOIN #chan ", "JOIN", makeParams("#chan"));

    std::cout << COLOR_GREEN << "All Parser tests passed!" << COLOR_RESET << std::endl;

    return 0;
}
