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
bool Command::TryGetParameterAsPercent(uint8_t &percentage) const
{
    try
    {
        auto value = std::stoi(parameter);
        if (value < 0 || value > 100)
        {
            return false;
        }
        percentage = static_cast<uint>(value);
    }
    catch (const std::exception &)
    {
        return false; // keine gültige Zahl
    }
    return true;
}

bool Command::isNameWithIndex(const char* name, uint8_t &index) const
{
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