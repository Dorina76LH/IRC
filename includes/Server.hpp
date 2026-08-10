#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <csignal>

class Client;
class Channel;

// Orchestre tout : le socket d'ecoute, la boucle poll() unique,
// la table des clients connectes et celle des channels.
class Server
{
public:
    Server(int port, const std::string& password);
    ~Server();

    void run(); // boucle principale : poll() -> accept/recv/send

    // --- Arret propre (Ctrl+C) ---
    static void signalHandler(int signum);
    static bool _running; // mis a false par signalHandler, verifie dans run()

private:
    // --- Setup ---
    void setupListenSocket();
    void setupSignalHandlers();

    // --- Boucle poll() (1 SEUL poll() pour tout : ecoute + clients) ---
    void handlePollEvents();
    void acceptNewClient();
    void handleClientReadable(int fd);
    void handleClientWritable(int fd);
    void disconnectClient(int fd);

    // --- Dispatch des commandes ---
    // Recoit une ligne deja extraite du buffer (une commande complete),
    // la parse (commande + params) et appelle le bon handler.
    void processLine(Client& client, const std::string& line);

    // --- Acces partages (utilises par les handlers de commandes) ---
    Client*  getClientByFd(int fd);
    Client*  getClientByNickname(const std::string& nick);
    Channel* getChannel(const std::string& name);
    Channel* getOrCreateChannel(const std::string& name);
    void     removeChannelIfEmpty(const std::string& name);

    bool         checkPassword(const std::string& password) const;
    bool         isNicknameInUse(const std::string& nick) const;

private:
    int                          _listenFd;
    int                          _port;
    std::string                  _password;
    std::string                  _serverName; // utilise dans le prefixe des messages (RFC 2812)

    std::vector<struct pollfd>   _pollFds;
    std::map<int, Client*>       _clients;      // fd -> Client
    std::map<std::string, Channel*> _channels;  // nom -> Channel
};

#endif
