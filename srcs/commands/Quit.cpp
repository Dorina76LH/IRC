#include "../../includes/Commands.hpp"

// void Commands::handleQuit(Client &client, const std::vector<std::string> &params, std::map<std::string, Channel *> &channels)
// {
//     // RFC 1459 (4.1.6) : Le message par défaut est le pseudonyme du client
//     std::string reason = client.getNickname();

//     if (!params.empty() && !params[0].empty())
//     {
//         reason = params[0];
//         if (reason[0] == ':')
//             reason = reason.substr(1);
//     }

//     // Message de broadcast RFC 1459
//     std::string quitMessage = ":" + client.getNickname() + "!" + client.getUsername() 
//                             + "@localhost QUIT :" + reason + "\r\n";

//     // Informer tous les membres des canaux où se trouve le client
//     for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
//     {
//         Channel *channel = it->second;
//         if (channel->isMember(client.getFdSocket()))
//         {
//             channel->broadcast(quitMessage);
//             channel->removeMember(client.getFdSocket());
//         }
//     }

//     // Demande de déconnexion
//     client.setShouldDisconnect(true);
// }