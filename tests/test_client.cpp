/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_client.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 10:59:58 by doberes           #+#    #+#             */
/*   Updated: 2026/08/16 17:57:02 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//* -- Includes -- *//
#include "../includes/Client.hpp"
#include <iostream>
#include <cassert>

//* -- Colors -- *//
#define COLOR_RESET "\033[0m"
#define COLOR_CYAN  "\033[1;36m"
#define COLOR_GREEN "\033[1;32m"

//* -- Compilation and Execution Instructions -- *//
// c++ -std=c++98 -Wall -Wextra -Werror tests/test_client.cpp srcs/Client.cpp -o test_client

//& assert(condition) : if condition is false, the program aborts immediately
//& and prints the failing condition, file, and line number to stderr.
//& This lets us catch bugs early — as soon as one assertion fails, execution
//& stops, so all assertions ABOVE it in the code are known to have passed.

int main()
{
    
    //& Create a Client object with a dummy socket file descriptor
    Client client(42);
    
    //& Test private canonical methods
    //? Since the private canonical methods (default constructor, copy constructor,
    //? and assignment operator) are declared but not implemented, we cannot directly
    //? test them. However, we can ensure that they are inaccessible by trying to use
    //? them in a way that would cause a compilation error if they were accessible.
    
    // Client client_default;           // do not compile, default constructor is private
    // Client client_copy(client);      // do not compile, copy constructor is private
    // client = client_copy;            // do not compile, assignment operator is private 
    
    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "CLASS BASIC TESTS " << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;
    
    //& Test getters and parameterized constructor
    std::cout << COLOR_CYAN << "=== Testing parameterized constructor and getters ===" << COLOR_RESET << std::endl;
    assert(client.getFdSocket() == 42);
    std::cout << "✅ Socket file descriptor is correctly." << std::endl;
    assert(client.getNickname().empty());
    std::cout << "✅ Nickname is empty." << std::endl;
    assert(client.getUsername().empty());
    std::cout << "✅ Username is empty." << std::endl;
    assert(client.getRealname().empty());
    std::cout << "✅ Realname is empty." << std::endl;
    assert(!client.isRegistered());
    std::cout << "✅ Client is not registered." << std::endl;
    assert(!client.isAuthenticated());
    std::cout << "✅ Client is not authenticated." << std::endl;
    assert(client.getReadBuffer().empty());
    std::cout << "✅ Read buffer is empty." << std::endl;
    assert(client.getWriteBuffer().empty());
    std::cout << "✅ Write buffer is empty." << std::endl;

    //& Test setters
    std::cout << COLOR_CYAN << "\n=== Testing setters ===" << COLOR_RESET << std::endl;
    client.setNickname("test_nick");
    client.setUsername("test_user");
    client.setRealname("Test User");
    client.setRegistered(true);
    client.setAuthenticated(true);
    
    //& Verify that the setters worked correctly
    assert(client.getNickname() == "test_nick");
    std::cout << "✅ Nickname set to: " << client.getNickname() << std::endl;
    assert(client.getUsername() == "test_user");
    std::cout << "✅ Username set to: " << client.getUsername() << std::endl;
    assert(client.getRealname() == "Test User");
    std::cout << "✅ Realname set to: " << client.getRealname() << std::endl;
    assert(client.isRegistered());
    std::cout << "✅ Client registered status set to: " << (client.isRegistered() ? "true" : "false") << std::endl;
    assert(client.isAuthenticated());
    std::cout << "✅ Client authenticated status set to: " << (client.isAuthenticated() ? "true" : "false") << std::endl;

    //& Test the output operator
    std::cout << COLOR_CYAN << "\n=== Testing output operator ===" << COLOR_RESET << std::endl;
    std::cout << client;
    std::cout << client;

    std::cout << COLOR_GREEN << "All constructor / destructor / getter/ setter / << passed!" << COLOR_RESET << std::endl;
    
    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "BUFFER MANAGEMENT TEST" << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;

    //& Test read buffer: strict \r\n (RFC compliant)
    std::cout << COLOR_CYAN << "=== Testing \\r\\n line ending ===" << COLOR_RESET << std::endl;
    Client clientA(50);
    clientA.appendToReadBuffer("NICK 42student\r\n");
    assert(clientA.hasCompleteLine());
    assert(clientA.extractLine() == "NICK 42student");
    assert(!clientA.hasCompleteLine());
    assert(clientA.getReadBuffer().empty());
    std::cout << "✅ \\r\\n terminated line correctly extracted." << std::endl;

    //& Test read buffer: \n only (netcat/telnet style)
    std::cout << COLOR_CYAN << "\n=== Testing \\n only line ending ===" << COLOR_RESET << std::endl;
    Client clientB(51);
    clientB.appendToReadBuffer("NICK 42student\n");
    assert(clientB.hasCompleteLine());
    assert(clientB.extractLine() == "NICK 42student");
    assert(!clientB.hasCompleteLine());
    std::cout << "✅ \\n only terminated line correctly extracted." << std::endl;

    //& Test read buffer: multiple lines at once, mixed endings
    std::cout << COLOR_CYAN << "\n=== Testing multiple lines in one buffer ===" << COLOR_RESET << std::endl;
    Client clientC(52);
    clientC.appendToReadBuffer("PASS abc\r\nNICK bob\n");
    assert(clientC.hasCompleteLine());
    assert(clientC.extractLine() == "PASS abc");
    assert(clientC.hasCompleteLine());
    assert(clientC.extractLine() == "NICK bob");
    assert(!clientC.hasCompleteLine());
    std::cout << "✅ Multiple mixed-ending lines correctly extracted in order." << std::endl;

    //& Test read buffer: line split across two appends (simulates partial recv())
    std::cout << COLOR_CYAN << "\n=== Testing line split across two appends ===" << COLOR_RESET << std::endl;
    Client clientD(53);
    clientD.appendToReadBuffer("NICK inco");
    assert(!clientD.hasCompleteLine());
    assert(clientD.extractLine() == "");
    clientD.appendToReadBuffer("mplete\n");
    assert(clientD.hasCompleteLine());
    assert(clientD.extractLine() == "NICK incomplete");
    std::cout << "✅ Line split across two appends correctly reassembled." << std::endl;

    //& Test read buffer: empty buffer
    std::cout << COLOR_CYAN << "\n=== Testing empty buffer ===" << COLOR_RESET << std::endl;
    Client clientE(54);
    assert(!clientE.hasCompleteLine());
    assert(clientE.extractLine() == "");
    std::cout << "✅ Empty buffer correctly returns no line." << std::endl;

    std::cout << COLOR_GREEN << "\nAll buffer management tests passed!" << COLOR_RESET << std::endl;
    
    return 0;
}

