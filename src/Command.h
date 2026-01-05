#pragma one
#include "Arduino.h"
#include <string>

struct Command
{
    std::string name;
    std::string parameter;
    bool getParameterAsBool(bool defaultValue) const;
    bool TryGetParameterAsPercent(uint8_t& percentage) const;
    bool isNameWithIndex(const char* name, uint8_t& index) const;
};