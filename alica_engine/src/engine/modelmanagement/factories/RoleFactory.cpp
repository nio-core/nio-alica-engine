#include "engine/modelmanagement/factories/RoleFactory.h"
#include "engine/modelmanagement/Strings.h"
#include "engine/model/Role.h"

#include <model/Characteristic.h>

namespace alica
{
    Role* RoleFactory::create(const YAML::Node& roleNode, RoleSet* roleSet)
    {
        Role* role = new Role();
        Factory::setAttributes(roleNode, role);
        Factory::storeElement(role, alica::Strings::role);
        role->_roleSet = roleSet;

        if (Factory::isValid(roleNode[alica::Strings::taskPriorities])) {
            const YAML::Node& taskPriorities = roleNode[alica::Strings::taskPriorities];
            for (YAML::const_iterator it = taskPriorities.begin(); it != taskPriorities.end(); ++it) {
                Factory::roleTaskReferences.push_back(std::make_tuple(role->getId(), Factory::getReferencedId((*it).first), ((*it).second).as<double>()));
            }
        }

        if (Factory::isValid(roleNode[alica::Strings::characteristics])) {
            //std::cout << "\nRF:: characteristics for role " << role->getName() << std::endl;
            const YAML::Node& roleCharacteristics = roleNode[alica::Strings::characteristics];
            //std::cout << "RF:: characteristics size:" << roleCharacteristics.size() << std::endl;

            for (YAML::const_iterator it = roleCharacteristics.begin(); it != roleCharacteristics.end(); ++it) {
                const YAML::Node& characteristicNode = (*it);
                Characteristic * characteristic = new Characteristic(); 
                Factory::setAttributes(characteristicNode, characteristic);
                characteristic->setValue(getValue<std::string>(characteristicNode, alica::Strings::value));
                characteristic->setWeight(getValue<double>(characteristicNode, alica::Strings::weight));
                //std::cout << "RF:: characteristic " << characteristic->getName() << " " << characteristic->getValue() << " " << characteristic->getWeight() << std::endl;
                //Factory::storeElement(characteristic, alica::Strings::characteristic);
                role->addCharacteristic(characteristic->getName(), characteristic);
            }
        }

        return role;
    }

    void RoleFactory::attachReferences() {
        // roleTaskReferences
        for (std::tuple<int64_t, int64_t, double> triple: Factory::roleTaskReferences) {
            Role* role = (Role*) Factory::getElement(std::get<0>(triple));
            Task* task = (Task*) Factory::getElement(std::get<1>(triple));
            role->_taskPriorities.emplace(task, std::get<2>(triple));
        }
        Factory::planTypePlanReferences.clear();
    }
} // namespace alica
