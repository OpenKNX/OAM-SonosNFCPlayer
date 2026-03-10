#pragma once
#include <string>
#include <vector>
#include "CommandParser.h"

class Card
{
    std::string _uidStr;
    std::vector<Command> _commands;
public:    
    Card(const std::string& uid, const char* content, size_t length);
    ~Card();
    const std::string& getUid() const;
    const std::vector<Command>& getCommands() const;
    bool hasCommand(const std::string& name) const;
    void logInformation() const;

    static std::string createUidString(const uint8_t* uid, unsigned int uidLength);
   
    
};
