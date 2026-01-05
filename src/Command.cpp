#include "Command.h"
#include "OpenKNX.h"

bool Command::getParameterAsBool(bool defaultValue) const
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
bool Command::tryGetParameterAsPercent(uint8_t &percentage) const
{
    percentage = 255;
    try
    {
        auto value = std::stoi(parameter);
        if (value < 0 || value > 100)
        {
            logWarning("Command", "Parameter '%s' is not a valid percentage", parameter.c_str());
            return false;
        }
        percentage = static_cast<uint8_t>(value);
    }
    catch (const std::exception &)
    {
        return false; // keine gültige Zahl
    }
    return true;
}

bool Command::tryGetParameterAsInt(int &value) const
{
    value = 0;
    try
    {
        value = std::stoi(parameter);
    }
    catch (const std::exception &)
    {
        return false; // keine gültige Zahl
    }
    return true;
}


bool Command::tryGetParameterAsPercentageOrBool(uint8_t &percentage, uint8_t &onOff)
{
    percentage = 255;
    onOff = 255;
    uint8_t device = 0;
    try
    {
        std::string lowerCaseParameter;
        for (char c : parameter)
            lowerCaseParameter += std::tolower(c);
        if (lowerCaseParameter == "on")
        {
            onOff = 1;
        }
        else if (lowerCaseParameter == "off")
        {
            onOff = 0;
        }
        else
        {
            auto percentageValue = std::stoi(lowerCaseParameter);
            if (percentageValue < 0 || percentageValue > 100)
            {
                logWarning("Command", "Command '%s' contains invalid percentage %d", name.c_str(), percentageValue);
                return false;
            }
            percentage = percentageValue;
        }
    }
    catch (const std::exception &e)
    {
        logWarning("Command", "Command '%s' contains invalid parameter %s: %s", name.c_str(), parameter.c_str(), e.what());
        return false; // keine gültige Zahl
    }
    return true;
}

bool Command::isNameWithIndex(const char* name, uint8_t &index) const
{
    index = 255;
    if (this->name.rfind(name, 0) != 0)
    {
        return false;
    }
    std::string indexStr = this->name.substr(strlen(name));
    if (indexStr.empty())
    {
        logWarning("Command", "Command name '%s' missing index", this->name.c_str());
        return false;
    }
    try
    {
        auto idx = std::stoi(indexStr);
        if (idx < 1 || idx > 255)
        {
            logWarning("Command", "Command name '%s' has index out of range", this->name.c_str());
            return false;
        }
        index = static_cast<uint8_t>(idx);
    }
    catch (const std::exception &)
    {
        logWarning("Command", "Command name '%s' has invalid index", this->name.c_str());
        return false; // keine gültige Zahl
    }
    return true;
}
