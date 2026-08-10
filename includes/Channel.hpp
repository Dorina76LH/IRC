#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

class Client;

// Un salon IRC (RFC 2811). Chaque Channel gere ses propres membres,
// operateurs et modes, independamment des autres channels.
class Channel
{
public:
    Channel();
    Channel(const std::string& name);
    ~Channel();

    const std::string& getName() const;

    // --- Membres ---
    void                         addMember(Client* client);
    void                         removeMember(Client* client);
    bool                         isMember(Client* client) const;
    const std::vector<Client*>&  getMembers() const;
    size_t                       memberCount() const;

    // --- Operateurs (mode +o) ---
    void                addOperator(Client* client);
    void                removeOperator(Client* client);
    bool                isOperator(Client* client) const;

    // --- Invitations (utile si mode +i) ---
    void                addInvite(Client* client);
    void                removeInvite(Client* client);
    bool                isInvited(Client* client) const;

    // --- Topic (commande TOPIC) ---
    const std::string&  getTopic() const;
    void                setTopic(const std::string& topic);

    // --- Modes (RFC 2811 / sujet ft_irc : i t k o l) ---
    bool                isInviteOnly() const;          // +i
    void                setInviteOnly(bool val);
    bool                isTopicRestricted() const;      // +t : TOPIC reserve aux operateurs
    void                setTopicRestricted(bool val);
    bool                hasKey() const;                  // +k
    const std::string&  getKey() const;
    void                setKey(const std::string& key);
    void                removeKey();
    bool                hasUserLimit() const;            // +l
    size_t              getUserLimit() const;
    void                setUserLimit(size_t limit);
    void                removeUserLimit();

    // --- Diffusion ---
    // Envoie msg (deja formate au format IRC) a tous les membres,
    // sauf exclude (utile pour ne pas renvoyer un message a son emetteur).
    void                broadcast(const std::string& msg, Client* exclude = 0);

private:
    std::string          _name;
    std::string          _topic;
    std::vector<Client*> _members;
    std::vector<Client*> _operators;
    std::vector<Client*> _invited;

    bool        _inviteOnly;
    bool        _topicRestricted;
    bool        _hasKey;
    std::string _key;
    bool        _hasUserLimit;
    size_t      _userLimit;
};

#endif
