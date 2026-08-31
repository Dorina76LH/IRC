#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"

static void handleChannelMode(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels)
{
	std::string target = client.getNickname().empty() ? "*" : client.getNickname();
	std::string channelName = commandParams[0];
	std::map<std::string, Channel *>::iterator it = channels.find(channelName);

	if (it == channels.end())
	{
		client.appendToWriteBuffer(Commands::buildReply("403", target, channelName, "No such channel"));
		return;
	}

	Channel *channel = it->second;

	if (commandParams.size() == 1)
	{
		std::string currentModes = channel->getModes();
		client.appendToWriteBuffer(Commands::buildReply("324", target, channelName, currentModes));
		return;
	}

	if (!channel->isOperator(client.getFdSocket()))
	{
		client.appendToWriteBuffer(Commands::buildReply("482", target, channelName, "You're not channel operator"));
		return;
	}

	std::string modeFlags = commandParams[1];
	bool adding = true;
	size_t paramIndex = 2;

	for (size_t i = 0; i < modeFlags.size(); ++i)
	{
		char flag = modeFlags[i];
		if (flag == '+')
			adding = true;
		else if (flag == '-')
			adding = false;
		else if (flag == 'i')
			channel->setInviteOnly(adding);
		else if (flag == 't')
			channel->setTopicRestricted(adding);
		else if (flag == 'k')
		{
			if (adding && paramIndex < commandParams.size())
				channel->setKey(commandParams[paramIndex++]);
			else if (!adding)
				channel->removeKey();
		}
		else if (flag == 'l')
		{
			if (adding && paramIndex < commandParams.size())
			{
				int limit = std::atoi(commandParams[paramIndex++].c_str());
				if (limit > 0)
					channel->setUserLimit(limit);
			}
			else if (!adding)
				channel->removeUserLimit();
		}
		else
		{
			std::string unknownChar(1, flag);
			client.appendToWriteBuffer(Commands::buildReply("472", target, unknownChar, "is unknown mode char to me"));
		}
	}
}

static void handleUserMode(Client &client, const std::vector<std::string> &commandParams)
{
	std::string target = client.getNickname().empty() ? "*" : client.getNickname();
	std::string targetName = commandParams[0];

	if (targetName != client.getNickname())
	{
		client.appendToWriteBuffer(Commands::buildReply("502", target, "Cannot change mode for other users"));  
		return;
	}

	if (commandParams.size() == 1)
	{
		std::string umodes = client.isInvisible() ? "+i" : "+";
		client.appendToWriteBuffer(Commands::buildReply("221", target, umodes, ""));
		return;
	}

	std::string modeFlags = commandParams[1];
	bool adding = true;

	for (size_t i = 0; i < modeFlags.size(); ++i)
	{
		char flag = modeFlags[i];
		if (flag == '+')
			adding = true;
		else if (flag == '-')
			adding = false;
		else if (flag == 'i')
			client.setInvisible(adding);
		else
		{
			std::string unknownChar(1, flag);
			client.appendToWriteBuffer(Commands::buildReply("472", target, unknownChar, "is unknown mode char to me"));
		}
	}
}


// Méthode de la classe Commands
void Commands::handleMode(Client &client, const std::vector<std::string> &commandParams, std::map<std::string, Channel *> &channels)
{
	std::string target = client.getNickname().empty() ? "*" : client.getNickname();

	if (commandParams.empty())
	{
		client.appendToWriteBuffer(Commands::buildReply("461", target, "MODE", "Not enough parameters"));
		return;
	}

	std::string targetName = commandParams[0];

	if (!targetName.empty() && targetName[0] == '#')
		handleChannelMode(client, commandParams, channels);
	else
		handleUserMode(client, commandParams);
}