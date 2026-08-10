#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>

// Codes numeriques de reponse IRC utilises par ft_irc (RFC 2812).
// A envoyer au format : ":servername CODE nickname :message"
namespace Reply
{
    const std::string RPL_WELCOME          = "001"; // connexion + auth OK
    const std::string RPL_YOURHOST         = "002";

    const std::string RPL_CHANNELMODEIS    = "324";
    const std::string RPL_NOTOPIC          = "331";
    const std::string RPL_TOPIC            = "332";
    const std::string RPL_NAMREPLY         = "353"; // liste des membres d'un channel
    const std::string RPL_ENDOFNAMES       = "366";
    const std::string RPL_INVITING         = "341";

    const std::string ERR_NOSUCHNICK       = "401";
    const std::string ERR_NOSUCHCHANNEL    = "403";
    const std::string ERR_CANNOTSENDTOCHAN = "404";
    const std::string ERR_UNKNOWNCOMMAND   = "421";
    const std::string ERR_NICKNAMEINUSE    = "433";
    const std::string ERR_USERNOTINCHANNEL = "441";
    const std::string ERR_NOTONCHANNEL     = "442";
    const std::string ERR_USERONCHANNEL    = "443";
    const std::string ERR_NOTREGISTERED    = "451"; // NICK/USER pas encore faits
    const std::string ERR_NEEDMOREPARAMS   = "461";
    const std::string ERR_ALREADYREGISTRED = "462";
    const std::string ERR_PASSWDMISMATCH   = "464";
    const std::string ERR_CHANNELISFULL    = "471"; // mode +l atteint
    const std::string ERR_UNKNOWNMODE      = "472";
    const std::string ERR_INVITEONLYCHAN   = "473"; // mode +i
    const std::string ERR_BADCHANNELKEY    = "475"; // mode +k
    const std::string ERR_CHANOPRIVSNEEDED = "482"; // reserve aux operateurs
}

// Signature commune que chaque handler de commande doit respecter.
// client  : l'auteur de la commande
// params  : les arguments deja separes (ex: JOIN #chan cle -> params = ["#chan", "cle"])
//
// Chaque fichier srcs/commands/<Nom>.cpp implemente UNE de ces fonctions,
// appelee par Server::processLine() via son dispatcher interne.
class Server;
class Client;

namespace Commands
{
    void pass(Server& server, Client& client, const std::vector<std::string>& params);
    void nick(Server& server, Client& client, const std::vector<std::string>& params);
    void user(Server& server, Client& client, const std::vector<std::string>& params);
    void join(Server& server, Client& client, const std::vector<std::string>& params);
    void part(Server& server, Client& client, const std::vector<std::string>& params);
    void quit(Server& server, Client& client, const std::vector<std::string>& params);
    void privmsg(Server& server, Client& client, const std::vector<std::string>& params);
    void topic(Server& server, Client& client, const std::vector<std::string>& params);
    void invite(Server& server, Client& client, const std::vector<std::string>& params);
    void kick(Server& server, Client& client, const std::vector<std::string>& params);
    void mode(Server& server, Client& client, const std::vector<std::string>& params);
}

#endif
