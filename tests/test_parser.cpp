#include "../includes/Parser.hpp"
#include <iostream>

static void test(const std::string &line)
{
    std::string command;
    std::vector<std::string> params;

    Parser::parseLine(line, command, params);

    std::cout << "line: \"" << line << "\"" << std::endl;
    std::cout << "  command: \"" << command << "\"" << std::endl;
    for (size_t i = 0; i < params.size(); i++)
        std::cout << "  params[" << i << "]: \"" << params[i] << "\"" << std::endl;
    std::cout << std::endl;
}

int main()
{
    // 1. commande simple avec un seul parametre
    test("NICK ada");

    // 2. plusieurs parametres separes par un seul espace
    test("JOIN #chan1 #chan2");

    // 3. dernier parametre avec des espaces (trailing param)
    test("PRIVMSG #chan :salut tout le monde");

    // 4. commande seule, sans aucun parametre
    test("PASS");

    // 5. espaces multiples consecutifs entre les parametres
    test("JOIN   #chan1     #chan2");

    // 6. trailing param vide (le ':' est le dernier caractere de la ligne)
    test("TOPIC #chan :");

    // 7. un ':' qui n'est pas en debut de mot ne doit rien declencher
    test("PRIVMSG #chan :salut: comment ca va");

    // 8. ligne completement vide
    test("");

    // 9. ligne qui ne contient que des espaces
    test("   ");

    // 10. espace en fin de ligne, sans trailing param
    test("JOIN #chan ");

    return 0;
}
