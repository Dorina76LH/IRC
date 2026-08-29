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

static bool isChannelTarget(const std::string &target)
{
    return (!target.empty() && (target[0] == '#' || target[0] == '&'));
}

static Client *findClientByNickname(std::map<int, Client *> &clients, const std::string &nickname)
{
    for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second->getNickname() == nickname)
            return (it->second);
    }
    return (NULL);
}

static void sendToChannel(Client &client, const std::string &channelName, const std::string &message,
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
        client.appendToWriteBuffer(Commands::buildReply("404", replyTarget, channelName, "Cannot send to channel"));
        return;
    }

    std::string fullMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + channelName + " :" + message + "\r\n";
    channel->broadcast(fullMessage, client.getFdSocket());
}

static void sendToNickname(Client &client, const std::string &nickname, const std::string &message,
                             std::map<int, Client *> &clients, const std::string &replyTarget)
{
    Client *target = findClientByNickname(clients, nickname);
    if (target == NULL)
    {
        client.appendToWriteBuffer(Commands::buildReply("401", replyTarget, nickname, "No such nick/channel"));
        return;
    }

    std::string fullMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + nickname + " :" + message + "\r\n";
    target->appendToWriteBuffer(fullMessage);
}

void Commands::handlePrivMsg(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels, std::map<int, Client *> &clients)
{
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    if (!client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply("451", replyTarget, "You have not registered"));
        return;
    }

    if (commandParams.empty() || commandParams[0].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("411", replyTarget, "No recipient given (PRIVMSG)"));
        return;
    }

    if (commandParams.size() < 2 || commandParams[1].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("412", replyTarget, "No text to send"));
        return;
    }

    std::vector<std::string> targets = splitByComma(commandParams[0]);
    const std::string &message = commandParams[1];

    for (size_t index = 0; index < targets.size(); ++index)
    {
        if (isChannelTarget(targets[index]))
            sendToChannel(client, targets[index], message, channels, replyTarget);
        else
            sendToNickname(client, targets[index], message, clients, replyTarget);
    }
}
