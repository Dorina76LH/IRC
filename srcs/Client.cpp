/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 19:08:52 by doberes           #+#    #+#             */
/*   Updated: 2026/08/15 19:15:33 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* -- Includes -- *//
#include "Client.hpp"

//* -- Canonical methods -- *//

//& Default constructor
Client::Client() :  _fdSocket(-1),
                    _nickname(""),
                    _username(""),
                    _realname(""),
                    _isRegistered(false),
                    _isAuthenticated(false),
                    _readBuffer(""),
                    _writeBuffer("")
{    }

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

//& Copy constructor
Client::Client(const Client &other) : _fdSocket(other._fdSocket),
                                        _nickname(other._nickname),
                                        _username(other._username),
                                        _realname(other._realname),
                                        _isRegistered(other._isRegistered),
                                        _isAuthenticated(other._isAuthenticated),
                                        _readBuffer(other._readBuffer),
                                        _writeBuffer(other._writeBuffer)
{    };

//& Copy assignment operator
Client & Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _fdSocket = other._fdSocket;
        _nickname = other._nickname;
        _username = other._username;
        _realname = other._realname;
        _isRegistered = other._isRegistered;
        _isAuthenticated = other._isAuthenticated;
        _readBuffer = other._readBuffer;
        _writeBuffer = other._writeBuffer;
    }
    return *this;
}

//& Destructor
Client::~Client() {}
