//* -- Includes -- *//
#include "../includes/Commands.hpp"
#include <iostream>
#include <cassert>

//* -- Colors -- *//
#define COLOR_RESET "\033[0m"
#define COLOR_CYAN  "\033[1;36m"
#define COLOR_GREEN "\033[1;32m"

//* -- Compilation and Execution Instructions -- *//
// c++ -std=c++98 -Wall -Wextra -Werror tests/test_commands.cpp srcs/Client.cpp srcs/commands/Pass.cpp srcs/commands/Nick.cpp srcs/commands/User.cpp -o test_commands

//& assert(condition) : if condition is false, the program aborts immediately
//& and prints the failing condition, file, and line number to stderr.
//& This lets us catch bugs early — as soon as one assertion fails, execution
//& stops, so all assertions ABOVE it in the code are known to have passed.

static std::vector<std::string> makeParams(const std::string &p0)
{
    std::vector<std::string> params;
    params.push_back(p0);
    return (params);
}

static std::vector<std::string> makeParams(const std::string &p0, const std::string &p1, const std::string &p2, const std::string &p3)
{
    std::vector<std::string> params;
    params.push_back(p0);
    params.push_back(p1);
    params.push_back(p2);
    params.push_back(p3);
    return (params);
}

int main()
{
    const std::string SERVER_PASSWORD = "secret";

    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "PASS COMMAND TESTS" << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;

    //& Correct password authenticates the client, no error reply
    std::cout << COLOR_CYAN << "=== Testing PASS with correct password ===" << COLOR_RESET << std::endl;
    Client clientA(1);
    Commands::handlePass(clientA, makeParams(SERVER_PASSWORD), SERVER_PASSWORD);
    assert(clientA.isAuthenticated());
    assert(!clientA.hasDataToSend());
    std::cout << "✅ Correct password authenticates the client with no error reply." << std::endl;

    //& Wrong password: rejected, ERR_PASSWDMISMATCH (464), not authenticated
    std::cout << COLOR_CYAN << "\n=== Testing PASS with wrong password ===" << COLOR_RESET << std::endl;
    Client clientB(2);
    Commands::handlePass(clientB, makeParams("wrong"), SERVER_PASSWORD);
    assert(!clientB.isAuthenticated());
    assert(clientB.getWriteBuffer().find(" 464 ") != std::string::npos);
    std::cout << "✅ Wrong password rejected with 464 and client not authenticated." << std::endl;

    //& No password given: ERR_NEEDMOREPARAMS (461)
    std::cout << COLOR_CYAN << "\n=== Testing PASS with no parameters ===" << COLOR_RESET << std::endl;
    Client clientC(3);
    std::vector<std::string> emptyParams;
    Commands::handlePass(clientC, emptyParams, SERVER_PASSWORD);
    assert(!clientC.isAuthenticated());
    assert(clientC.getWriteBuffer().find(" 461 ") != std::string::npos);
    std::cout << "✅ Missing password rejected with 461." << std::endl;

    //& Empty string password param: ERR_NEEDMOREPARAMS (461)
    std::cout << COLOR_CYAN << "\n=== Testing PASS with empty password parameter ===" << COLOR_RESET << std::endl;
    Client clientD(4);
    Commands::handlePass(clientD, makeParams(""), SERVER_PASSWORD);
    assert(!clientD.isAuthenticated());
    assert(clientD.getWriteBuffer().find(" 461 ") != std::string::npos);
    std::cout << "✅ Empty password parameter rejected with 461." << std::endl;

    //& Already registered client trying to PASS again: ERR_ALREADYREGISTRED (462)
    std::cout << COLOR_CYAN << "\n=== Testing PASS on an already registered client ===" << COLOR_RESET << std::endl;
    Client clientE(5);
    clientE.setRegistered(true);
    Commands::handlePass(clientE, makeParams(SERVER_PASSWORD), SERVER_PASSWORD);
    assert(!clientE.isAuthenticated());
    assert(clientE.getWriteBuffer().find(" 462 ") != std::string::npos);
    std::cout << "✅ PASS on already registered client rejected with 462." << std::endl;

    std::cout << COLOR_GREEN << "\nAll PASS command tests passed!" << COLOR_RESET << std::endl;

    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "NICK COMMAND TESTS" << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;

    std::vector<std::string> noNicknamesInUse;

    //& Security: NICK before PASS validated, ERR_NOTREGISTERED (451)
    std::cout << COLOR_CYAN << "=== Testing NICK before PASS was validated ===" << COLOR_RESET << std::endl;
    Client clientNoAuth(9);
    Commands::handleNick(clientNoAuth, makeParams("bob"), noNicknamesInUse);
    assert(clientNoAuth.getNickname().empty());
    assert(clientNoAuth.getWriteBuffer().find(" 451 ") != std::string::npos);
    std::cout << " NICK before PASS rejected with 451." << std::endl;

    //& Valid nickname, no collision: nickname is set, no error reply
    std::cout << COLOR_CYAN << "=== Testing NICK with a valid nickname ===" << COLOR_RESET << std::endl;
    Client clientF(10);
    clientF.setAuthenticated(true);
    Commands::handleNick(clientF, makeParams("ada"), noNicknamesInUse);
    assert(clientF.getNickname() == "ada");
    assert(!clientF.hasDataToSend());
    std::cout << "✅ Valid nickname correctly set with no error reply." << std::endl;

    //& No nickname given: ERR_NONICKNAMEGIVEN (431)
    std::cout << COLOR_CYAN << "\n=== Testing NICK with no nickname given ===" << COLOR_RESET << std::endl;
    Client clientG(11);
    clientG.setAuthenticated(true);
    std::vector<std::string> emptyNickParams;
    Commands::handleNick(clientG, emptyNickParams, noNicknamesInUse);
    assert(clientG.getNickname().empty());
    assert(clientG.getWriteBuffer().find(" 431 ") != std::string::npos);
    std::cout << "✅ Missing nickname rejected with 431." << std::endl;

    //& Invalid syntax: starts with a digit, ERR_ERRONEUSNICKNAME (432)
    std::cout << COLOR_CYAN << "\n=== Testing NICK starting with a digit ===" << COLOR_RESET << std::endl;
    Client clientH(12);
    clientH.setAuthenticated(true);
    Commands::handleNick(clientH, makeParams("1nick"), noNicknamesInUse);
    assert(clientH.getNickname().empty());
    assert(clientH.getWriteBuffer().find(" 432 ") != std::string::npos);
    std::cout << "✅ Nickname starting with a digit rejected with 432." << std::endl;

    //& Invalid syntax: too long (> 9 characters), ERR_ERRONEUSNICKNAME (432)
    std::cout << COLOR_CYAN << "\n=== Testing NICK too long (> 9 characters) ===" << COLOR_RESET << std::endl;
    Client clientI(13);
    clientI.setAuthenticated(true);
    Commands::handleNick(clientI, makeParams("waytoolongnick"), noNicknamesInUse);
    assert(clientI.getNickname().empty());
    assert(clientI.getWriteBuffer().find(" 432 ") != std::string::npos);
    std::cout << "✅ Nickname over 9 characters rejected with 432." << std::endl;

    //& Invalid syntax: disallowed character, ERR_ERRONEUSNICKNAME (432)
    std::cout << COLOR_CYAN << "\n=== Testing NICK with a disallowed character ===" << COLOR_RESET << std::endl;
    Client clientJ(14);
    clientJ.setAuthenticated(true);
    Commands::handleNick(clientJ, makeParams("bad!nick"), noNicknamesInUse);
    assert(clientJ.getNickname().empty());
    assert(clientJ.getWriteBuffer().find(" 432 ") != std::string::npos);
    std::cout << "✅ Nickname with a disallowed character rejected with 432." << std::endl;

    //& Valid syntax: RFC 1459 special characters allowed
    std::cout << COLOR_CYAN << "\n=== Testing NICK with allowed special characters ===" << COLOR_RESET << std::endl;
    Client clientK(15);
    clientK.setAuthenticated(true);
    Commands::handleNick(clientK, makeParams("a-b[c]"), noNicknamesInUse);
    assert(clientK.getNickname() == "a-b[c]");
    assert(!clientK.hasDataToSend());
    std::cout << "✅ Nickname with allowed special characters correctly set." << std::endl;

    //& Nickname already in use: ERR_NICKNAMEINUSE (433)
    std::cout << COLOR_CYAN << "\n=== Testing NICK already in use ===" << COLOR_RESET << std::endl;
    Client clientL(16);
    clientL.setAuthenticated(true);
    std::vector<std::string> nicknamesInUse;
    nicknamesInUse.push_back("bob");
    Commands::handleNick(clientL, makeParams("bob"), nicknamesInUse);
    assert(clientL.getNickname().empty());
    assert(clientL.getWriteBuffer().find(" 433 ") != std::string::npos);
    std::cout << "✅ Nickname collision rejected with 433." << std::endl;

    //& RFC 1459, 2.2 : '{' '}' '|' are the lowercase equivalents of '[' ']' '\',
    //& so nickname uniqueness must be checked case-insensitively with this mapping.
    std::cout << COLOR_CYAN << "\n=== Testing NICK collision with IRC casemapping ({}|  <-> []\\ ) ===" << COLOR_RESET << std::endl;
    Client clientL2(17);
    clientL2.setAuthenticated(true);
    std::vector<std::string> casemappedNicknamesInUse;
    casemappedNicknamesInUse.push_back("ADA[");
    Commands::handleNick(clientL2, makeParams("ada{"), casemappedNicknamesInUse);
    assert(clientL2.getNickname().empty());
    assert(clientL2.getWriteBuffer().find(" 433 ") != std::string::npos);
    std::cout << "✅ \"ada{\" correctly collides with \"ADA[\" per IRC casemapping." << std::endl;

    std::cout << COLOR_GREEN << "\nAll NICK command tests passed!" << COLOR_RESET << std::endl;

    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "USER COMMAND TESTS" << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;

    //& Security: USER before PASS validated, ERR_NOTREGISTERED (451)
    std::cout << COLOR_CYAN << "=== Testing USER before PASS was validated ===" << COLOR_RESET << std::endl;
    Client clientNoAuth2(19);
    Commands::handleUser(clientNoAuth2, makeParams("art", "0", "*", "Bob Michel"));
    assert(clientNoAuth2.getUsername().empty());
    assert(clientNoAuth2.getWriteBuffer().find(" 451 ") != std::string::npos);
    std::cout << "✅ USER before PASS rejected with 451." << std::endl;

    //& Valid USER command: username and realname set, no error reply
    std::cout << COLOR_CYAN << "=== Testing USER with valid parameters ===" << COLOR_RESET << std::endl;
    Client clientM(20);
    clientM.setAuthenticated(true);
    Commands::handleUser(clientM, makeParams("ada", "0", "*", "Ada Real Name"));
    assert(clientM.getUsername() == "ada");
    assert(clientM.getRealname() == "Ada Real Name");
    assert(!clientM.hasDataToSend());
    std::cout << "✅ Valid USER command correctly sets username and realname." << std::endl;

    //& Not enough parameters (< 4): ERR_NEEDMOREPARAMS (461)
    std::cout << COLOR_CYAN << "\n=== Testing USER with not enough parameters ===" << COLOR_RESET << std::endl;
    Client clientN(21);
    clientN.setAuthenticated(true);
    std::vector<std::string> shortParams;
    shortParams.push_back("ada");
    shortParams.push_back("0");
    Commands::handleUser(clientN, shortParams);
    assert(clientN.getUsername().empty());
    assert(clientN.getWriteBuffer().find(" 461 ") != std::string::npos);
    std::cout << "✅ USER with too few parameters rejected with 461." << std::endl;

    //& Empty username parameter: ERR_NEEDMOREPARAMS (461)
    std::cout << COLOR_CYAN << "\n=== Testing USER with empty username ===" << COLOR_RESET << std::endl;
    Client clientO(22);
    clientO.setAuthenticated(true);
    Commands::handleUser(clientO, makeParams("", "0", "*", "Real Name"));
    assert(clientO.getUsername().empty());
    assert(clientO.getWriteBuffer().find(" 461 ") != std::string::npos);
    std::cout << "✅ USER with empty username rejected with 461." << std::endl;

    //& Empty realname parameter: ERR_NEEDMOREPARAMS (461)
    std::cout << COLOR_CYAN << "\n=== Testing USER with empty realname ===" << COLOR_RESET << std::endl;
    Client clientP(23);
    clientP.setAuthenticated(true);
    Commands::handleUser(clientP, makeParams("ada", "0", "*", ""));
    assert(clientP.getRealname().empty());
    assert(clientP.getWriteBuffer().find(" 461 ") != std::string::npos);
    std::cout << "✅ USER with empty realname rejected with 461." << std::endl;

    //& hostname/servername are accepted but ignored (not stored anywhere observable)
    std::cout << COLOR_CYAN << "\n=== Testing USER ignores hostname/servername ===" << COLOR_RESET << std::endl;
    Client clientQ(24);
    clientQ.setAuthenticated(true);
    Commands::handleUser(clientQ, makeParams("ada", "untrusted-host", "untrusted-server", "Ada"));
    assert(clientQ.getUsername() == "ada");
    assert(clientQ.getRealname() == "Ada");
    assert(!clientQ.hasDataToSend());
    std::cout << "✅ Client-supplied hostname/servername accepted but not trusted/stored." << std::endl;

    //& Already registered client trying USER again: ERR_ALREADYREGISTRED (462)
    std::cout << COLOR_CYAN << "\n=== Testing USER on an already registered client ===" << COLOR_RESET << std::endl;
    Client clientR(25);
    clientR.setRegistered(true);
    Commands::handleUser(clientR, makeParams("ada", "0", "*", "Ada Real Name"));
    assert(clientR.getUsername().empty());
    assert(clientR.getWriteBuffer().find(" 462 ") != std::string::npos);
    std::cout << "✅ USER on already registered client rejected with 462." << std::endl;

    std::cout << COLOR_GREEN << "\nAll USER command tests passed!" << COLOR_RESET << std::endl;

    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "FULL AUTHENTICATION FLOW TEST" << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;

    //& Simulates the full PASS -> NICK -> USER sequence for a single client
    std::cout << COLOR_CYAN << "=== Testing full PASS/NICK/USER sequence ===" << COLOR_RESET << std::endl;
    Client clientS(30);
    Commands::handlePass(clientS, makeParams(SERVER_PASSWORD), SERVER_PASSWORD);
    Commands::handleNick(clientS, makeParams("arthur"), noNicknamesInUse);
    Commands::handleUser(clientS, makeParams("arthur", "0", "*", "Arthur Beaufils"));
    assert(clientS.isAuthenticated());
    assert(clientS.getNickname() == "arthur");
    assert(clientS.getUsername() == "arthur");
    assert(clientS.getRealname() == "Arthur Beaufils");
    assert(!clientS.isRegistered()); // registration completion is Server's responsibility, not the handlers'
    assert(!clientS.hasDataToSend());
    std::cout << "✅ Full PASS/NICK/USER sequence correctly authenticates and fills client info." << std::endl;

    std::cout << COLOR_GREEN << "\nAll full authentication flow tests passed!" << COLOR_RESET << std::endl;

    return 0;
}
