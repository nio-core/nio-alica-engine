#include "engine/IRoleAssignment.h"

#include "engine/AlicaEngine.h"

namespace alica
{
IRoleAssignment::IRoleAssignment()
        : ownRole(nullptr)
        , communication(nullptr)
{
}

const Role* IRoleAssignment::getRole(essentials::IdentifierConstPtr robotId)
{

    // for ( std::pair<essentials::IdentifierConstPtr, const Role *> mapping : this->robotRoleMapping) {
    //     std::cout << "\033[0;35m" << "IRA: mapping   agent id " << mapping.first.hash()  << "    role " << mapping.second->getName() << "\033[0m" << std::endl;
    // }

    auto iter = this->robotRoleMapping.find(robotId);
    if (iter != this->robotRoleMapping.end()) {
        return iter->second;
    } else {
        std::stringstream ss;
        ss << "IRA: There is no role assigned for robot: " << robotId.get()->getRaw() << std::endl;
        AlicaEngine::abort(ss.str());
        return nullptr;
    }
}

void IRoleAssignment::setCommunication(const IAlicaCommunication* communication)
{
    this->communication = communication;
}

} /* namespace alica */
