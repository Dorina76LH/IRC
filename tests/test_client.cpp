/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_client.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 10:59:58 by doberes           #+#    #+#             */
/*   Updated: 2026/08/17 11:52:08 by doberes          ###   ########.fr       */
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
    Client clientM(70);
    client.appendToReadBuffer("PASS test");
    client.appendToWriteBuffer("Response pending");
    std::cout << client; // vérifie visuellement que readBuffer/writeBuffer affichent bien le contenu, pas "empty"

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
    
    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "WRITE BUFFER MANAGEMENT TEST" << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;

    //& Test appendToWriteBuffer: raw data is stored as-is, without modification
    std::cout << COLOR_CYAN << "=== Testing raw append (no \\r\\n) ===" << COLOR_RESET << std::endl;
    Client clientF(60);
    clientF.appendToWriteBuffer("Welcome to the server");
    assert(clientF.getWriteBuffer() == "Welcome to the server");
    std::cout << "✅ Data stored as-is, no automatic \\r\\n added." << std::endl;
    std::cout << COLOR_CYAN << "=== Raw append passed! ===\n" << COLOR_RESET << std::endl;

    //& Test appendToWriteBuffer: data already containing \r\n is preserved
    std::cout << COLOR_CYAN << "=== Testing append with \\r\\n already included ===" << COLOR_RESET << std::endl;
    Client clientG(61);
    clientG.appendToWriteBuffer("Already terminated\r\n");
    assert(clientG.getWriteBuffer() == "Already terminated\r\n");
    std::cout << "✅ Data with existing \\r\\n stored unchanged." << std::endl;
    std::cout << COLOR_CYAN << "=== Append with \\r\\n included passed! ===\n" << COLOR_RESET << std::endl;

    //& Test hasDataToSend: empty vs non-empty buffer
    std::cout << COLOR_CYAN << "=== Testing hasDataToSend ===" << COLOR_RESET << std::endl;
    Client clientH(62);
    assert(!clientH.hasDataToSend());
    std::cout << "✅ Empty write buffer correctly reports no data to send." << std::endl;
    clientH.appendToWriteBuffer("Some message");
    assert(clientH.hasDataToSend());
    std::cout << "✅ Non-empty write buffer correctly reports data to send." << std::endl;
    std::cout << COLOR_CYAN << "=== hasDataToSend passed! ===\n" << COLOR_RESET << std::endl;

    //& Test clearSentData: partial send (simulates send() not sending everything at once)
    std::cout << COLOR_CYAN << "=== Testing partial clearSentData ===" << COLOR_RESET << std::endl;
    Client clientI(63);
    clientI.appendToWriteBuffer("HelloWorld\r\n"); // 12 chars
    clientI.clearSentData(5); // simulate send() only sent "Hello"
    assert(clientI.getWriteBuffer() == "World\r\n");
    std::cout << "✅ Partial send correctly removes only the sent bytes." << std::endl;
    std::cout << COLOR_CYAN << "=== Partial clearSentData passed! ===\n" << COLOR_RESET << std::endl;

    //& Test clearSentData: exact full send
    std::cout << COLOR_CYAN << "=== Testing exact clearSentData ===" << COLOR_RESET << std::endl;
    Client clientJ(64);
    clientJ.appendToWriteBuffer("Bye\r\n"); // 5 chars
    clientJ.clearSentData(5); // simulate send() sent everything
    assert(clientJ.getWriteBuffer().empty());
    assert(!clientJ.hasDataToSend());
    std::cout << "✅ Exact full send correctly empties the buffer." << std::endl;
    std::cout << COLOR_CYAN << "=== Exact clearSentData passed! ===\n" << COLOR_RESET << std::endl;

    //& Test clearSentData: defensive case, bytesSent greater than buffer size
    std::cout << COLOR_CYAN << "=== Testing clearSentData with excessive bytesSent ===" << COLOR_RESET << std::endl;
    Client clientK(65);
    clientK.appendToWriteBuffer("Hi\r\n"); // 4 chars
    clientK.clearSentData(999); // way more than the buffer actually contains
    assert(clientK.getWriteBuffer().empty());
    std::cout << "✅ Excessive bytesSent safely clears the whole buffer without crashing." << std::endl;
    std::cout << COLOR_CYAN << "=== Excessive bytesSent passed! ===\n" << COLOR_RESET << std::endl;

    //& Test clearSentData: multiple partial sends accumulating to full buffer
    std::cout << COLOR_CYAN << "=== Testing multiple partial sends ===" << COLOR_RESET << std::endl;
    Client clientL(66);
    clientL.appendToWriteBuffer("ABCDE\r\n"); // 7 chars
    clientL.clearSentData(3); // "ABC" sent
    assert(clientL.getWriteBuffer() == "DE\r\n");
    clientL.clearSentData(4); // remaining "DE\r\n" sent
    assert(clientL.getWriteBuffer().empty());
    std::cout << "✅ Multiple partial sends correctly drain the buffer over time." << std::endl;
    std::cout << COLOR_CYAN << "=== Multiple partial sends passed! ===\n" << COLOR_RESET << std::endl;

    std::cout << COLOR_GREEN << "All write buffer management tests passed!" << COLOR_RESET << std::endl;
    
    std::cout << COLOR_GREEN << "\n==================================================" << std::endl;
    std::cout << "ADDITIONAL EDGE CASE TESTS" << std::endl;
    std::cout << "==================================================\n" << COLOR_RESET << std::endl;

    //& Test clearSentData: zero bytes sent (send() blocked, nothing transmitted)
    std::cout << COLOR_CYAN << "=== Testing clearSentData with 0 bytes sent ===" << COLOR_RESET << std::endl;
    Client clientN(71);
    clientN.appendToWriteBuffer("Test\r\n");
    clientN.clearSentData(0);
    assert(clientN.getWriteBuffer() == "Test\r\n");
    std::cout << "✅ Zero bytesSent correctly leaves the buffer untouched." << std::endl;
    std::cout << COLOR_CYAN << "=== Zero bytesSent passed! ===\n" << COLOR_RESET << std::endl;

    //& Test independence between multiple Client instances
    std::cout << COLOR_CYAN << "=== Testing independence between clients ===" << COLOR_RESET << std::endl;
    Client clientO(72);
    Client clientP(73);
    clientO.setNickname("alice");
    clientP.setNickname("bob");
    assert(clientO.getNickname() == "alice");
    std::cout << "✅ clientO correctly kept its own nickname: " << clientO.getNickname() << std::endl;
    assert(clientP.getNickname() == "bob");
    std::cout << "✅ clientP correctly kept its own nickname: " << clientP.getNickname() << std::endl;
    assert(clientO.getNickname() != clientP.getNickname());
    std::cout << "✅ Modifying one client did not affect the other." << std::endl;
    std::cout << COLOR_CYAN << "=== Independence between clients passed! ===\n" << COLOR_RESET << std::endl;

    //& Test appendToReadBuffer: accumulation with no complete line yet
    std::cout << COLOR_CYAN << "=== Testing accumulation without a complete line ===" << COLOR_RESET << std::endl;
    Client clientQ(74);
    clientQ.appendToReadBuffer("PAR");
    clientQ.appendToReadBuffer("TIAL ");
    clientQ.appendToReadBuffer("DATA");
    assert(!clientQ.hasCompleteLine());
    std::cout << "✅ Buffer correctly reports no complete line yet." << std::endl;
    assert(clientQ.getReadBuffer() == "PARTIAL DATA");
    std::cout << "✅ Buffer correctly accumulated: " << clientQ.getReadBuffer() << std::endl;
    std::cout << COLOR_CYAN << "=== Accumulation without complete line passed! ===\n" << COLOR_RESET << std::endl;

    std::cout << COLOR_GREEN << "All additional edge case tests passed!" << COLOR_RESET << std::endl;
    
    return 0;
}

