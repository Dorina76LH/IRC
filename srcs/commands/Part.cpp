#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

static std::vector<std::string> splitByComma(const std::string &list)
{
    std::vector<std::string> parts;
    size_t start = 0;

    while (start <= list.size())
    {
        size_t comma = list.find(',', start);
        if (comma == std::string::npos)
        {
            parts.push_back(list.substr(start));
            break;
        }
        parts.push_back(list.substr(start, comma - start));
        start = comma + 1;
    }
    return (parts);
}

static void partSingleChannel(Client &client, const std::string &channelName, const std::string &partMessage,
                                std::map<std::string, Channel *> &channels, const std::string &replyTarget)
{
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

    if (commandParams.empty() || commandParams[0].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("461", replyTarget, "PART", "Not enough parameters"));
        return;
    }

    std::vector<std::string> channelNames = splitByComma(commandParams[0]);
    std::string partMessage = (commandParams.size() > 1) ? commandParams[1] : "";

    for (size_t index = 0; index < channelNames.size(); ++index)
        partSingleChannel(client, channelNames[index], partMessage, channels, replyTarget);
}
