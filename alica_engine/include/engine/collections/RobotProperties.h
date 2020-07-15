#pragma once

#include "engine/Types.h"

#include <essentials/IdentifierConstPtr.h>
#include <SystemConfig.h>

#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace essetials
{
    class IdentifierConstPtr;
}

namespace alica
{

class AlicaEngine;
class Characteristic;
class Capability;

class RobotProperties
{
public:
    RobotProperties();
    RobotProperties(const AlicaEngine* ae, const std::string& name);
    ~RobotProperties();

    essentials::IdentifierConstPtr extractID(std::string& name,  essentials::SystemConfig* sc) const;

    const std::string& getDefaultRole() const { return _defaultRole; }
    std::unordered_map<std::string, Characteristic* > getCharacteristics() const { return _characteristics; }
    const std::string& getName() const { return _name; }

    friend std::ostream& operator<<(std::ostream& os, const alica::RobotProperties& obj)
    {
        os << "RobotProperties: Default Role: " << obj.getDefaultRole() << std::endl;
        return os;
    }

private:
    void readFromConfig(const AlicaEngine* engine, const std::string& name);
    void extractAgentCharacteristics(const std::string& name, essentials::SystemConfig* sc);

    const AlicaEngine* _ae;
    const std::string _name;
    std::unordered_map<std::string, Characteristic* > _characteristics;
    std::string _defaultRole;
};

} /* namespace alica */
