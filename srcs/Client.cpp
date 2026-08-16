/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 19:08:52 by doberes           #+#    #+#             */
/*   Updated: 2026/08/16 11:47:36 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* -- Includes -- *//
#include "../includes/Client.hpp"

//* -- Colors -- *//
#define COLOR_RESET "\033[0m"
#define COLOR_CYAN  "\033[1;36m"

//* -- Canonical methods -- *//

//& Parameterized constructor
Client::Client(int fdSocket) :  _fdSocket(fdSocket),
                                _nickname(""),
                                _username(""),
                                _realname(""),
                                _isRegistered(false),
                                _isAuthenticated(false),
                                _readBuffer(""),
                                _writeBuffer("") 
{    }

//& Destructor
Client::~Client() {}

//* -- Getters -- *//

//& Get the socket file descriptor
int Client::getFdSocket() const
{
    return (this->_fdSocket);
}

//& Get the client's nickname
const std::string & Client::getNickname() const
{
    return (this->_nickname);
}

//& Get the client's username
const std::string & Client::getUsername() const
{
    return (this->_username);
}

//& Get the client's real name
const std::string & Client::getRealname() const
{
    return (this->_realname);
}

//& Check if the client is registered
bool Client::isRegistered() const
{
    return (this->_isRegistered);
}

//& Check if the client is authenticated
bool Client::isAuthenticated() const
{
    return (this->_isAuthenticated);
}

//& Get the read buffer
const std::string & Client::getReadBuffer() const
{
    return (this->_readBuffer);
}

const std::string & Client::getWriteBuffer() const
{
    return (this->_writeBuffer);
}

//* -- Setters -- *//
//DEBUG: quelles sont les verifications à faire pour les setters ? (ex: nickname ne doit pas être vide, username ne doit pas contenir d'espaces, etc.)
//DEBUG: les verifications se font vis le serveur ou l'authentification ? (ex: nickname ne doit pas être vide, username ne doit pas contenir d'espaces, etc.)les verifications se font vis le serveur ou l'authentification ? (ex: nickname ne doit pas être vide, username ne doit pas contenir d'espaces, etc.)
//DEBUG: selon la norme RFC 2812, le nickname ne doit pas être vide et ne doit pas contenir d'espaces. Le username ne doit pas contenir d'espaces non plus. Le realname peut contenir des espaces, mais ne doit pas être vide. L'authentification se fait via le serveur, donc les vérifications doivent se faire côté serveur.

//& Set the client's nickname
void Client::setNickname(const std::string &nickname)
{
    this->_nickname = nickname;
}

//& Set the client's username
void Client::setUsername(const std::string &username)
{
    this->_username = username;
}

//& Set the client's real name
void Client::setRealname(const std::string &realname)
{
    this->_realname = realname;
}

//& Set the client's registration status
void Client::setRegistered(bool isRegistered)
{
    this->_isRegistered = isRegistered;
}

//& Set the client's authentication status
void Client::setAuthenticated(bool isAuthenticated)
{
    this->_isAuthenticated = isAuthenticated;
}


//* -- Buffer management methods -- *//
//TODO

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
