#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

/*
    [] RPL_INVITING (341)
    [X] ERR_NEEDMOREPARAMS (461)
    [X] ERR_NOSUCHCHANNEL (403)
    [X] ERR_NOTONCHANNEL (442)
    [X] ERR_CHANOPRIVSNEEDED (482)
    [X] ERRUSERONCHANNEL (443) 
    [X] ERR_NOSUCHNICK (401)
    [] RPL_AWAY (301)
*/
void Commands::handleInvite(Client &client, const std::vector<std::string> &commandParams,
                            std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients)
{
    //* STEP 1 : Build the replyTarget
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    //* STEP 2 : Check registration
    // 451 ERR_NOTREGISTERED
    // "<client> :You have not registered"
    if (!client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOTREGISTERED, replyTarget, MSG_NOTREGISTERED));
        return;
    }

    //* STEP 3 : Check parameters count (nickname and channel name)
    // 461 ERR_NEEDMOREPARAMS
    // "<client> INVITE :Not enough parameters"
    if (commandParams.size() < 2 || commandParams[0].empty() || commandParams[1].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_NEEDMOREPARAMS, replyTarget, "INVITE", MSG_NEEDMOREPARAMS));
        return;
    }

    //& Set varriables for nickname and channelName from commandParams
    const std::string &nickname = commandParams[0];
    const std::string &channelName = commandParams[1];

    //* STEP 4 : Check channel mask format (valid channel name)
    // 476 ERR_BADCHANMASK
    if (!Channel::isValidChannelName(channelName))
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_BADCHANMASK, replyTarget, channelName, MSG_BADCHANMASK));
        return;
    }
    
    //* STEP 5 : Check channel existence
    // 403 ERR_NOSUCHCHANNEL
    // "<client> <channel> :No such channel"
    //& Set variable for channel iterator
    std::map<std::string, Channel *>::iterator channelIt = channels.find(channelName);

    //& Look for the channel in the channels map
    if (channelIt == channels.end())
    {
        std::string param = channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOSUCHCHANNEL, replyTarget, param, MSG_NOSUCHCHANNEL));
        return;
    }

    //* STEP 6 : Check if inviter is on channel
    // 442 ERR_NOTONCHANNEL
    // "<client> <channel> :You're not on that channel"

    //& Set pointer to the target channel
    Channel *channel = channelIt->second;

    //& Check if the client is a member of the channel
    if (!channel->isMember(client.getFdSocket()))
    {
        std::string param = channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOTONCHANNEL, replyTarget, param, MSG_NOTONCHANNEL));
        return;

    }

    //* STEP 7 : Check operator privileges if the channel is invite-only (mode i)
    // 482 ERR_CHANOPRIVSNEEDED
    // "<client> <channel> :You're not channel operator"
    if (channel->isInviteOnly() && !channel->isOperator(client.getFdSocket()))
    {
        std::string param = channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_CHANOPRIVSNEEDED, replyTarget, param, MSG_CHANOPRIVSNEEDED));
        return;
    }
    
    //* STEP 8 : Check if the target client exists
    // 401 ERR_NOSUCHNICK
    // "<client> <nickname> :No such nick/channel"
    
    //& Set variables
    Client *targetClient = NULL;
    std::map<int, Client *>::iterator it;

    //& Loop through the clients map to find the target client by nickname
    for (it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second &&
            //it->second->isRegistered() &&
            it->second->getNickname() == nickname)
        {
            targetClient = it->second;
            break;
        }
    }
    
    if (!targetClient)
    {
        std::string param = nickname;
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOSUCHNICK, replyTarget, param, MSG_NOSUCHNICK));
        return;
    }

    //* STEP 9 : Check if target is already on channel
    // 443 ERR_USERONCHANNEL
    // "<client> <nick> <channel> :is already on channel"
    if (channel->isMember(targetClient->getFdSocket()))
    {
        std::string param = targetClient->getNickname() + " " + channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_USERONCHANNEL, replyTarget, param, MSG_USERONCHANNEL));
        return;
    }

    //* STEP 10 : Execute INVITE
    // 341 RPL_INVITING
    // "<client> <nick> <channel>"
    
    //& Add the target client to the channel's invited list
    channel->addInvited(targetClient->getFdSocket());
    
    //& Send a confirmation message to the inviting client
    std::string param = targetClient->getNickname() + " " + channelName;
    client.appendToWriteBuffer(Commands::buildReply(RPL_INVITING, replyTarget, param, ""));
    
    //& Send an invitation message to the invited client (targetClient)
    // RFC 1459 (2.3.1)
    // <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
    // <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
    std::string prefix = client.getNickname() + "!" + client.getUsername() + "@" + HOSTNAME;
    std::string inviteNotice = ":" + prefix + " INVITE " + targetClient->getNickname() + " :" + channelName + "\r\n";
    targetClient->appendToWriteBuffer(inviteNotice);
}
