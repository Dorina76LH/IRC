#include "../../includes/Commands.hpp"
#include <cctype>

// RFC 1459 : nicknames are limited to 9 characters.
static const size_t        MAX_NICKNAME_LENGTH = 9;

//RFC 1459 : <special> ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'
// '|' is added on top of the strict RFC 1459 grammar: because of IRC's Scandinavian
// origin (RFC 1459, 2.2), '{' '}' '|' are the lowercase equivalents of '[' ']' '\'
// respectively, so a nickname grammar that accepts the uppercase forms must also
// accept their lowercase counterpart.
static const std::string   ALLOWED_SPECIAL_CHARACTERS = "-[]\\`^{}|";

static bool isLetter(char character)
{
    return (std::isalpha(static_cast<unsigned char>(character)) != 0);
}

static bool isDigit(char character)
{
    return (std::isdigit(static_cast<unsigned char>(character)) != 0);
}

static bool isAllowedSpecialCharacter(char character)
{
    return (ALLOWED_SPECIAL_CHARACTERS.find(character) != std::string::npos);
}

// RFC 1459, 2.2 : because of IRC's Scandinavian origin, '{' '}' '|' are considered
// the lowercase equivalents of '[' ']' '\' respectively. This mapping is critical
// when determining the equivalence of two nicknames: "ada" and "ADA" are the same
// nickname, but so are "{ada}" and "[ADA]".
static char ircToUpper(char character)
{
    switch (character)
    {
        case '{': return ('[');
        case '}': return (']');
        case '|': return ('\\');
        default:  return (static_cast<char>(std::toupper(static_cast<unsigned char>(character))));
    }
}

static bool ircNicknamesAreEqual(const std::string &firstNickname, const std::string &secondNickname)
{
    if (firstNickname.size() != secondNickname.size())
        return (false);

    for (size_t index = 0; index < firstNickname.size(); ++index)
    {
        if (ircToUpper(firstNickname[index]) != ircToUpper(secondNickname[index]))
            return (false);
    }
    return (true);
}

// Checks a candidate nickname against the RFC 1459 grammar:
// <nick> ::= <letter> { <letter> | <number> | <special> }
static bool isValidNicknameSyntax(const std::string &nicknameCandidate)
{
    if (nicknameCandidate.empty())
        return (false);
    if (nicknameCandidate.size() > MAX_NICKNAME_LENGTH)
        return (false);
    if (!isLetter(nicknameCandidate[0]))
        return (false);

    for (size_t index = 1; index < nicknameCandidate.size(); ++index)
    {
        char currentCharacter = nicknameCandidate[index];
        if (!isLetter(currentCharacter) && !isDigit(currentCharacter) && !isAllowedSpecialCharacter(currentCharacter))
            return (false);
    }
    return (true);
}

static bool isNicknameAlreadyUsed(const std::string &nicknameCandidate, const std::vector<std::string> &nicknamesInUse)
{
    for (size_t index = 0; index < nicknamesInUse.size(); ++index)
    {
        if (ircNicknamesAreEqual(nicknamesInUse[index], nicknameCandidate))
            return (true);
    }
    return (false);
}

void Commands::handleNick(Client &client, const std::vector<std::string> &commandParams, const std::vector<std::string> &nicknamesInUse)
{
    // Before registration, an unregistered client has no nickname yet:
    // RFC 1459 uses '*' as the target of replies sent to such a client.
    std::string replyTarget = client.getNickname().empty() ? "*" : client.getNickname();

    if (!client.isAuthenticated())
    {
        client.appendToWriteBuffer(Commands::buildReply("451", replyTarget, "You have not registered"));
        return;
    }

    if (commandParams.empty() || commandParams[0].empty())
    {
        client.appendToWriteBuffer(Commands::buildReply("431", replyTarget, "No nickname given"));
        return;
    }

    const std::string &nicknameCandidate = commandParams[0];

    if (!isValidNicknameSyntax(nicknameCandidate))
    {
        client.appendToWriteBuffer(Commands::buildReply("432", nicknameCandidate, "Erroneous nickname"));
        return;
    }

    if (isNicknameAlreadyUsed(nicknameCandidate, nicknamesInUse))
    {
        client.appendToWriteBuffer(Commands::buildReply("433", nicknameCandidate, "Nickname is already in use"));
        return;
    }

    client.setNickname(nicknameCandidate);
}
