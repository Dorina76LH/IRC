/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_bot.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: doberes <doberes@student.42lehavre.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/30 09:51:22 by doberes           #+#    #+#             */
/*   Updated: 2026/08/30 10:22:58 by doberes          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Bot.hpp"
#include <iostream>
#include <iomanip>

// c++ -std=c++98 -Wall -Wextra -Werror tests/test_bot.cpp srcs/Bot.cpp -o test_bot


int main()
{
    Bot bot;

    std::cout << "\n=== TEST 1: HELP sans paramètre ===" << std::endl;
    bot.handleHelp("");

    std::cout << "\n=== TEST 2: HELP NICK ===" << std::endl;
    bot.handleHelp("NICK");

    std::cout << "\n=== TEST 3: HELP COMMANDE_INCONNUE ===" << std::endl;
    bot.handleHelp("LOL");

    return 0;
}