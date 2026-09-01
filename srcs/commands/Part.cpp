#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

static void partSingleChannel(Client &client, const std::string &channelName, const std::string &partMessage,
                                std::map<std::string, Channel *> &channels, const std::string &replyTarget)
{
    if (!Channel::isValidChannelName(channelName))
    {
        client.appendToWriteBuffer(Commands::buildReply("476", replyTarget, channelName, "Bad Channel Mask"));
        return;
    }

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

    std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channelName;
    if (!partMessage.empty())
        message += " :" + partMessage;
    message += "\r\n";
    channel->broadcast(message);

    channel->removeMember(client.getFdSocket());

    if (channel->getMemberCount() == 0)
    {
        delete channel;
        channels.erase(it);
    }
}

void Commands::handlePart(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels)
{
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    if (!client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply("451", replyTarget, "You have not registered"));
        return;
    }

    if (commandParams.empty() || commandParams[0].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("461", replyTarget, "PART", "Not enough parameters"));
        return;
    }

    std::vector<std::string> channelNames = Commands::splitByComma(commandParams[0]);
    std::string partMessage = (commandParams.size() > 1) ? commandParams[1] : "";

    for (size_t index = 0; index < channelNames.size(); ++index)
        partSingleChannel(client, channelNames[index], partMessage, channels, replyTarget);
}
