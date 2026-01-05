#include "CommandParser.h"

bool CommandParser::isWhitespace(char c)
{
    return c == ' ' || c == '\t';
}

bool CommandParser::isCommandDelimiter(char c)
{
    return c == ';' || c == '\n' || c == '\r';
}

bool CommandParser::isQuote(char c)
{
    return c == '"' || c == '\'';
}

void CommandParser::trimTrailingWhitespace(std::string &s)
{
    while (!s.empty() && isWhitespace(s.back()))
    {
        s.pop_back();
    }
}

const std::vector<Command> CommandParser::parse(const std::string &input)
{
    std::vector<Command> result;

    Command current;
    ParserState state = ParserState::ParsingName;
    char cQuateChar = '\0';

    for (size_t i = 0; i < input.size(); ++i)
    {
        char c = input[i];

        switch (state)
        {

        case ParserState::ParsingName:
            if (isWhitespace(c))
            {
                // ignore
            }
            else if (c == ':')
            {
                state = ParserState::ParsingParamLeadingWS;
            }
            else if (isCommandDelimiter(c))
            {
                if (!current.name.empty())
                    result.push_back(current);
                current = {};
            }
            else
            {
                current.name += std::tolower(c);
            }
            break;

        case ParserState::ParsingParamLeadingWS:
            if (isWhitespace(c))
            {
                // ignore
            }
            else if (isQuote(c))
            {
                state = ParserState::ParsingQuotedParam;
                cQuateChar = c;
            }
            else if (isCommandDelimiter(c))
            {
                // emtpy parameter
                result.push_back(current);
                current = {};
                state = ParserState::ParsingName;
            }
            else
            {
                current.parameter += c;
                state = ParserState::ParsingParamContent;
            }
            break;

        case ParserState::ParsingParamContent:
            if (isCommandDelimiter(c))
            {
                trimTrailingWhitespace(current.parameter);
                result.push_back(current);
                current = {};
                state = ParserState::ParsingName;
            }
            else
            {
                current.parameter += c;
            }
            break;

        case ParserState::ParsingQuotedParam:
            if (c == cQuateChar)
            {
                if (i + 1 < input.size() && input[i + 1] == cQuateChar)
                {
                    current.parameter += cQuateChar;
                    ++i;
                }
                else
                {
                    state = ParserState::ParsingParamContent;
                }
            }
            else
            {
                current.parameter += c;
            }
            break;
        }
    }

    // push last command
    if (!current.name.empty() || !current.parameter.empty())
    {
        trimTrailingWhitespace(current.parameter);
        result.push_back(current);
    }

    return result;
}
