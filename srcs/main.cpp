#include "../includes/Server.hpp"

bool isNumber(const std::string &str)
{
	if (str.empty())
		return false;

	for (std::size_t i = 0; i < str.size(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return false;
	}

	return true;
}

int Parsing(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>." << std::endl;
		return 1;
	}

	if (!isNumber(argv[1]))
	{
		std::cerr << "Error: port must be a positive number." << std::endl;
		return 1;
	}

	if (std::atoi(argv[1]) < 1 || std::atoi(argv[1]) > 65535)
	{
		std::cerr << "Error: port must be between 1 and 65535." << std::endl;
		return 1;
	}

	if (std::string(argv[2]).empty())
	{
		std::cerr << "Error: password cannot be empty." << std::endl;
		return 1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	if (Parsing(argc, argv) != 0)
		return 1;
	try
	{
		Server server(std::atoi(argv[1]), argv[2]);
		server.run(); // lance juste la boucle poll()
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
