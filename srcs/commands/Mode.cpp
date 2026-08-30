#include "Commands.hpp"


// MODE <cible> [<modes> [<paramètres>]]
void Commands::handleMode(Client &client, const std::vector<std::string> &commandParams)
{
	std::string target = client.getNickname().empty() ? "*" : client.getNickname();

	// Verification des parametres minimums (RFC 2812 : 461 ERR_NEEDMOREPARAMS)
	if (commandParams.empty())
	{
		client.appendToWriteBuffer(Commands::buildReply("461", target, "MODE", "Not enough parameters"));
		return;
	}

	std::string targetName = commandParams[0];

	// Gestion si le destinataire est un canal (commence par #)
	if (targetName[0] == '#')
	{
		// En attente de voir comment on fait pour la reférence du serveur / du channel
		return;
	}

	// Gestion du MODE Utilisateur (ex: MODE nickname +i)
	if (targetName != client.getNickname())
	{
		// On ne peut pas modifier le mode d'un autre utilisateur (451 / 502 ERR_USERSDONTMATCH)
		client.appendToWriteBuffer(Commands::buildReply("502", target, "Cannot change mode for other users"));	
		return;
	}

	// Si le client demande juste ses modes actuels (ex: MODE nickname)
	if (commandParams.size() == 1)
	{
		// Reponse 221 RPL_UMODEIS
		client.appendToWriteBuffer(Commands::buildReply("221", target, "+i", ""));
		return;
	}

	// Traitement des flags utilisateur (ex: +i, -i)
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
			client.setInvisible(adding); // besoin d'un setter dans Client.hpp
		else
		{
			// Flag inconnu (472 ERR_UNKNOWNMODE)
			std::string unknownChar(1, flag);
			client.appendToWriteBuffer(Commands::buildReply("472", target, unknownChar, "is unknown mode char to me"));
		}
	}
}