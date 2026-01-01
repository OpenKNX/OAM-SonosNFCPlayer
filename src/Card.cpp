#include "Card.h"


Card::Card(std::string content) : _textContent(content)
{
}

std::string Card::getTextContent() const
{
    return _textContent;
}