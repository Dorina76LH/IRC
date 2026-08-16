/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_client.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 10:59:58 by doberes           #+#    #+#             */
/*   Updated: 2026/08/16 11:43:53 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* -- Includes -- *//
#include "../includes/Client.hpp"
#include <iostream>
#include <cassert>

// to compile:
// c++ -std=c++98 -Wall -Wextra -Werror tests/test_client.cpp srcs/Client.cpp -o test_client

int main()
{
    
    //& Create a Client object with a dummy socket file descriptor
    Client client(42);
    
    //& Test private canonical methods
    //? Since the private canonical methods (default constructor, copy constructor,
    //? and assignment operator) are declared but not implemented, we cannot directly
    //? test them. However, we can ensure that they are inaccessible by trying to use
    //? them in a way that would cause a compilation error if they were accessible.
    
    // Client client_default;              // do not compile, default constructor is private
    // Client client_copy(client);      // do not compile, copy constructor is private
    // client = client_copy;            // do not compile, assignment operator is private 
    
    //& Test getters
    assert(client.getFdSocket() == 42);
    assert(client.getNickname().empty());
    assert(client.getUsername().empty());
    assert(client.getRealname().empty());
    assert(!client.isRegistered());
    assert(!client.isAuthenticated());
    assert(client.getReadBuffer().empty());
    assert(client.getWriteBuffer().empty());

    //& Test setters
    client.setNickname("test_nick");
    client.setUsername("test_user");
    client.setRealname("Test User");
    client.setRegistered(true);
    client.setAuthenticated(true);

    //& Verify that the setters worked correctly
    assert(client.getNickname() == "test_nick");
    assert(client.getUsername() == "test_user");
    assert(client.getRealname() == "Test User");
    assert(client.isRegistered());
    assert(client.isAuthenticated());

    //& Test the output operator
    std::cout << client;
    std::cout << client;

    std::cout << "All tests passed!" << std::endl;
    return 0;
}

