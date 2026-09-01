#ifndef COMMANDS_HPP
#define COMMANDS_HPP

// --- INCLUDES ---
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "Client.hpp"
#include "../includes/Client.hpp"
#include "../includes/Bot.hpp"

// --- NUMERIC REPLIES ---
#define RPL_INVITING "341"
#define HOSTNAME "localhost"

// --- ERROR REPLIES ---
#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL "442"
#define ERR_USERONCHANNEL "443"
#define ERR_NOTREGISTERED "451"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_BADCHANMASK "476"
#define ERR_CHANOPRIVSNEEDED "482"

// --- Standard errors ---
#define MSG_NOSUCHNICK "No such nick/channel"
#define MSG_NOSUCHCHANNEL "No such channel"
#define MSG_UNKNOWNCOMMAND "Unknown command"
#define MSG_USERNOTINCHANNEL "They aren't on that channel"
#define MSG_NOTONCHANNEL "You're not on that channel"
#define MSG_USERONCHANNEL "is already on channel"
#define MSG_NOTREGISTERED "You have not registered"
#define MSG_NEEDMOREPARAMS "Not enough parameters"
#define MSG_ALREADYREGISTRED "You may not reregister"
#define MSG_BADCHANMASK "Bad Channel Mask"
#define MSG_CHANOPRIVSNEEDED "You're not channel operator"

class Channel;
class Server;

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
        Builds an IRC server reply with an extra middle parameter (e.g. the name of the
        command that failed): ":ircserv <code> <target> <param> :<message>\r\n"
        Use this instead of the 3-argument overload whenever <message> would otherwise
        need to embed a hardcoded ':' itself (that ':' must mark the start of the
        trailing parameter exactly once, not be duplicated).
        */
        static std::string buildReply(const std::string &code, const std::string &target, const std::string &param, const std::string &message)
        {
            return (":ircserv " + code + " " + target + " " + param + " :" + message + "\r\n");
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

		/*
        Handles the MODE command (RFC 1459).
        */
        static void handleMode(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels);

		/*
        Handles the QUIT command (RFC 1459).
        */
		static void handleQuit(Client &client, const std::vector<std::string> &commandParams, Server &server);
       
		/*
        Handles the JOIN command (RFC 1459).
        client : the client who sent the JOIN command
        commandParams : the parameters that followed "JOIN"
        (commandParams[0] = comma-separated channel names, commandParams[1] = optional
        comma-separated keys, matched to channel names by position)
        channels : all channels known to the server, keyed by channel name.
        A channel that does not exist yet is created and the joining client
        becomes its first operator.
        */
        static void handleJoin(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels);

        /*
        Handles the INVITE command (RFC 1459).
        Command : INVITE <nickname> <channel>
        client : the client who sent the INVITE command
        commandParams : the parameters that followed "INVITE"
        (commandParams[0] = nickname of the client to invite, commandParams[1] = channel name)
        channels : all channels known to the server, keyed by channel name.
        clients : every client currently connected to the server, keyed by fd socket.
        */
        static void handleInvite(Client &client, const std::vector<std::string> &commandParams,
                     std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients);
        
        /*
        Handles the KICK command (RFC 1459).
        Command : KICK <channel> <user> [<comment>]
        client : the client who sent the KICK command
        commandParams : the parameters that followed "KICK"
        (commandParams[0] = channel name, commandParams[1] = nickname of the client to kick, commandParams[2] = optional comment)
        channels : all channels known to the server, keyed by channel name.
        clients : every client currently connected to the server, keyed by fd socket.
        */
        static void handleKick(Client &client, const std::vector<std::string> &commandParams,
                       std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients);

        /*
        Handles the TOPIC command (RFC 1459).
        client : the client who sent the TOPIC command
        commandParams : the parameters that followed "TOPIC"
        (commandParams[0] = channel name, commandParams[1] = new topic, only present
        when the client wants to CHANGE the topic; absent commandParams[1] means the
        client only wants to VIEW the current topic, even if that means an empty string)
        channels : all channels known to the server, keyed by channel name.
        */
        static void handleTopic(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels);

        /*
        Handles the PART command (RFC 1459).
        client : the client who sent the PART command
        commandParams : the parameters that followed "PART"
        (commandParams[0] = comma-separated channel names, commandParams[1] = optional
        part message)
        channels : all channels known to the server, keyed by channel name.
        A channel that becomes empty after the client leaves is destroyed and removed
        from the map.
        */
        static void handlePart(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels);

        /*
        Handles the PRIVMSG command (RFC 1459).
        client : the client who sent the PRIVMSG command
        commandParams : the parameters that followed "PRIVMSG"
        (commandParams[0] = comma-separated targets, each either a channel name
        starting with '#'/'&' or a nickname ; commandParams[1] = the message)
        channels : all channels known to the server, keyed by channel name.
        clients : every client currently connected to the server, keyed by fd socket.
        Used to resolve a nickname target to the Client it belongs to.
        */
        static void handlePrivMsg(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients);
        
        /*
        Handles the HELP command
        client : the client who sent the HELP command
        commandParams : the parameters that followed "HELP"
        (commandParams[0] = optional command name to get help for, or empty to get a list of all commands)
        bot : the bot instance, used to access the help map and print help messages
        Used to provide help information to the client about available commands and their usage.
        */
        static void handleHelp(Client &client, const std::vector<std::string> &commandParams, Bot &bot);
    
    private:

        // Utility class only: no instance should ever be created.
        Commands();
        ~Commands();
        Commands(const Commands &other);
        Commands &operator=(const Commands &other);
};

#endif
