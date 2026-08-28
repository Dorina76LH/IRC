#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

void Commands::handleInvite(Client &client, const std::vector<std::string> &commandParams,
                            std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients)
{
    //* STEP 1 : Build the reply target based on whether the client has a nickname or not.
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    //* STEP 2 : Check if the command has enough parameters (nickname and channel name).
    // 461 ERR_NEEDMOREPARAMS is sent if there are not enough parameters.
    if (commandParams.size() < 2 || commandParams[0].empty() || commandParams[1].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("461", replyTarget, "INVITE", "Not enough parameters"));
        return;
    }
    
    //* STEP 3 : Find the channel in the server's channel map.
    
    // Define nicnkame and channelName from commandParams, define an iterator to find the channel in the channels map.
    const std::string &nickname = commandParams[0];
    const std::string &channelName = commandParams[1];
    std::map<std::string, Channel *>::iterator channelIt = channels.find(channelName);

    // Look for the channel in the channels map
    // 403 ERR_NOSUCHCHANNEL is sent if the channel does not exist.
    if (channelIt == channels.end())
    {
        client.appendToWriteBuffer(Commands::buildReply("403", replyTarget, channelName, "No such channel"));
        return;
    }

    //* STEP 4 : Check if the client is a member of the channel and if they are an operator.


}



