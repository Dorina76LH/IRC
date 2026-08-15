/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 18:49:08 by doberes           #+#    #+#             */
/*   Updated: 2026/08/15 19:07:33 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

//* -- Includes -- *//
#include <string>


//* -- Class -- *//

class Client
{
    public:

        //& Canonical methods
        Client();
        Client(int fdSocket);
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client();

        //& Client information getters
        int     getFdSocket() const;
        const   std::string &getNickname() const;
        const   std::string &getUsername() const;
        const   std::string &getRealname() const;
        bool    isRegistered() const;
        bool    isAuthenticated() const;

        //& Client information setters
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setRealname(const std::string &realname);
        void setRegistered(bool isRegistered);
        void setAuthenticated(bool isAuthenticated);
        
        //& Buffer management
    
    private:
    
        //& Socket file descriptor
        int         _fdSocket;
        
        //& Client information
        std::string _nickname;
        std::string _username;
        std::string _realname;

        //& Client state flags
        bool        _isRegistered;      // nickname, username, realname OK
        bool        _isAuthenticated;   // Password OK

        //& Buffers for incoming and outgoing data
        std::string _readBuffer;        // incoming data buffer, read from socket
        std::string _writeBuffer;       // outgoing data buffer, to be sent to socket

        
        
};

#endif