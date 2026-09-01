#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include <string> 
#include <map>   // std::map (membres, operateurs, invites)
#include <sstream>
#include "Client.hpp"

class Channel
{
    /*
        WHAT IS A CHANNEL?
        A channel is a named chat room (e.g. "#general") that clients can join.
        Responsibilities include:
        - Storing the channel's name, topic, members and operators.
        - Storing the channel's modes (i, t, k, l) and their associated data (key, limit).
        - Tracking clients invited to join when the channel is invite-only.
        - Broadcasting messages to every member of the channel.
    */
    public:

        // Canonical members
        Channel(const std::string &name);
        ~Channel();

        // Validation helpers
        static bool isValidChannelName(const std::string &name);

        // Channel information getters
        const std::string & getName() const;
        const std::string & getTopic() const;
        void                setTopic(const std::string &topic);

        // Members management - clients currently inside the channel
        void        addMember(Client *client);
        void        removeMember(int fdSocket);
        bool        isMember(int fdSocket) const;
        size_t      getMemberCount() const;
        const std::map<int, Client *> & getMembers() const;

        // Operators management - members with elevated privileges (KICK, INVITE, TOPIC, MODE)
        void    addOperator(int fdSocket);
        void    removeOperator(int fdSocket);
        bool    isOperator(int fdSocket) const;

        // Invitation management - used when the channel is invite-only (mode i)
        void    addInvited(int fdSocket);
        void    removeInvited(int fdSocket);
        bool    isInvited(int fdSocket) const;

        // Mode i - invite-only channel
        bool    isInviteOnly() const;
        void    setInviteOnly(bool isInviteOnly);

        // Mode t - TOPIC command restricted to channel operators
        bool    isTopicRestricted() const;
        void    setTopicRestricted(bool isTopicRestricted);

        // Mode k - channel key (password)
        bool                hasKey() const;
        const std::string & getKey() const;
        void                setKey(const std::string &key);
        void                removeKey();

        // Mode l - user limit
        bool    hasUserLimit() const;
        int     getUserLimit() const;
        void    setUserLimit(int userLimit);
        void    removeUserLimit();

        // Broadcasting - forward a message to every member (optionally skipping one)
        void    broadcast(const std::string &message, int excludeFdSocket = -1) const;

		// Mode getters
		std::string getModes() const;
		int getFdByNickname(const std::string &nickname) const;
		
    private:

        // Channel information
        std::string _name;
        std::string _topic;

        // Channel members and operators, keyed by client socket fd
        std::map<int, Client *> _members;
        std::map<int, bool>     _operators;
        std::map<int, bool>     _invited;

        // Channel modes
        bool        _inviteOnly;       // mode i
        bool        _topicRestricted;  // mode t
        std::string _key;              // mode k, empty = no key
        int         _userLimit;        // mode l, 0 = no limit

        // Private canonical methods - not needed for this class
        Channel();
        Channel(const Channel &other);
        Channel &operator=(const Channel &other);
};

#endif
