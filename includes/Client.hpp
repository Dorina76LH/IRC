/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 18:49:08 by doberes           #+#    #+#             */
/*   Updated: 2026/08/16 16:00:43 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

//* -- Includes -- *//
#include <string>       // std::string
#include <iostream>     // std::ostream
#include <iomanip>      // std::setw, std::left


//* -- Class -- *//

/*
    PRIVATE CANONICAL FORM - DISABLING COPY AND ASSIGNMENT

    According to the C++98 standard, the best practice to prevent an object 
    from being created without parameter, copied or assigned is to declare
    the default constructor, the copy constructor and the assignment operator
    in the 'private' scope.
    
    CRITICAL SECURITY NOTE:
    To guarantee total safety, these functions MUST NOT be implemented in the
    .cpp file. Leaving them unimplemented ensures that any accidental internal
    or friend calls will trigger a linker error, while external calls will fail
    at compile time.

    RETURN BY VALUE VS RETURN BY CONST REFERENCE

    | Type | Copy cost | Best practice |
    |---|---|---|
    | `int`, `bool`, `char`, `float`, pointers | Trivial (a few bytes) | Return / pass **by value** |
    | `std::string`, `std::vector`, `std::map`, your own complex classes | Potentially expensive (heap allocation) | Return / pass **by const reference** |

    Primitive types are cheap to copy — a reference wouldn't be faster, just extra
    complexity. Types that manage dynamically allocated memory (like `std::string`)
    are potentially expensive to copy, since copying means allocating new memory
    and duplicating the underlying data. Returning a `const &` gives direct
    read-only access to the existing data instead, avoiding that cost while still
    preventing the caller from modifying the object's internal state.
*/
class Client
{
    /*
        WHAT IS A CLIENT?
        A client is a user connected to the IRC server. Each client has a unique socket
        file descriptor, a nickname, a username, and a real name. The client can be
        registered and authenticated. The client has a read buffer and a write buffer
        for incoming and outgoing data

        So, this class is responsible for managing the state and information of a single
        client connected to the IRC server.
        Responsibilities include:
        - Storing the client's socket file descriptor, nickname, username, and real name.
        - Tracking whether the client is registered and authenticated.
        - Managing the read and write buffers for incoming and outgoing data.
        - Providing getters and setters for the client's information and state.
        - Providing a way to output the client's information for debugging or logging purposes.
    */
    public:

        //& Canonical methods
        Client(int fdSocket);
        ~Client();
        
        //& Client information getters
        int                 getFdSocket() const;
        const std::string & getNickname() const;
        const std::string & getUsername() const;
        const std::string & getRealname() const;
        bool                isRegistered() const;
        bool                isAuthenticated() const;
        const std::string & getReadBuffer() const;
        const std::string & getWriteBuffer() const;
        
        //& Client information setters
        void setNickname(const std::string &nickname);
        void setUsername(const std::string &username);
        void setRealname(const std::string &realname);
        void setRegistered(bool isRegistered);
        void setAuthenticated(bool isAuthenticated);
        
        //& Buffer management
        //TODO : Implement methods to manage the read and write buffers, such as adding data to the buffers, clearing them, and checking their sizes.
        
        //& Read buffer management - data coming from the client (read from the socket)
        void        appendToReadBuffer(const std::string &data);
        bool        hasCompleteLine() const;
        std::string extractLine();

        //& Write buffer management - data to be sent to the client (write to the socket)
        // void    appendToWriteBuffer(const std::string &message);
        // bool    hasDataToSend() const;
        // void    clearSentData(size_t bytesSent);
        
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
        
        //& Private canonical methods - not needed for this class
        Client();
        Client(const Client &other);
        Client &operator=(const Client &other);
        
        //& Buffers for incoming and outgoing data
        std::string _readBuffer;        // incoming data buffer, read from socket
        std::string _writeBuffer;       // outgoing data buffer, to be sent to socket

        
};

//* -- Non-member functions -- *//

std::ostream &operator<<(std::ostream &os, const Client &client);

#endif