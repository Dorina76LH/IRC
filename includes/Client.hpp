#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client
{
public:
    Client();
    Client(int fd);
    ~Client();

    // --- Identité ---
    int                 getFd() const;
    const std::string&  getNickname() const;
    void                setNickname(const std::string& nick);
    const std::string&  getUsername() const;
    void                setUsername(const std::string& user);
    const std::string&  getRealname() const;
    void                setRealname(const std::string& realname);
    const std::string&  getHost() const;   // IP du client, recuperee via getpeername() a l'accept()
    void                setHost(const std::string& host);
    std::string         getPrefix() const; // "nick!user@host" pour les messages IRC (RFC 2812)

    // --- Etat de connexion ---
    bool                isAuthenticated() const;   // mot de passe correct fourni
    void                setAuthenticated(bool val);
    bool                isRegistered() const;       // NICK + USER + PASS ok -> peut rejoindre channels
    void                setRegistered(bool val);

    // --- Buffers reseau (recv/send non bloquants) ---
    void                appendToRecvBuffer(const std::string& data);
    bool                hasCompleteCommand() const;   // cherche un \r\n dans le buffer
    std::string         popNextCommand();             // extrait et retire une commande complete

    void                queueMessage(const std::string& msg); // ajoute au buffer d'envoi
    const std::string&  getSendBuffer() const;
    void                consumeSendBuffer(size_t n);   // apres un send() partiel

private:
    int         _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _host;
    bool        _authenticated;
    bool        _registered;
    std::string _recvBuffer;
    std::string _sendBuffer;
};

#endif
