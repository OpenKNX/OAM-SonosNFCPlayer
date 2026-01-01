#pragma once
#include <string>

class Card
{
    std::string _textContent; 
public:    
    Card(std::string content);
    std::string getTextContent() const;
};
