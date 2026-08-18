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

        /*
        Handles the PASS command (RFC 1459).
        Must be the first command sent by a client, before NICK/USER.
        client : the client who sent the PASS command
        commandParams : the parameters that followed "PASS" (commandParams[0] = password attempt)
        serverPassword : the password configured for this server instance
        */
        static void handlePass(Client &client, const std::vector<std::string> &commandParams, const std::string &serverPassword);

        /*
        Handles the USER command (RFC 1459).
        client : the client who sent the USER command
        commandParams : the parameters that followed "USER"
        (commandParams[0] = username, commandParams[1] = hostname, commandParams[2] = servername,
        commandParams[3] = realname). hostname/servername are client-supplied and untrusted,
        so they are accepted but ignored, as most real-world IRC servers do.
        */
        static void handleUser(Client &client, const std::vector<std::string> &commandParams);

    private:

        // Utility class only: no instance should ever be created.
        Commands();
        ~Commands();
        Commands(const Commands &other);
        Commands &operator=(const Commands &other);
};

#endif
