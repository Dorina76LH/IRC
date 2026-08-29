#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>


class Parser
{
    /*
        WHAT IS THE PARSER?
        The Parser turns a raw IRC protocol line (already reassembled by Client)
        into a command name and a list of parameters, ready to be dispatched to
        Commands::handleX(). It knows nothing about sockets, clients or channels:
        its only job is to understand the IRC line syntax.
    */
    public:

        // Splits a single IRC line into a command name and its parameters.
        static void parseLine(const std::string &line, std::string &command, std::vector<std::string> &params);

    private:

        // Utility class only: no instance should ever be created.
        Parser();
        ~Parser();
        Parser(const Parser &other);
        Parser &operator=(const Parser &other);
};

#endif
