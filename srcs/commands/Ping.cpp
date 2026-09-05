#include "../../includes/Commands.hpp"
#include "../../includes/Client.hpp"

void Commands::handlePing(Client &client, const std::vector<std::string> &commandParams)
{
	if (commandParams.empty())
	{
		std::string nickname = client.getNickname().empty() ? "*" : client.getNickname();
		client.appendToWriteBuffer(Commands::buildReply("409", nickname, "PONG", "No origin specified"));
		return;
	}

	std::string token = commandParams[0];
	std::string pongResponse = "PONG :" + token + "\r\n";
	
	client.appendToWriteBuffer(pongResponse);
}
