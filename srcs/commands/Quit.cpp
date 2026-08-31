#include "../../includes/Commands.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Server.hpp"

void Server::disconnectClientByFd(int fd)
{
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == fd)
		{
			disconnectClient(i);
			return;
		}
	}
}

void Server::broadcastToSharedChannels(int clientFd, const std::string &message)
{
	std::set<int> recipients;

	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		Channel *channel = it->second;
		if (channel && channel->isMember(clientFd))
		{
			const std::map<int, Client *> &members = channel->getMembers();
			
			for (std::map<int, Client *>::const_iterator memIt = members.begin(); memIt != members.end(); ++memIt)
			{
				recipients.insert(memIt->first);
			}
		}
	}

	recipients.erase(clientFd);

	for (std::set<int>::iterator it = recipients.begin(); it != recipients.end(); ++it)
	{
		std::map<int, Client*>::iterator clientIt = _clients.find(*it);
		if (clientIt != _clients.end())
			clientIt->second->appendToWriteBuffer(message);
	}
}

void Commands::handleQuit(Client &client, const std::vector<std::string> &commandParams, Server &server)
{
	std::string quitReason = "Client quit";

	if (!commandParams.empty())
	{
		quitReason = commandParams[0];
		for (size_t i = 1; i < commandParams.size(); ++i)
			quitReason += " " + commandParams[i];
	}

	std::string fullPrefix = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost";
	std::string quitMessage = fullPrefix + " QUIT :" + quitReason + "\r\n";
	std::string errorMessage = "ERROR :Closing Link: localhost (" + quitReason + ")\r\n";

	client.appendToWriteBuffer(errorMessage);
	server.broadcastToSharedChannels(client.getFdSocket(), quitMessage);
	server.disconnectClientByFd(client.getFdSocket());
}