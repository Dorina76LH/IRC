#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

/*
    Command: KICK
    Parameters: <channel> <user> *( "," <user> ) [<comment>]
    
    Erros handled:
    [X] ERR_NOSUCHNICK (401)
    [X] ERR_NOSUCHCHANNEL (403)
    [X] ERR_NOTREGISTERED (451)
    [X] ERR_NOTONCHANNEL (442) 
    [X] ERR_NEEDMOREPARAMS (461)
    [X] ERR_BADCHANMASK (476)
    [X] ERR_CHANOPRIVSNEEDED (482)
    
*/
void Commands::handleKick(Client &client, const std::vector<std::string> &commandParams,
                            std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients)
{
    //* STEP 1 : Build replyTarget
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    //* STEP 2 : Check kicker's registration
    // 451 ERR_NOTREGISTERED
    if (!client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOTREGISTERED, replyTarget, MSG_NOTREGISTERED));
        return;
    }
    
    //* STEP 3 : Check parameters count (nickname, channel name).
    // 461 ERR_NEEDMOREPARAMS
    if (commandParams.size() < 2 || commandParams[0].empty() || commandParams[1].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_NEEDMOREPARAMS, replyTarget, "KICK", MSG_NEEDMOREPARAMS));
        return;
    }

    //& Set varriables for nickname and channelName from commandParams
    const std::string &channelName = commandParams[0];
    const std::string &nickname = commandParams[1];
    
    //* STEP 4 : Set raison parameter (optional)
    // Default raison is the nickname of the client issuing the KICK command
    std::string raison  = (commandParams.size() >= 3 && !commandParams[2].empty())
                             ? commandParams[2]
                             : client.getNickname();

    //* STEP 5 : Check channel mask format
    // 476 ERR_BADCHANMASK
    if (!Channel::isValidChannelName(channelName))
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_BADCHANMASK, replyTarget, channelName, MSG_BADCHANMASK));
        return;
    }
    
    //* STEP 6 : Check channel existence
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

    //* STEP 7 : Check if kicker is on channel
    // 442 ERR_NOTONCHANNEL
    // "<client> <channel> :You're not on that channel"

    //& Set pointer to the target channel
    Channel *channel = channelIt->second;

    //& Check if the client is a member of the channel
    std::string param = channelName;
    if (!channel->isMember(client.getFdSocket()))
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOTONCHANNEL, replyTarget, param, MSG_NOTONCHANNEL));
        return;
    }

    //* STEP 8 : Check operator privileges of the kicker
    // 482 ERR_CHANOPRIVSNEEDED
    // "<client> <channel> :You're not channel operator"
    if (!channel->isOperator(client.getFdSocket()))
    {
        client.appendToWriteBuffer(Commands::buildReply(ERR_CHANOPRIVSNEEDED, replyTarget, param, MSG_CHANOPRIVSNEEDED));
        return;
    }

    //* STEP 9 : Check if the target client exists and is registered (has a nickname, username, and realname).
    // 401 ERR_NOSUCHNICK
    // "<client> <nickname> :No such nick/channel"
    
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

    if (!targetClient)
    {
        std::string param = nickname;
        client.appendToWriteBuffer(Commands::buildReply(ERR_NOSUCHNICK, replyTarget, param, MSG_NOSUCHNICK));
        return;
    }
    
    //* STEP 10 : Check target client is on channel
    // 441 ERR_USERNOTINCHANNEL
    // "<client> <nick> <channel> :They aren't on that channel"
    if (!channel->isMember(targetClient->getFdSocket()))
    {
        std::string param = targetClient->getNickname() + " " + channelName;
        client.appendToWriteBuffer(Commands::buildReply(ERR_USERNOTINCHANNEL, replyTarget, param, MSG_USERNOTINCHANNEL));
        return;
    }

    //* STEP 11 : Execute KICK
    
    //& Build the KICK message to be broadcasted
    // Format: ":<prefix> KICK <channel> <user> :<comment>\r\n"
    std::string prefix = client.getNickname() + "!" + client.getUsername() + "@" + HOSTNAME;
    std::string kickMessage = ":" + prefix + " KICK " + channelName + " " + targetClient->getNickname() + " :" + raison + "\r\n";
    
    //& Broadcast the KICK message to all members of the channel, excluding the kicker
    channel->broadcast(kickMessage, -1);

    //& Remove the target client from the channel's member list
    channel->removeMember(targetClient->getFdSocket());
}