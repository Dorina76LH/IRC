#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "Server.hpp"
#include "Client.hpp"
#include "Parser.hpp"

volatile bool g_running = true;

// g++ -Wall -Wextra -Werror -std=c++98 -Iincludes tests/test_server.cpp srcs/Client.cpp srcs/Parser.cpp srcs/Server.cpp srcs/commands/*.cpp -o test_server && ./test_server

// -------------------------------------------------------------
// 1. TESTS DU BUFFER ET DU PARSER
// -------------------------------------------------------------
static void testBufferAndParser()
{
	std::cout << "[1/2] Test du Buffer et du Parser..." << std::endl;

	// Test Buffer : Commande fragmentée
	{
		Client client(4);
		client.appendToReadBuffer("PAS");
		assert(client.hasCompleteLine() == false);
		client.appendToReadBuffer("S abc\r\n");
		assert(client.hasCompleteLine() == true);
		assert(client.extractLine() == "PASS abc");
	}

	// Test Parser : Commande USER avec traînant ':'
	{
		std::string cmd;
		std::vector<std::string> args;
		Parser::parseLine("USER art 0 * :Bob Michel", cmd, args);
		assert(cmd == "USER");
		assert(args.size() == 4);
		assert(args[0] == "art");
		assert(args[3] == "Bob Michel");
	}

	std::cout << "  -> Buffer et Parser OK!" << std::endl;
}

// -------------------------------------------------------------
// 2. TEST DU BUFFER MULTI-COMMANDES
// -------------------------------------------------------------
static void testMultiCommandBuffer()
{
	std::cout << "[2/3] Test du buffer multi-commandes (un seul recv())..." << std::endl;

	// Deux commandes completes recues en un seul appel a appendToReadBuffer,
	// pour simuler un seul recv() ramenant plusieurs lignes IRC d'un coup.
	{
		Client client(4);
		client.appendToReadBuffer("NICK Bob\r\nUSER art 0 * :Bob Michel\r\n");

		assert(client.hasCompleteLine() == true);
		assert(client.extractLine() == "NICK Bob");

		assert(client.hasCompleteLine() == true);
		assert(client.extractLine() == "USER art 0 * :Bob Michel");

		assert(client.hasCompleteLine() == false);
	}

	// Meme scenario, mais en verifiant que le dispatcher traite bien
	// les deux commandes extraites l'une apres l'autre, dans l'ordre.
	{
		Server server(6667, "secretpass");
		Client client(4);

		server.processClientMessage(&client, "PASS secretpass");
		assert(client.isAuthenticated() == true);

		client.appendToReadBuffer("NICK Bob\r\nUSER art 0 * :Bob Michel\r\n");
		while (client.hasCompleteLine())
			server.processClientMessage(&client, client.extractLine());

		assert(client.getNickname() == "Bob");
		assert(client.getUsername() == "art");
		assert(client.isRegistered() == true);
	}

	std::cout << "  -> Extraction consecutive OK !" << std::endl;
}

// -------------------------------------------------------------
// 3. TESTS DES COMMANDES DU SERVEUR
// -------------------------------------------------------------
static void testCommandsExecution()
{
	std::cout << "[3/3] Test de l'execution des commandes IRC..." << std::endl;

	Server server(6667, "secretpass");
	Client client(4);

	// TEST PASS : Mauvais mot de passe -> 464 ERR_PASSWDMISMATCH
	server.processClientMessage(&client, "PASS wrongpass");
	assert(client.isAuthenticated() == false);
	assert(client.getWriteBuffer().find("464") != std::string::npos);
	client.clearSentData(client.getWriteBuffer().size());

	// TEST PASS : Bon mot de passe
	server.processClientMessage(&client, "PASS secretpass");
	assert(client.isAuthenticated() == true);
	assert(client.isRegistered() == false);

	// TEST NICK : Parametre manquant -> 431 ERR_NONICKNAMEGIVEN
	server.processClientMessage(&client, "NICK");
	assert(client.getNickname().empty());
	assert(client.getWriteBuffer().find("431") != std::string::npos);
	client.clearSentData(client.getWriteBuffer().size());

	// TEST NICK : Valide
	server.processClientMessage(&client, "NICK Bob");
	assert(client.getNickname() == "Bob");
	assert(client.isRegistered() == false);

	// TEST USER : Parametres insuffisants -> 461 ERR_NEEDMOREPARAMS
	server.processClientMessage(&client, "USER art 0 *");
	assert(client.isRegistered() == false);
	assert(client.getWriteBuffer().find("461") != std::string::npos);
	client.clearSentData(client.getWriteBuffer().size());

	// TEST USER : Valide -> Finalise l'enregistrement et envoie 001 RPL_WELCOME
	server.processClientMessage(&client, "USER art 0 * :Bob Michel");
	assert(client.getUsername() == "art");
	assert(client.getRealname() == "Bob Michel");
	assert(client.isRegistered() == true);
	assert(client.getWriteBuffer().find("001") != std::string::npos);
	client.clearSentData(client.getWriteBuffer().size());

	// TEST USER : Deja enregistre -> 462 ERR_ALREADYREGISTRED
	server.processClientMessage(&client, "USER art 0 * :Bob Michel");
	assert(client.getWriteBuffer().find("462") != std::string::npos);
	client.clearSentData(client.getWriteBuffer().size());

	// TEST COMMANDE INCONNUE -> 421 ERR_UNKNOWNCOMMAND
	server.processClientMessage(&client, "FOOBAR");
	assert(client.getWriteBuffer().find("421") != std::string::npos);
	client.clearSentData(client.getWriteBuffer().size());

	std::cout << "  -> Execution des commandes OK !" << std::endl;
}

int main()
{
	std::cout << "=== SUITE DE TESTS SERVEUR IRC ===" << std::endl;

	testBufferAndParser();
	testMultiCommandBuffer();
	testCommandsExecution();

	std::cout << "✅ TOUS LES TESTS ASSERTS SONT VALIDÉS !" << std::endl;
	return 0;
}