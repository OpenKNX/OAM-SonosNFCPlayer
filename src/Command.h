#pragma one
#include "Arduino.h"
#include <string>

struct Command
{
    std::string name;
    std::string parameter;
    bool getParameterAsBool(bool defaultValue) const;
    bool tryGetParameterAsPercentageOrBool(uint8_t &percentage, uint8_t &onOff);
    bool tryGetParameterAsPercent(uint8_t& percentage) const;
    bool tryGetParameterAsInt(int& value) const;
    bool isNameWithIndex(const char* name, uint8_t& index) const;
};