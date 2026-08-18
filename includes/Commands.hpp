#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class Commands
{
    public:
        /*
        Builds an IRC server reply: ":ircserv <code> <target> :<message>\r\n"
        Used by every command handler to send numeric replies to a client.
        */
        static std::string buildReply(const std::string &code, const std::string &target, const std::string &message)
        {
            return (":ircserv " + code + " " + target + " :" + message + "\r\n");
        }

        /*
        Handles the NICK command (RFC 1459).
        client : the client who sent the NICK command
        commandParams : the parameters that followed "NICK" (commandParams[0] = requested nickname)
        nicknamesInUse : the nicknames of all other clients currently connected,
        used to reject a nickname that is already taken
        */
        static void handleNick(Client &client, const std::vector<std::string> &commandParams, const std::vector<std::string> &nicknamesInUse);

    private:

        // Utility class only: no instance should ever be created.
        Commands();
        ~Commands();
        Commands(const Commands &other);
        Commands &operator=(const Commands &other);
};

#endif
