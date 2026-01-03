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
    const char *lineBegin = content;
    int lineLength = 0;
    int line = 0;
    bool previousWasCR = false;
    for (size_t i = 0; i < length; i++)
    {
        auto c = content[i];
        bool lf = (c == 0x0A);
        if (lf && previousWasCR)
        {
            // skip LF after CR
            previousWasCR = false;
            continue;
        }
        bool cr = (c == 0x0D);
        previousWasCR = cr;
        if (!cr && !lf)
            lineLength++;
        bool isLastChar = (i == length - 1);
        if (cr || lf || isLastChar)
        {
            if (line == 0)
            {
                _url = std::string(lineBegin, lineLength);
            }
            else if (line == 1)
            {
                auto commands = std::string(lineBegin, lineLength);
                std::stringstream ss(commands);
                std::string item;
                while (std::getline(ss, item, ';'))
                {
                    _commands.push_back(item);
                }
            }
            else if (line == 2)
            {
                _imageUrl = std::string(lineBegin, lineLength);
            }
            else if (line == 3)
            {
                _imageUrl = std::string(lineBegin, lineLength);
            }
            else
            {
            break;
            }
            line++;
            lineBegin = content + i + 1;
            lineLength = 0;
        }
    }
}

const std::string &Card::getUrl() const
{
    return _url;
}

const std::string &Card::getTitle() const
{
    return _title;
}

const std::string &Card::getImageUrl() const
{
    return _imageUrl;
}

const std::vector<std::string> &Card::getCommands() const
{
    return _commands;
}

bool Card::hasCommand(const std::string &command) const
{
    for (const auto &cmd : _commands)
    {
        if (cmd == command)
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
    if (!_url.empty())
        logInfo("Card", "Url: %s", _url.c_str());
    if (!_title.empty())
        logInfo("Card", "Title: %s", _title.c_str());
    if (!_imageUrl.empty())
        logInfo("Card", "ImageUrl: %s", _imageUrl.c_str());
    for (const auto &command : _commands)
    {
        logInfo("Card", "Command: %s", command.c_str());
    }
}

Card::~Card()
{
    logDebug("Card", "Card destructor called");
}