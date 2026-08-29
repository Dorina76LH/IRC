#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

void Commands::handleTopic(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels)
{
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    if (!client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply("451", replyTarget, "You have not registered"));
        return;
    }

    if (commandParams.empty() || commandParams[0].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("461", replyTarget, "TOPIC", "Not enough parameters"));
        return;
    }

    const std::string &channelName = commandParams[0];
    std::map<std::string, Channel *>::iterator it = channels.find(channelName);
    if (it == channels.end())
    {
        client.appendToWriteBuffer(Commands::buildReply("403", replyTarget, channelName, "No such channel"));
        return;
    }

    Channel *channel = it->second;

    if (!channel->isMember(client.getFdSocket()))
    {
        client.appendToWriteBuffer(Commands::buildReply("442", replyTarget, channelName, "You're not on that channel"));
        return;
    }

    // Pas de deuxieme parametre : le client veut juste consulter le topic actuel.
    if (commandParams.size() < 2)
    {
        if (channel->getTopic().empty())
            client.appendToWriteBuffer(Commands::buildReply("331", replyTarget, channelName, "No topic is set"));
        else
            client.appendToWriteBuffer(Commands::buildReply("332", replyTarget, channelName, channel->getTopic()));
        return;
    }

    if (channel->isTopicRestricted() && !channel->isOperator(client.getFdSocket()))
    {
        client.appendToWriteBuffer(Commands::buildReply("482", replyTarget, channelName, "You're not channel operator"));
        return;
    }

    channel->setTopic(commandParams[1]);

    std::string topicMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost TOPIC " + channelName + " :" + commandParams[1] + "\r\n";
    channel->broadcast(topicMessage);
}
