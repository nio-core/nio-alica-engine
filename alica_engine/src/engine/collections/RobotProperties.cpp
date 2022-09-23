#include "engine/PlanRepository.h"

#include "engine/AlicaEngine.h"
#include "engine/collections/RobotProperties.h"
//#include "engine/teammanager/TeamManager.h"
#include "model/Characteristic.h"

namespace alica
{

RobotProperties::RobotProperties() {
    std::cout << "RobotProperties Constructor" << std::endl;
}

RobotProperties::RobotProperties(const AlicaEngine* engine, const std::string& name)
:_ae(engine), 
 _name(name)
{
    std::cout << "RobotProperties Constructor 2    name:" << name <<   std::endl;
    readFromConfig(engine, name);
    //extractAgentCharacteristics( name,  essentials::SystemConfig::getInstance());
}


 essentials::IdentifierConstPtr RobotProperties::extractID(std::string& name,  essentials::SystemConfig* sc) const {
        std::cout << "\033[0;34m" << "RP:: ---------------------------" << "\033[0m" << std::endl;
        std::cout << "\033[0;34m" << "RP:: Extract ID for " << name << "\033[0m" << std::endl;
        std::cout << "\033[0;34m" << "RP:: ---------------------------" << "\033[0m" << std::endl;
        std::string id = (*sc)["Globals"]->tryGet<std::string>("NO_ID_SPECIFIED","Globals", "Team", name.c_str(), "ID", NULL);
        //std::cout << "RP: extractID for " << name << "  id:" << id << std::endl;

        if (!id.empty() && id.compare("NO_ID_SPECIFIED") != 0){
            essentials::IdentifierConstPtr idPtr = _ae->getId(id);
            std::cout << "\033[0;34m" << "RP:: ---------------------------" << "\033[0m" << std::endl;
            std::cout << "\033[0;34m" << "RP:: Use ID " << id << "  for " << name <<  "  " << idPtr << "\033[0m" << std::endl;
            std::cout << "\033[0;34m" << "RP:: ---------------------------" << "\033[0m" << std::endl;
            return _ae->getId(id);
        } else {
            
            if (_ae == nullptr)
                return nullptr;
            essentials::IdentifierConstPtr idPtr = _ae->getId(name);
            std::cout << "\033[0;34m" << "RP:: ---------------------------" << "\033[0m" << std::endl;
            std::cout << "\033[0;34m" << "RP:: Get ID " << idPtr->hash() << "  for " << name << "\033[0m" << std::endl;
            std::cout << "\033[0;34m" << "RP:: ---------------------------" << "\033[0m" << std::endl;
            return idPtr;
        }

    }

/**
 * Reads the default role as well as the capabilities and characteristics from the Globals.conf.
 */
void RobotProperties::readFromConfig(const AlicaEngine* engine, const std::string& name)
{
    std::shared_ptr<std::vector<std::string>> characteristics;
    essentials::SystemConfig* sc = essentials::SystemConfig::getInstance();
    std::shared_ptr<std::vector<std::string>> agentNames = (*sc)["Globals"]->getSections("Globals.Team", NULL);
    std::cout << "RP: reading from config ... (team size " << agentNames->size() << ")" << std::endl;

    for (const std::string& agentName : * agentNames) {
        std::cout << "RP: find existing agent configuration  name:" << name << " ==  name:" << agentName << std::endl;

        if (agentName.compare(name.c_str()) == 0) {
            characteristics = (*sc)["Globals"]->getNames("Globals", "Team", name.c_str(), NULL);
                std::cout << "RP: find " << characteristics->size() << " characteristics "  << std::endl;
                for (const std::string& s : *characteristics) {
                
                    if (s.compare("ID") == 0 || s.compare("DefaultRole") == 0) {
                        continue;
                    }
                    std::string key = s;
                    std::string kvalue = (*sc)["Globals"]->get<std::string>("Globals", "Team", name.c_str(), s.c_str(), NULL);
            //        for (const Capability* cap : engine->getPlanRepository()->getCapabilities()) {
            //            if (cap->getName().compare(key) == 0) {
            //                for (const CapValue* val : cap->getCapValues()) {
            //                    // transform(kvalue.begin(), kvalue.end(), kvalue.begin(), ::tolower);
            //                    if (val->getName().compare(kvalue) == 0) {
            //                        _characteristics.emplace(key, std::unique_ptr<const Characteristic>(new Characteristic(cap, val)));
            //                    }
            //                }
            //            }
            //        }
                }
            _defaultRole = (*sc)["Globals"]->tryGet<std::string>("NO_ROLE_SPECIFIED", "Globals", "Team", name.c_str(), "DefaultRole", NULL);
            break;
        } 
    }   

    if (!characteristics)  {
        std::cout << "RP: no exisiting agent configuration " << std::endl;
        extractAgentCharacteristics( name,  sc);
    } 
   std::cout << "RP: reading from conifg finished " << std::endl;
}

void RobotProperties::extractAgentCharacteristics(const std::string& name, essentials::SystemConfig* sc) {
            // characteristics = (*sc)["Globals"]->getSections("Globals.Characteristics", NULL);
        // std::cout << "RP: find " << characteristics->size() << " characteristics "  << std::endl;
        //   for (const std::string& s : *characteristics) {
        //     if (s.compare("collectionType") == 0) {
        //         continue;
        //     }
        //     std::string key = s;
        //     std::string kvalue = (*sc)["Globals"]->get<std::string>("Globals", "Characteristics", s.c_str(), NULL);
        // }
        // _defaultRole = "NO_ROLE_SPECIFIED";
        std::vector<std::string> characteristics = *(*sc)["Globals"]->getNames("Globals", "Characteristics", NULL);
        std::cout << "RP: find " << characteristics.size() << " characteristics "  << std::endl;

        for (std::string& key : characteristics) {
            std::cout << "RP:: extract agent characteristic " << key << std::endl;
            
            if (key.compare("ID") == 0 || key.compare("defaultRole") == 0 || key.compare("collectionType") == 0) {
                continue;
            }

            std::string value = (*sc)["Globals"]->get<std::string>("Globals", "Characteristics", key.c_str(), NULL);
            
            Characteristic* characteristic = new Characteristic();
            characteristic->setName(key);
            characteristic->setValue(value);
            _characteristics.emplace(key, characteristic);

            std::cout << "RP:: characteristic " << key << " " << value << " " << std::endl;
        }
    }

RobotProperties::~RobotProperties() {}

} /* namespace alica */
