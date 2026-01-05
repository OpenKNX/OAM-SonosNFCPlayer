#include "Card.h"
#include "OpenKNX.h"
#include <sstream>

Card::Card(const uint8_t *uid, unsigned int uidLength, const char *content, unsigned int length)
{
    logDebug("Card", "Card constructor called");
    for (int i = 0; i < uidLength; i++)
    {
        char buf[4];
        sprintf(buf, "%02X", (uint8_t)uid[i]);
        _uidStr += buf;
    }
    std::string contentStr(content, length);
    if (contentStr.rfind("x-file-cifs://192.168.0.1/Share/Storage/Musik/") == 0)
    {
        // old card format, convert to new format
        contentStr = "uri:x-file-cifs:" + contentStr.substr(46);
        logDebug("Card", "Converted old card content to new format: %s", contentStr.c_str());
    }
    _commands = CommandParser::parse(contentStr);
}

const std::vector<Command> &Card::getCommands() const
{
    return _commands;
}

bool Card::hasCommand(const std::string &name) const
{
    for (const auto &cmd : _commands)
    {
        if (cmd.name == name)
            return true;
    }
    return false;
}

const std::string &Card::getUid() const
{
    return _uidStr;
}

void Card::logInformation() const
{

    logInfo("Card", "UID: %s", _uidStr.c_str());
    for (const auto &command : _commands)
    {
        if (command.parameter.length() > 0)
            logInfo("Card", "%s: '%s'", command.name.c_str(), command.parameter.c_str());
        else
            logInfo("Card", "%s", command.name.c_str());
    }
}

Card::~Card()
{
    logDebug("Card", "Card destructor called");
}
