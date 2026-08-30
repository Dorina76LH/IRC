/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 08:46:35 by doberes           #+#    #+#             */
/*   Updated: 2026/08/30 10:43:32 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_CPP
#define BOT_CPP

#include <map>
#include <string>
#include <iostream>
#include <ostream>
#include <iomanip>

class Bot
{
    public:
        //& constructor and destructor
        Bot();
        ~Bot();
        
        //& print the bot ascii art to the console
        static void printBot(std::ostream &os = std::cout);
        static void printFooter(std::ostream &os = std::cout);

        //& handle the help command and print the help message to the client
        void handleHelp(const std::string &param, std::ostream &os = std::cout);

    
    private:
        //& private copy constructor and assignment operator to prevent instantiation
        Bot(const Bot &other);
        Bot &operator=(const Bot &other);

        //& individual help command handlers
        void helpPass(std::ostream &os);
        void helpNick(std::ostream &os);
        void helpUser(std::ostream &os);
        void helpJoin(std::ostream &os);
        void helpPart(std::ostream &os);
        void helpPrivMsg(std::ostream &os);
        void helpInvite(std::ostream &os);
        void helpKick(std::ostream &os);
        void helpTopic(std::ostream &os);
        void helpMode(std::ostream &os);

        //& define pointers to member functions for each help command
        typedef void (Bot::*HelpFunc)(std::ostream &os);

        //& map of command names to their corresponding help functions
        std::map<std::string, HelpFunc> _helpMap;
        
        //& initialize the help map with command names and their corresponding help functions
        void initHelpMap();
};

#endif