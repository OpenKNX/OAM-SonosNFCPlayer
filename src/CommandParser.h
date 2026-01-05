#pragma once

#include <string>
#include <vector>
#include "Command.h"

class CommandParser
{
    enum class ParserState
    {
        ParsingName,
        ParsingParamLeadingWS,
        ParsingParamContent,
        ParsingQuotedParam
    };
    static bool isWhitespace(char c);
    static bool isCommandDelimiter(char c);
    static bool isQuote(char c);
    static void trimTrailingWhitespace(std::string& s);
public:
    /// Parses a command chain of the form:
    ///   Command[:Parameter][;Command[:Parameter]]*
    ///
    /// - Whitespace (including line breaks) is ignored outside of quoted strings
    /// - Parameters can be enclosed in "..."
    /// - "" within quotes escapes a "
    static const std::vector<Command> parse(const std::string &input);
};
