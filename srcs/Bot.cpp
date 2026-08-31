/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 08:49:14 by doberes           #+#    #+#             */
/*   Updated: 2026/08/30 14:15:33 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// --- INCLUDES ---
#include "../includes/Bot.hpp"

// --- Colors ---
#define COLOR_RESET "\033[0m"
#define COLOR_CYAN  "\033[1;36m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_YELLOW "\033[1;33m"

//* Constructor and Destructor

//& Bot constructor initializes the help map with command names and their corresponding help functions
Bot::Bot()
{
    initHelpMap();
}

//& Bot destructor
Bot::~Bot() {}

//* Printing functions

//& Prints the bot ASCII art to the console
void Bot::printBot(std::ostream &os)
{
    os  << COLOR_CYAN
        << "\n===============================================\n"
        << "      ,___,\n"
        << "      [O.o]  IRC BOT ACTIVE\n"
        << "      /)  )  Ready to serve\n"
        << "     --\"--\"\n"
        << "===============================================\n" 
        << COLOR_RESET << std::endl;
}

//& Prints a footer message to the console
void Bot::printFooter(std::ostream &os)
{
    os  << COLOR_CYAN 
        << "\n===============================================\n"
        << "    ,___,\n"
        << "    [O.o]  End of help. \n"
        << "    /)  )  Have a nice IRC chat!\n"
        << "   --\"--\"\n"
        << "===============================================\n" 
        << COLOR_RESET << std::endl;
}

//* initHelpMap
// Initializes the help map with command names and their corresponding help functions
void Bot::initHelpMap()
{
    _helpMap["INVITE"] = &Bot::helpInvite;
    _helpMap["JOIN"] = &Bot::helpJoin;
    _helpMap["KICK"] = &Bot::helpKick;
    _helpMap["MODE"] = &Bot::helpMode;
    _helpMap["NICK"] = &Bot::helpNick;
    _helpMap["PART"] = &Bot::helpPart;
    _helpMap["PASS"] = &Bot::helpPass;
    _helpMap["PRIVMSG"] = &Bot::helpPrivMsg;
    _helpMap["TOPIC"] = &Bot::helpTopic;
    _helpMap["USER"] = &Bot::helpUser;
}

//* Individual help command handlers

void Bot::helpInvite(std::ostream &os)
{
    os << COLOR_GREEN << "\n=== Command INVITE ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Invites a user to join a channel." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "INVITE <nickname> <channel>" << std::endl;
    os << COLOR_YELLOW << "Notes       : " << COLOR_RESET << "Requester must be on the channel. On +i channels, operator status is required." << std::endl;
}

void Bot::helpJoin(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command JOIN ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Joins a channel. Creates it if it doesn't exist." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "JOIN <channel>{,<channel>} [<key>{,<key>}]" << std::endl;
}

void Bot::helpKick(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command KICK ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Forcibly removes a user from a channel." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "KICK <channel> <nickname> [:<reason>]" << std::endl;
    os << COLOR_YELLOW << "Notes       : " << COLOR_RESET << "Requires channel operator privileges." << std::endl;
}

void Bot::helpMode(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command MODE ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Sets or queries channel modes." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "MODE <channel> <+|-<modes>> [<params>]" << std::endl;
    os << COLOR_YELLOW << "Modes       : " << COLOR_RESET << "i (invite-only), t (topic restriction), k (key), o (operator), l (user limit)" << std::endl;
}

void Bot::helpNick(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command NICK ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Sets or changes your nickname." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "NICK <nickname>" << std::endl;
}

void Bot::helpPass(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command PASS ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Authenticates your connection with the server password." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "PASS <password>" << std::endl;
    os << COLOR_YELLOW << "Notes       : " << COLOR_RESET << "Must be sent before NICK and USER commands." << std::endl;
}

void Bot::helpPart(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command PART ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Leaves one or multiple channels." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "PART <channel>{,<channel>} [:<reason>]" << std::endl;
}

void Bot::helpPrivMsg(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command PRIVMSG ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Sends a private message to a user or a channel." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "PRIVMSG <target> :<message>" << std::endl;
}

void Bot::helpTopic(std::ostream &os)
{
    os << COLOR_GREEN  << "\n=== Command TOPIC ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Queries or changes the topic of a channel." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "TOPIC <channel> [:<new_topic>]" << std::endl;
    os << COLOR_YELLOW << "Notes       : " << COLOR_RESET << "If channel is +t, only channel operators can change the topic." << std::endl;
}

void Bot::helpUser(std::ostream &os)
{
    os << COLOR_GREEN  << "=== Command USER ===" << COLOR_RESET << std::endl;
    os << COLOR_YELLOW << "Description : " << COLOR_RESET << "Specifies username, hostname, servername and realname." << std::endl;
    os << COLOR_YELLOW << "Usage       : " << COLOR_RESET << "USER <username> <hostname> <servername> :<realname>" << std::endl;
    os << COLOR_YELLOW << "Notes       : " << COLOR_RESET << "Used during registration after PASS." << std::endl;
}

// Handles the help command and prints the help message to the client
void Bot::handleHelp(const std::string &param, std::ostream &os)
{
    //? STEP 1 : Print the bot ASCII art to the console
    printBot(os);
    
    
    //? STEP 2.1 :  If no specific command is requested, print a list of all available commands 
    if (param.empty())
    {
        //& deine iterator for the help map
        std::map<std::string, HelpFunc>::iterator it;

        //& Print the list of available commands
        
        os << COLOR_GREEN << "\n========================================" << COLOR_RESET << std::endl;
        os << COLOR_GREEN << "       AVAILABLE COMMANDS LIST          " << COLOR_RESET << std::endl;
        os << COLOR_GREEN << "========================================" << COLOR_RESET << std::endl;
        
        for (it = _helpMap.begin(); it != _helpMap.end(); ++it)
        {
            //& Call the corresponding help function for each command
            (this->*(it->second))(os);
        }
    }

    //? STEP 2.2 : If a specific command is requested, print the help message for that command
    else
    {
        //& Find the requested command in the help map
        std::map<std::string, HelpFunc>::iterator it = _helpMap.find(param);
        if (it != _helpMap.end())
        {
            //& Call the corresponding help function for the requested command
            (this->*(it->second))(os);
        }
        else
        {
            //& Print an error message if the requested command is not found
            os << COLOR_GREEN << "\nUnknown command: " << param << COLOR_RESET << std::endl;
        }
    }
    
    //? STEP 3 : Print the footer message to the console
    printFooter(os);
}
