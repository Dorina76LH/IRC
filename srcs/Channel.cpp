#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"

//Canonical methods

Channel::Channel(const std::string &name)
: _name(name), _topic(""), _inviteOnly(false), _topicRestricted(false), _key(""), _userLimit(0)
{
}

Channel::~Channel()
{
}

// Validation helpers
bool Channel::isValidChannelName(const std::string &channelName)
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

//Channel information

const std::string & Channel::getName() const
{
    return (_name);
}

const std::string & Channel::getTopic() const
{
    return (_topic);
}

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

//Members management

void Channel::addMember(Client *client)
{
    _members[client->getFdSocket()] = client; // insere ou remplace
}

void Channel::removeMember(int fdSocket)
{
    _members.erase(fdSocket); // supprime, ne fait rien si absent
    _operators.erase(fdSocket);  // un membre qui part perd aussi son statut operateur
    _invited.erase(fdSocket);  // et son statut d'invite, pour rester coherent si jamais reinvite plus tard
}

bool Channel::isMember(int fdSocket) const
{
    return (_members.find(fdSocket) != _members.end()); // la clé existe ? 
}

size_t Channel::getMemberCount() const
{
    return (_members.size());
}

const std::map<int, Client *> & Channel::getMembers() const
{
    return (_members);
}

//Operators management

void Channel::addOperator(int fdSocket)
{
    _operators[fdSocket] = true;
}

void Channel::removeOperator(int fdSocket)
{
    _operators.erase(fdSocket);
}

bool Channel::isOperator(int fdSocket) const
{
    return (_operators.find(fdSocket) != _operators.end());
}

//Invitation management

void Channel::addInvited(int fdSocket)
{
    _invited[fdSocket] = true;
}

void Channel::removeInvited(int fdSocket)
{
    _invited.erase(fdSocket);
}

bool Channel::isInvited(int fdSocket) const
{
    return (_invited.find(fdSocket) != _invited.end());
}

//Mode i

bool Channel::isInviteOnly() const
{
    return (_inviteOnly);
}

void Channel::setInviteOnly(bool isInviteOnly)
{
    _inviteOnly = isInviteOnly;
}

//Mode t

bool Channel::isTopicRestricted() const
{
    return (_topicRestricted);
}

void Channel::setTopicRestricted(bool isTopicRestricted)
{
    _topicRestricted = isTopicRestricted;
}

//Mode k

bool Channel::hasKey() const
{
    return (!_key.empty());
}

const std::string & Channel::getKey() const
{
    return (_key);
}

void Channel::setKey(const std::string &key)
{
    _key = key;
}

void Channel::removeKey()
{
    _key.clear();
}

//Mode l

bool Channel::hasUserLimit() const
{
    return (_userLimit > 0);
}

int Channel::getUserLimit() const
{
    return (_userLimit);
}

void Channel::setUserLimit(int userLimit)
{
    _userLimit = userLimit;
}

void Channel::removeUserLimit()
{
    _userLimit = 0;
}

//Broadcasting - diffusion de message

void Channel::broadcast(const std::string &message, int excludeFdSocket) const
{
    std::map<int, Client *>::const_iterator it;

    for (it = _members.begin(); it != _members.end(); ++it)
    {
        if (it->first == excludeFdSocket)
            continue;
        it->second->appendToWriteBuffer(message);
    }
}

std::string Channel::getModes() const
{
	std::string modes = "+";
	std::string params = "";

	if (this->_inviteOnly)
		modes += "i";
	if (this->_topicRestricted)
		modes += "t";
	if (!this->_key.empty())
	{
		modes += "k";
		params += " " + this->_key;
	}
	if (this->_userLimit > 0)
	{
		modes += "l";
		std::stringstream ss;
		ss << this->_userLimit;
		params += " " + ss.str();
	}	
	return modes + params;
}

int Channel::getFdByNickname(const std::string &nickname) const
{
    for (std::map<int, Client *>::const_iterator it = _members.begin(); it != _members.end(); ++it)
    {
        if (it->second && it->second->getNickname() == nickname)
            return it->first;
    }
    return -1;
}
