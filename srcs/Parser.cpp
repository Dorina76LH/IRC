#include "../includes/Parser.hpp"

/*

-> tolère d'éventuels espaces en tout début de ligne

-> extrait le premier mot comme command

-> boucle sur le reste : a chaque tour, si le char courant est ':', prend tout le reste comme dernier parametre unique et sort, 
    
-> sinon decoupe normalement au prochain espace gere les espaces multiples consecutifs sans creer de parametres vides 

*/
void Parser::parseLine(const std::string &line, std::string &command, std::vector<std::string> &params)
{
    command.clear();
    params.clear();

    size_t pos = 0;

    // 
    while (pos < line.size() && line[pos] == ' ')
        pos++;

    size_t spacePos = line.find(' ', pos);
    if (spacePos == std::string::npos)
    {
        command = line.substr(pos);
        return;
    }
    command = line.substr(pos, spacePos - pos);
    pos = spacePos;

    while (pos < line.size())
    {
        while (pos < line.size() && line[pos] == ' ')
            pos++;
        if (pos >= line.size())
            break;

        if (line[pos] == ':')
        {
            params.push_back(line.substr(pos + 1));
            break;
        }

        spacePos = line.find(' ', pos);
        if (spacePos == std::string::npos)
        {
            params.push_back(line.substr(pos));
            break;
        }

        params.push_back(line.substr(pos, spacePos - pos));
        pos = spacePos;
    }
}
