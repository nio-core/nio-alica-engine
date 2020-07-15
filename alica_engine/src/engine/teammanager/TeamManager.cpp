#include "engine/teammanager/TeamManager.h"

#include "engine/AlicaEngine.h"
#include "engine/collections/RobotProperties.h"

#include <SystemConfig.h>
#include <iostream>
#include <utility>

namespace alica
{

TeamManager::TeamManager(AlicaEngine* engine, bool useConfigForTeam = true)
        : localAgent(nullptr)
        , useConfigForTeam(useConfigForTeam)
        , engine(engine)
{
}

TeamManager::~TeamManager()
{
    for (auto& agentEntry : _agents) {
        delete agentEntry.second;
    }
}

void TeamManager::init()
{
    essentials::SystemConfig* sc =essentials::SystemConfig::getInstance();
    this->teamTimeOut = AlicaTime::milliseconds((*sc)["Alica"]->get<unsigned long>("Alica.TeamTimeOut", NULL));

    if (useConfigForTeam) {
        this->readTeamFromConfig(sc);
    }
}

void TeamManager::readTeamFromConfig(essentials::SystemConfig* sc)
{
    std::string localAgentName = this->engine->getRobotName();
    //std::string localAgentName = "Service";
    std::shared_ptr<std::vector<std::string>> agentNames = (*sc)["Globals"]->getSections("Globals.Team", NULL);

    Agent* agent;
    bool foundSelf = false;
    for (const std::string& agentName : *agentNames) {
        std::cout << "TM: " << agentName << std::endl;
        std::string id = (*sc)["Globals"]->tryGet<std::string>("-1", "Globals", "Team", agentName.c_str(), "ID", NULL);
        std::cout << "TM: " << id << std::endl;
        
        if (id != "-1") {
            agent = new Agent(this->engine, this->teamTimeOut, this->engine->getId(id), agentName);
            std::cout << "TM: static agent id " << " with local_agent_name:" << localAgentName << " agent_name:" << agentName << std::endl; 
        } 
        else {
             essentials::IdentifierConstPtr newID = essentials::IDManager().generateID(18);
             std::cout << "TM: generated id " << newID << std::endl; 
             agent = new Agent(this->engine, this->teamTimeOut, this->engine->getId(localAgentName), localAgentName);
            //  agent = new Agent(this->engine, this->teamTimeOut, newID, localAgentName);
             std::cout << "TM: dynamic agent id " << agent->getId() << " with local_agent_name:" << localAgentName << " agent_name:" << agentName << std::endl; 
        }
        
        if (!foundSelf && (agentName.compare(localAgentName) == 0 || id == "-1" )) {
            foundSelf = true;
            this->localAgent = agent;
            this->localAgent->setLocal(true);
        } else {
            for (auto& agentEntry : _agents) {
                if (*(agentEntry.first) == *(agent->getId())) {
                    AlicaEngine::abort("TM: Two robots with the same ID in Globals.conf. ID: ", agent->getId());
                }
            }
        }
        _agents.emplace(agent->getId(), agent);
    }
    if (!foundSelf) {
        AlicaEngine::abort("TM: Could not find own agent name in Globals Id = ", localAgentName);
    }

    if ((*sc)["Alica"]->get<bool>("Alica.TeamBlackList.InitiallyFull", NULL)) {
        for (auto& agentEntry : _agents) {
            agentEntry.second->setIgnored(true);
        }
    }
}

ActiveAgentIdView TeamManager::getActiveAgentIds() const
{
    return ActiveAgentIdView(_agents);
}

ActiveAgentView TeamManager::getActiveAgents() const
{
    return ActiveAgentView(_agents);
}

int TeamManager::getTeamSize() const
{
    int teamSize = 0;
    for (auto& agentEntry : _agents) {
        if (agentEntry.second->isActive()) {
            ++teamSize;
        }
    }
    return teamSize;
}

const Agent* TeamManager::getAgentByID(essentials::IdentifierConstPtr agentId) const
{
    auto agentEntry = _agents.find(agentId);
    if (agentEntry != _agents.end()) {
        return agentEntry->second;
    } else {
        return nullptr;
    }
}

essentials::IdentifierConstPtr TeamManager::getLocalAgentID() const
{
    return this->localAgent->getId();
}

void TeamManager::setTimeLastMsgReceived(essentials::IdentifierConstPtr agentId, AlicaTime timeLastMsgReceived)
{
    auto mapIter = _agents.find(agentId);
    if (mapIter != _agents.end()) {
        mapIter->second->setTimeLastMsgReceived(timeLastMsgReceived);
    } else {
        // TODO alex robot properties protokoll anstoßen
        Agent* agent = new Agent(this->engine, this->teamTimeOut, agentId);
        agent->setTimeLastMsgReceived(timeLastMsgReceived);
        _agents.emplace(agentId, agent);
    }
}

bool TeamManager::isAgentActive(essentials::IdentifierConstPtr agentId) const
{
    auto agentEntry = _agents.find(agentId);
    if (agentEntry != _agents.end()) {
        return agentEntry->second->isActive();
    } else {
        return false;
    }
}

/**
 * Checks if an agent is ignored
 * @param agentId an essentials::AgentID identifying the agent
 */
bool TeamManager::isAgentIgnored(essentials::IdentifierConstPtr agentId) const
{
    auto agentEntry = _agents.find(agentId);
    if (agentEntry != _agents.end()) {
        return agentEntry->second->isIgnored();
    } else {
        return false;
    }
}

void TeamManager::setAgentIgnored(essentials::IdentifierConstPtr agentId, const bool ignored) const
{
    auto agentEntry = _agents.find(agentId);
    if (agentEntry != _agents.end()) {
        agentEntry->second->setIgnored(ignored);
    }
}

bool TeamManager::setSuccess(essentials::IdentifierConstPtr agentId, const AbstractPlan* plan, const EntryPoint* entryPoint)
{
    auto agentEntry = _agents.find(agentId);
    if (agentEntry != _agents.end()) {
        agentEntry->second->setSuccess(plan, entryPoint);
        return true;
    }
    return false;
}

bool TeamManager::setSuccessMarks(essentials::IdentifierConstPtr agentId, const IdGrp& suceededEps)
{
    auto agentEntry = _agents.find(agentId);
    if (agentEntry != _agents.end()) {
        agentEntry->second->setSuccessMarks(suceededEps);
        return true;
    }
    return false;
}

const DomainVariable* TeamManager::getDomainVariable(essentials::IdentifierConstPtr agentId, const std::string& sort) const
{
    auto agentEntry = _agents.find(agentId);
    if (agentEntry != _agents.end()) {
        return agentEntry->second->getDomainVariable(sort);
    }
    return nullptr;
}

} /* namespace alica */
