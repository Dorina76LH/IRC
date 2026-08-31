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

// RFC 1459, 1.3 : <channel> ::= ('#' | '&') <chstring>
// <chstring> excludes spaces, commas (used as a list separator) and the
// control character 0x07 (BEL, used as a message terminator by some clients).
static bool isValidChannelName(const std::string &channelName)
{
    if (channelName.empty())
        return (false);
    if (channelName[0] != '#' && channelName[0] != '&')
        return (false);

    for (size_t index = 1; index < channelName.size(); ++index)
    {
        char currentCharacter = channelName[index];
        if (currentCharacter == ' ' || currentCharacter == ',' || currentCharacter == '\x07')
            return (false);
    }
    return (true);
}

static std::string buildNamesReply(const Channel &channel)
{
    std::string namesList;
    const std::map<int, Client *> &members = channel.getMembers();

    for (std::map<int, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if (!namesList.empty())
            namesList += " ";
        if (channel.isOperator(it->first))
            namesList += "@";
        namesList += it->second->getNickname();
    }
    return (namesList);
}

static void joinSingleChannel(Client &client, const std::string &channelName, const std::string &key,
                                std::map<std::string, Channel *> &channels, const std::string &replyTarget)
{
    if (!isValidChannelName(channelName))
    {
        client.appendToWriteBuffer(Commands::buildReply("476", replyTarget, channelName, "Bad Channel Mask"));
        return;
    }

    std::map<std::string, Channel *>::iterator it = channels.find(channelName);
    bool isNewChannel = (it == channels.end());
    Channel *channel = isNewChannel ? new Channel(channelName) : it->second;

    if (isNewChannel)
        channels[channelName] = channel;

    if (channel->isMember(client.getFdSocket()))
        return;

    if (!isNewChannel)
    {
        if (channel->isInviteOnly() && !channel->isInvited(client.getFdSocket()))
        {
            client.appendToWriteBuffer(Commands::buildReply("473", replyTarget, channelName, "Cannot join channel (+i)"));
            return;
        }
        if (channel->hasKey() && channel->getKey() != key)
        {
            client.appendToWriteBuffer(Commands::buildReply("475", replyTarget, channelName, "Cannot join channel (+k)"));
            return;
        }
        if (channel->hasUserLimit() && channel->getMemberCount() >= static_cast<size_t>(channel->getUserLimit()))
        {
            client.appendToWriteBuffer(Commands::buildReply("471", replyTarget, channelName, "Cannot join channel (+l)"));
            return;
        }
    }

    channel->addMember(&client);
    channel->removeInvited(client.getFdSocket());
    if (isNewChannel)
        channel->addOperator(client.getFdSocket());

    std::string joinMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n";
    channel->broadcast(joinMessage);

    if (!channel->getTopic().empty())
        client.appendToWriteBuffer(Commands::buildReply("332", replyTarget, channelName, channel->getTopic()));

    client.appendToWriteBuffer(Commands::buildReply("353", replyTarget, "= " + channelName, buildNamesReply(*channel)));
    client.appendToWriteBuffer(Commands::buildReply("366", replyTarget, channelName, "End of /NAMES list"));
}

void Commands::handleJoin(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels)
{
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    if (!client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply("451", replyTarget, "You have not registered"));
        return;
    }

    if (commandParams.empty() || commandParams[0].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("461", replyTarget, "JOIN", "Not enough parameters"));
        return;
    }

    std::vector<std::string> channelNames = splitByComma(commandParams[0]);
    std::vector<std::string> keys = (commandParams.size() > 1) ? splitByComma(commandParams[1]) : std::vector<std::string>();

    for (size_t index = 0; index < channelNames.size(); ++index)
    {
        std::string key = (index < keys.size()) ? keys[index] : "";
        joinSingleChannel(client, channelNames[index], key, channels, replyTarget);
    }
}
