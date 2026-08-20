#include "../../includes/Commands.hpp"

void Commands::handlePass(Client &client, const std::vector<std::string> &commandParams, const std::string &serverPassword)
{
    // Before registration, an unregistered client has no nickname yet:
    // RFC 1459 uses '*' as the target of replies sent to such a client.
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    if (client.isRegistered())
    {
        client.appendToWriteBuffer(Commands::buildReply("462", replyTarget, "You may not reregister"));
        return;
    }

    if (commandParams.empty() || commandParams[0].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("461", replyTarget, "PASS", "Not enough parameters"));
        return;
    }

    if (commandParams[0] != serverPassword)
    {
        client.appendToWriteBuffer(Commands::buildReply("464", replyTarget, "Password incorrect"));
        return;
    }

    client.setAuthenticated(true);
}
