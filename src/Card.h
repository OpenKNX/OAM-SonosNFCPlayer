#pragma once
#include <string>
#include <vector>

class Card
{
    std::string _uidStr;
    std::string _url; 
    std::string _title;
    std::string _imageUrl;
    std::vector<std::string> _commands;
public:    
    Card(const uint8_t* uid, unsigned int uidLength, const char* content, unsigned int length);
    ~Card();
    const std::string& getUid() const;
    const std::string& getUrl() const;
    const std::string& getTitle() const;
    const std::string& getImageUrl() const;
    const std::vector<std::string>& getCommands() const;
    bool hasCommand(const std::string& command) const;
    void logInformation() const;
};
