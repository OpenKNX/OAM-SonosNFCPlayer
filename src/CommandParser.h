#pragma once

#include <string>
#include <vector>

struct Command
{
    std::string name;
    std::string parameter;
    bool getParameterAsBool(bool defaultValue) const
    {
        if (parameter.empty())
            return true;
        std::string paramLower;
        for (char c : parameter)
            paramLower += std::tolower(c);
        if (paramLower == "true" || paramLower == "1" || paramLower == "yes" || paramLower == "on")
            return true;
        if (paramLower == "false" || paramLower == "0" || paramLower == "no" || paramLower == "off")
            return false;
        return defaultValue;
    }
    
};

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
