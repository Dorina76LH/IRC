#include "../../includes/Commands.hpp"

void Commands::handleUser(Client &client, const std::vector<std::string> &commandParams)
{
    // Before registration, an unregistered client has no nickname yet:
    // RFC 1459 uses '*' as the target of replies sent to such a client.
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    if (client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply("462", replyTarget, "You may not reregister"));
        return;
    }

    if (!client.isAuthenticated())
    {
        client.appendToWriteBuffer(Commands::buildReply("451", replyTarget, "You have not registered"));
        return;
    }

    // <username> <hostname> <servername> <realname>
    if (commandParams.size() < 4 || commandParams[0].empty() || commandParams[3].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("461", replyTarget, "USER", "Not enough parameters"));
        return;
    }

    // hostname (commandParams[1]) and servername (commandParams[2]) are client-supplied
    // and untrustworthy: the server already knows the real peer address and its own name,
    // so they are accepted but intentionally ignored here.
    client.setUsername(commandParams[0]);
    client.setRealname(commandParams[3]);
}
