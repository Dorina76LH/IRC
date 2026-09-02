#include "../../includes/Commands.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/Server.hpp"

// liste des flags :
// i : Canal sur invitation uniquement									(MODE <#canal> +i) / (MODE <#canal> -i)
// t : Restriction de la modification du sujet aux seuls opérateurs		(MODE <#canal> +t) / (MODE <#canal> -t)
// k : Définition / retrait du mot de passe du canal					(MODE <#canal> +k <key>) / (MODE <#canal> -k)
// o : Attribution / retrait du statut d'opérateur à un utilisateur		(MODE <#canal> +o <nickname>) / (MODE <#canal> -o <nickname>)
// l : Définition / retrait de la limite maximale d'utilisateurs		(MODE <#canal> +l <maxNumber>) / (MODE <#canal> -l)
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

	std::string appliedModes = "";
	std::string appliedParams = "";
	char currentSign = '\0';

	for (size_t i = 0; i < modeFlags.size(); ++i)
	{
		char flag = modeFlags[i];
		if (flag == '+')
			adding = true;
		else if (flag == '-')
			adding = false;
		else if (flag == 'i' || flag == 't' || flag == 'k' || flag == 'l' || flag == 'o')
		{
			char expectedSign = adding ? '+' : '-';
			bool modeApplied = false;

			if (flag == 'i')
			{
				channel->setInviteOnly(adding);
				modeApplied = true;
			}
			else if (flag == 't')
			{
				channel->setTopicRestricted(adding);
				modeApplied = true;
			}
			else if (flag == 'k')
			{
				if (adding)
				{
					if (paramIndex < commandParams.size())
					{
						std::string key = commandParams[paramIndex++];
						channel->setKey(key);
						appliedParams += " " + key;
						modeApplied = true;
					}
					else
						client.appendToWriteBuffer(Commands::buildReply("461", target, "MODE", "Not enough parameters"));
				}
				else
				{
					channel->removeKey();
					modeApplied = true;
				}
			}
			else if (flag == 'l')
			{
				if (adding)
				{
					if (paramIndex < commandParams.size())
					{
						int limit = atoi(commandParams[paramIndex++].c_str());
						if (limit > 0)
						{
							channel->setUserLimit(limit);
							std::stringstream ss;
							ss << limit;
							appliedParams += " " + ss.str();
							modeApplied = true;
						}
						else
							client.appendToWriteBuffer(Commands::buildReply("461", target, "MODE", "Invalid limit parameter"));

					}
					else
						client.appendToWriteBuffer(Commands::buildReply("461", target, "MODE", "Not enough parameters"));
				}
				else
				{
					channel->removeUserLimit();
					modeApplied = true;
				}
			}
			else if (flag == 'o')
			{
				if (paramIndex < commandParams.size())
				{
					std::string targetNick = commandParams[paramIndex++];
					int targetFd = channel->getFdByNickname(targetNick);

					if (targetFd == -1)
						client.appendToWriteBuffer(Commands::buildReply("441", target, targetNick + " " + channelName, "They aren't on that channel"));
					else
					{
						if (adding)
							channel->addOperator(targetFd);
						else
							channel->removeOperator(targetFd);

						appliedParams += " " + targetNick;
						modeApplied = true;
					}
				}
				else
					client.appendToWriteBuffer(Commands::buildReply("461", target, "MODE", "Not enough parameters"));
			}

			if (modeApplied)
			{
				if (currentSign != expectedSign)
				{
					appliedModes += expectedSign;
					currentSign = expectedSign;
				}
				appliedModes += flag;
			}
		}
		else
		{
			std::string unknownChar(1, flag);
			client.appendToWriteBuffer(Commands::buildReply("472", target, unknownChar, "is unknown mode char to me"));
		}
	}

	if (!appliedModes.empty())
	{
		std::string fullPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost";
		std::string modeMessage = fullPrefix + " MODE " + channelName + " " + appliedModes + appliedParams + "\r\n";
		
		channel->broadcast(modeMessage);
	}
}

// l : Marque l'utilisateur comme invisible		(MODE <#user> +i) / (MODE <#user> -i)
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
		client.appendToWriteBuffer(Commands::buildReply("221", target, umodes));
		return;
	}

	std::string modeFlags = commandParams[1];
	bool adding = true;
	bool modeChanged = false;

	for (size_t i = 0; i < modeFlags.size(); ++i)
	{
		char flag = modeFlags[i];
		if (flag == '+')
			adding = true;
		else if (flag == '-')
			adding = false;
		else if (flag == 'i')
		{
			client.setInvisible(adding);
			modeChanged = true;
		}
		else
		{
			client.appendToWriteBuffer(Commands::buildReply("501", target, "Unknown MODE flag"));
		}
	}

	if (modeChanged)
	{
		std::string fullPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost";
		std::string reply = fullPrefix + " MODE " + client.getNickname() + " :" + modeFlags + "\r\n";
		client.appendToWriteBuffer(reply);
	}
}

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
