/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asnothar <asnothar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 19:08:52 by doberes           #+#    #+#             */
/*   Updated: 2026/08/30 15:33:05 by asnothar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* -- Includes -- *//
#include "../includes/Client.hpp"

//* -- Colors -- *//
#define COLOR_RESET "\033[0m"
#define COLOR_CYAN  "\033[1;36m"

//* -- Canonical methods -- *//

//? Parameterized constructor
Client::Client(int fdSocket) :  _fdSocket(fdSocket),
                                _nickname(""),
                                _username(""),
                                _realname(""),
                                _isRegistered(false),
                                _isAuthenticated(false),
                                _readBuffer(""),
                                _writeBuffer("") 
{    }

//? Destructor
Client::~Client() {}

//* -- Getters -- *//

//? Get the socket file descriptor
int Client::getFdSocket() const
{
    return (this->_fdSocket);
}

//? Get the client's nickname
const std::string & Client::getNickname() const
{
    return (this->_nickname);
}

//? Get the client's username
const std::string & Client::getUsername() const
{
    return (this->_username);
}

//? Get the client's real name
const std::string & Client::getRealname() const
{
    return (this->_realname);
}

//? Check if the client is registered
bool Client::isRegistered() const
{
    return (this->_isRegistered);
}

//? Check if the client is authenticated
bool Client::isAuthenticated() const
{
    return (this->_isAuthenticated);
}

//? Check if the client is invisible
bool Client::isInvisible() const
{
	return this->_isInvisible;
}

//? Get the read buffer
const std::string & Client::getReadBuffer() const
{
    return (this->_readBuffer);
}

//? Get the write buffer
//& It does NOT automatically add a trailing '\r\n'.
//&
//& Why? Because Client has no way to know whether the buffer's current
//& trailing bytes belong to a fully sent message or to a message still
//& partially sent (send() can transmit only part of the buffer at once).
//& Automatically appending '\r\n' here could insert it in the middle of
//& an unsent fragment, corrupting the IRC protocol stream.
//&
//& The caller (Server / command handlers) is responsible for building
//& a complete, correctly terminated IRC message ("...\r\n") BEFORE
//& calling this method.
const std::string & Client::getWriteBuffer() const
{
    return (this->_writeBuffer);
}

//* -- Setters -- *//
//DEBUG: quelles sont les verifications à faire pour les setters ? (ex: nickname ne doit pas être vide, username ne doit pas contenir d'espaces, etc.)
//DEBUG: les verifications se font vis le serveur ou l'authentification ? (ex: nickname ne doit pas être vide, username ne doit pas contenir d'espaces, etc.)les verifications se font vis le serveur ou l'authentification ? (ex: nickname ne doit pas être vide, username ne doit pas contenir d'espaces, etc.)
//DEBUG: selon la norme RFC 2812, le nickname ne doit pas être vide et ne doit pas contenir d'espaces. Le username ne doit pas contenir d'espaces non plus. Le realname peut contenir des espaces, mais ne doit pas être vide. L'authentification se fait via le serveur, donc les vérifications doivent se faire côté serveur.

//? Set the client's nickname
void Client::setNickname(const std::string &nickname)
{
    this->_nickname = nickname;
}

//? Set the client's username
void Client::setUsername(const std::string &username)
{
    this->_username = username;
}

//? Set the client's real name
void Client::setRealname(const std::string &realname)
{
    this->_realname = realname;
}

//? Set the client's registration status
void Client::setRegistered(bool isRegistered)
{
    this->_isRegistered = isRegistered;
}

//? Set the client's authentication status
void Client::setAuthenticated(bool isAuthenticated)
{
    this->_isAuthenticated = isAuthenticated;
}

//? Set the client's invisible status
void Client::setInvisible(bool status)
{
	this->_isInvisible = status;
}


//* -- Read buffer management -> client to server -- *//
// Data coming from the client (read from the socket)

//? Append data to the read buffer
void Client::appendToReadBuffer(const std::string &data)
{
    this->_readBuffer += data;
}

//? Check if the read buffer contains a complete line (terminated by '\n')
// Note: the read buffer can contain multiple lines at once.
// IRC protocol messages are terminated by '\r\n', but we check for '\n' only.
// Why? Because tools like netcat or telnet only send '\n' when you press
// enter, and we want to be able to test our server with these tools while
// still respecting the IRC protocol (extractLine() strips the trailing '\r'
// if present).
bool Client::hasCompleteLine() const
{
    //& Check if the read buffer contains a complete line (terminated by '\n')
    size_t pos = this->_readBuffer.find('\n');

    //& hasCompleteLine is true if pos is not npos (not found), false otherwise
    bool lineFound = (pos != std::string::npos);

    return (lineFound);
}

//? Extract a complete line from the read buffer (terminated by '\n')
std::string Client::extractLine()
{
    //& Find the position of the first '\n' in the read buffer
    size_t pos = this->_readBuffer.find('\n');
    if (pos == std::string::npos)
    {
        return ("");
    }
    
    //& Extract the line from the read buffer (without the '\n')
    std::string line = this->_readBuffer.substr(0, pos);

    //& Remove the extracted line from the read buffer (including the '\n')
    this->_readBuffer.erase(0, (pos + 1));

    //& Strip the trailing '\r' if present (IRC protocol messages are terminated by '\r\n')
    if (!line.empty() && line[line.size() - 1] == '\r')
    {
        //& The trailing '\r' is at position length - 1
        line.erase(line.size() - 1 , 1);
    }

    return (line);
}

//* -- Write buffer management -> server to client -- *//

//& Append data to the write buffer
void Client::appendToWriteBuffer(const std::string &data)
{
    this->_writeBuffer += data;
}

//& Check if the write buffer has data to send to the client
bool Client::hasDataToSend() const
{
    bool hasData = !this->_writeBuffer.empty();
    return (hasData);
}

//& Clear the sent data from the write buffer after sending it to the client
void Client::clearSentData(size_t bytesSent)
{
    if (bytesSent > this->_writeBuffer.size())
    {
        //& If bytesSent is greater than the size of the write buffer, clear the entire buffer
        this->_writeBuffer.clear();
    }
    else
    {
        //& Otherwise, erase the sent data from the write buffer
        this->_writeBuffer.erase(0, bytesSent);
    }
}

//* -- Non-member functions -- *//

std::ostream &operator<<(std::ostream &os, const Client &client)
{
    os << "┌" << std::string(45, '-') << "\n"
       << "│ " << COLOR_CYAN << std::left << std::setw(15) << "Client" << "(fd " << client.getFdSocket() << ") " << COLOR_RESET << "\n"
       << "│ " << std::left << std::setw(15) << "nickname" << client.getNickname() << "\n"
       << "│ " << std::left << std::setw(15) << "username" << client.getUsername() << "\n"
       << "│ " << std::left << std::setw(15) << "realname" << client.getRealname() << "\n"
       << "│ " << std::left << std::setw(15) << "registered" << (client.isRegistered() ? "true" : "false") << "\n"
       << "│ " << std::left << std::setw(15) << "authenticated" << (client.isAuthenticated() ? "true" : "false") << "\n"
       << "│ " << std::left << std::setw(15) << "readBuffer" << (client.getReadBuffer().empty() ? "empty" : client.getReadBuffer()) << "\n"
       << "│ " << std::left << std::setw(15) << "writeBuffer" << (client.getWriteBuffer().empty() ? "empty" : client.getWriteBuffer()) << "\n"
       << "└" << std::string(45, '-') << std::endl;
    
    return (os);
}
