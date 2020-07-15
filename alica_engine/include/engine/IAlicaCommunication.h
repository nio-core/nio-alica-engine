#pragma once

#include <memory>
#include <string>

namespace alica
{
struct AlicaEngineInfo;
struct AllocationAuthorityInfo;
struct PlanTreeInfo;
struct SolverResult;
struct SyncTalk;
struct SyncReady;
class AlicaEngine;
class RoleSwitch;

class IAlicaCommunication
{
public:
    IAlicaCommunication(AlicaEngine* ae)
            : ae(ae){};
    virtual ~IAlicaCommunication() {}

    virtual void sendAllocationAuthority(const AllocationAuthorityInfo& aai) const = 0;
    virtual void sendAlicaEngineInfo(const AlicaEngineInfo& bi) const = 0;
    virtual void sendPlanTreeInfo(const PlanTreeInfo& pti) const = 0;
    virtual void sendRoleSwitch(const RoleSwitch& rs) const = 0;
    virtual void sendSyncReady(const SyncReady& sr) const = 0;
    virtual void sendSyncTalk(const SyncTalk& st) const = 0;
    virtual void sendSolverResult(const SolverResult& sr) const = 0;
    virtual void sendLogMessage(int level, const std::string& message) const {};

    virtual void tick(){};

    virtual void onSyncTalkReceived(std::shared_ptr<SyncTalk> st) const = 0;
    virtual void onSyncReadyReceived(std::shared_ptr<SyncReady> sr) const = 0;
    virtual void onAuthorityInfoReceived(const AllocationAuthorityInfo& aai) const = 0;
    virtual void onPlanTreeInfoReceived(std::shared_ptr<PlanTreeInfo> pti) const = 0;
    virtual void onSolverResult(const SolverResult& sr) const = 0;

    virtual void startCommunication() = 0;
    virtual void stopCommunication() = 0;

protected:
    AlicaEngine* ae;
};

} /* namespace alica */
