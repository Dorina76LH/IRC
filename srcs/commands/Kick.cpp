#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

/*
    [] ERR_NEEDMOREPARAMS (461)
    [] ERR_BADCHANMASK (476)
    [] ERR_NOSUCHCHANNEL (403)
    [] ERR_NOTONCHANNEL (442)
    [] ERR_CHANOPRIVSNEEDED (482)
    [] ERRUSERONCHANNEL (443) 
    [] ERR_NOSUCHNICK (401)
    
    Command: KICK
    Parameters: <channel> <user> *( "," <user> ) [<comment>]
*/
void Commands::handleKick(Client &client, const std::vector<std::string> &commandParams,
                            std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients)
{
    //* STEP 1 : Build the reply target based on whether the client has a nickname or not.
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    //* STEP 2 : Check if the command has enough parameters (nickname and channel name).
    // 461 ERR_NEEDMOREPARAMS
    if (commandParams.size() < 2 || commandParams[0].empty() || commandParams[1].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_NEEDMOREPARAMS, replyTarget, "KICK", MSG_NEEDMOREPARAMS));
        return;
    }

    //& Set varriables for nickname and channelName from commandParams
    const std::string &channelName = commandParams[0];
    const std::string &nickname = commandParams[1];
    //& Optional comment parameter
    // recuperer le message personnalise si existe, sinon mettre un message par defaut
    //TODO i'm here

    //* STEP 3 : Check if the user is registered (has a nickname, username, and realname).
    // 451 ERR_NOTREGISTERED
    if (!client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOTREGISTERED, replyTarget, MSG_NOTREGISTERED));
        return;
    }

    //* STEP 4 : Check if the target client exists and is registered (has a nickname, username, and realname).
    
    //& Set variables
    Client *targetClient = NULL;
    std::map<int, Client *>::iterator it;

    //& Loop through the clients map to find the target client by nickname
    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second &&
            it->second->isRegistered() &&
            it->second->getNickname() == nickname)
        {
            targetClient = it->second;
            break;
        }
    }
    
    // 401 ERR_NOSUCHNICK
    // "<client> <nickname> :No such nick/channel"
    if (!targetClient)
    {
        std::string param = nickname;
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOSUCHNICK, replyTarget, param, MSG_NOSUCHNICK));
        return;
    }
    
    //* STEP 5 : Find the channel in the server's channel map.
    
    //& Set variable for channel iterator
    std::map<std::string, Channel *>::iterator channelIt = channels.find(channelName);

    // Look for the channel in the channels map
    // 403 ERR_NOSUCHCHANNEL
    // "<client> <channel> :No such channel"
    if (channelIt == channels.end())
    {
        std::string param = channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOSUCHCHANNEL, replyTarget, param, MSG_NOSUCHCHANNEL));
        return;
    }

    //* STEP 6 : Check if the client is a member of the channel

    //& Set pointer to the target channel
    Channel *channel = channelIt->second;

    //& Check if the client is a member of the channel
    // 442 ERR_NOTONCHANNEL
    // "<client> <channel> :You're not on that channel"
    if (!channel->isMember(client.getFdSocket()))
    {
        std::string param = channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOTONCHANNEL, replyTarget, param, MSG_NOTONCHANNEL));
        return;

    }

    //* STEP 7 : Check if the channel is invite-only and if the client is an operator of the channel
    // 482 ERR_CHANOPRIVSNEEDED
    // "<client> <channel> :You're not channel operator"
    if (channel->isInviteOnly() && !channel->isOperator(client.getFdSocket()))
    {
        std::string param = channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_CHANOPRIVSNEEDED, replyTarget, param, MSG_CHANOPRIVSNEEDED));
        return;
    }
    
    //* STEP 8 : Check if the target client is already a member of the channel
    // 443 ERR_USERONCHANNEL
    // "<client> <nick> <channel> :is already on channel"
    if (channel->isMember(targetClient->getFdSocket()))
    {
        std::string param = targetClient->getNickname() + " " + channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_USERONCHANNEL, replyTarget, param, MSG_USERONCHANNEL));
        return;
    }

    //* STEP 9 : Success execution - remove the target client to the channel
    
    //& Add the target client to the channel's invited list
    channel->addInvited(targetClient->getFdSocket());
    
    //& Send a confirmation message to the inviting client
    // 341 RPL_INVITING
    // "<client> <nick> <channel>"
    std::string param = targetClient->getNickname() + " " + channelName;
    client.appendToWriteBuffer(Commands::buildReply(RPL_INVITING, replyTarget, param, ""));
    
    //& Send an invitation message to the invited client (targetClient)
    // RFC 1459 (2.3.1)
    // <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
    // <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
    std::string prefix = client.getNickname() + "!" + client.getUsername() + "@" + HOSTNAME;
    std::string inviteNotice = ":" + prefix + " KICK " + targetClient->getNickname() + " :" + channelName + "\r\n";
    targetClient->appendToWriteBuffer(inviteNotice);
}