/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Help.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 10:37:07 by doberes           #+#    #+#             */
/*   Updated: 2026/08/30 11:18:31 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include "../../includes/Commands.hpp"

//* Handle the HELP command and print the help message to the client
// client : the client who sent the HELP command (registration status is not relevant for HELP)
void Commands::handleHelp(Client &client, const std::vector<std::string> &commandParams, Bot &bot)
{
    //? STEP 1 : Extract the optional command name parameter from commandParams
    std::string param = "";
    if (!commandParams.empty())
    {
        param = commandParams[0];
        
        //& convert the command name to uppercase for case-insensitive comparison
        for (size_t i = 0; i < param.length(); ++i)
        {
            param[i] = static_cast<char>(std::toupper(param[i]));
        }    
    }

    //? STEP 2 : Call the bot's handleHelp method to print the help message to the client
    std::ostringstream botStream;
    bot.handleHelp(param, botStream);

    //? STEP 3 : Define replyTarget and botPrefix for the reply message
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();
    std::string botPrefix = ":HelpBot!bot@localhost PRIVMSG ";

    //? STEP 4 : Send the help message to the client line by line
    std::istringstream helpStream(botStream.str());
    std::string line;
    
    //& read each line from the bot's help message and send it to the client
    while (std::getline(helpStream, line))
    {
        //& Remove any trailing carriage return character from the line
        if (!line.empty() && line[line.size() -1 ] == '\r')
        {
            line.erase(line.size() - 1);
        }
        
        //& If the line is empty, send a single space to avoid sending an empty message
        if (line.empty())
        {
            line = " ";
        }
        
        //& Build the PRIVMSG reply for each line and append it to the client's write buffer
        client.appendToWriteBuffer(botPrefix + replyTarget + " :" + line + "\r\n");
    }
}


