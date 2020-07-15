#define BEH_DEBUG

#include "engine/BasicBehaviour.h"
#include "engine/AlicaEngine.h"
#include "engine/Assignment.h"
#include "engine/IAlicaCommunication.h"
#include "engine/PlanBase.h"
#include "engine/TeamObserver.h"
#include "engine/model/Behaviour.h"
#include "engine/model/EntryPoint.h"
#include "engine/model/Plan.h"
#include "engine/model/Variable.h"
#include "engine/teammanager/TeamManager.h"

#include <alica_common_config/debug_output.h>

#include <essentials/ITrigger.h>

#include <assert.h>
#include <iostream>

namespace alica
{
/**
 * Basic constructor. Initialises the timer. Should only be called from the constructor of inheriting classes.
 * If using eventTrigger set behaviourTrigger and register runCV
 * @param name The name of the behaviour
 */
BasicBehaviour::BasicBehaviour(const std::string& name)
        : _name(name)
        , _behaviourConfiguration(nullptr)
        , _engine(nullptr)
        , _failure(false)
        , _success(false)
        , _callInit(true)
        , _started(true)
        , _behaviour(nullptr)
        , _msInterval(100)
        , _msDelayedStart(0)
        , _running(false)
        , _contextInRun(nullptr)
        , _behaviourTrigger(nullptr)
        , _runThread(nullptr)
        , _context(nullptr)
        , _loop(false)
        , _finished(false)
{
}

void BasicBehaviour::terminate()
{
    _started = false;
    _runCV.notify_all();
    if (_runThread) {
        _runThread->join();
        delete _runThread;
    }
}

bool BasicBehaviour::isRunningInContext(const RunningPlan* rp) const
{
    // we run in the context of rp if rp is the context in run or the context in run is null and context is rp
    RunningPlan* curInRun;
    curInRun = _contextInRun;
    return curInRun == rp || (curInRun == nullptr && _context == rp && _started && _running);
}

void BasicBehaviour::setBehaviourConfiguration(const BehaviourConfiguration* behConf) {
    assert(_behaviourConfiguration == nullptr);
    _behaviourConfiguration = behConf;
}

void BasicBehaviour::setBehaviour(const Behaviour* beh)
{
    assert(_behaviour == nullptr);
    _behaviour = beh;
    if (_behaviour->isEventDriven()) {
        _runThread = new std::thread(&BasicBehaviour::runInternalTriggered, this);
    } else {
        _runThread = new std::thread(&BasicBehaviour::runInternalTimed, this);
    }
}

/**
 * Convenience method to obtain the robot's own id.
 * @return the own robot id
 */
essentials::IdentifierConstPtr BasicBehaviour::getOwnId() const
{
    return _engine->getTeamManager()->getLocalAgentID();
}

/**
 * Stops the execution of this BasicBehaviour.
 */
bool BasicBehaviour::stop()
{
    _running = false;
    _success = false;
    _failure = false;
    return true;
}

/**
 * Starts the execution of this BasicBehaviour.
 */
bool BasicBehaviour::start()
{
    _callInit = true;
    _running = true;
    if (!_behaviour->isEventDriven()) {
        _runCV.notify_all();
    }
    return true;
}

void BasicBehaviour::setSuccess()
{
    if (!_success) {
        _success = true;
        _engine->getPlanBase()->addFastPathEvent(_context);
    }
}

bool BasicBehaviour::isSuccess() const
{
    return _success && !_callInit;
}

void BasicBehaviour::setFailure()
{
    if (!_failure) {
        _failure = true;
        _engine->getPlanBase()->addFastPathEvent(_context);
    }
}

bool BasicBehaviour::isFailure() const
{
    return _failure && !_callInit;
}

void BasicBehaviour::setTrigger(essentials::ITrigger* trigger)
{
    _behaviourTrigger = trigger;
    _behaviourTrigger->registerCV(&_runCV);
}

void BasicBehaviour::initInternal()
{
    _success = false;
    _failure = false;
    _callInit = false;
    try {
        initialiseParameters();
    } catch (const std::exception& e) {
        ALICA_ERROR_MSG("BB: Exception in Behaviour-INIT of: " << getName() << std::endl << e.what());
    }
}

void BasicBehaviour::runInternalTimed()
{
    std::cout << "\033[0;36m" << "BB: runInternalTimed started" << "\033[0m" << std::endl;
    while (_started) {
        {
            std::unique_lock<std::mutex> lck(_runLoopMutex);
            if (!_running) {
                if (_contextInRun) {
                    onTermination();
                }
                _contextInRun = nullptr;
                std::cout << "\033[0;36m" << "BB: " << _name << " wait for signal to run" << "\033[0m" << std::endl;
                _runCV.wait(lck, [this] { return _running || !_started; }); // wait for signal to run
                std::cout << "\033[0;36m" << "BB: " << _name << " get signal" << "\033[0m" << std::endl;

            }
            _contextInRun = _context;
        }
        if (!_started) {
            _contextInRun = nullptr;
            return;
        }
        if (_callInit) {
            if (_msDelayedStart > std::chrono::milliseconds(0)) {
                std::this_thread::sleep_for(_msDelayedStart);
            }
            initInternal();
        }
        //std::chrono::system_clock::time_point 
        std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
        try {
            std::cout << "\033[0;36m" << "BB: call run method" << "\033[0m" << std::endl;
            run(nullptr);
        } catch (const std::exception& e) {
            std::string err = std::string("Exception caught:  ") + getName() + std::string(" - ") + std::string(e.what());
            sendLogMessage(4, err);
        }
        std::chrono::duration<float, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
        ALICA_WARNING_MSG_IF(
                duration > _msInterval + std::chrono::microseconds(100), "BB: Behaviour " << _name << "exceeded runtime:  " << duration.count() << "ms!");
        if (duration < _msInterval) {
            std::this_thread::sleep_for(_msInterval - duration);
        }
    }
}

void BasicBehaviour::runInternalTriggered()
{
    while (_started) {
        {
            if (_contextInRun) {
                onTermination();
            }
            std::cout << "\033[0;36m" << "BB: " << _name << " get signal" << "\033[0m" << std::endl;
            std::unique_lock<std::mutex> lck(_runLoopMutex);
            _contextInRun = nullptr;
            _runCV.wait(lck, [this] { return !_started || (_behaviourTrigger->isNotifyCalled(&_runCV) && _running); });
            _contextInRun = _started ? _context : nullptr;
        }
        if (!_started) {
            return;
        }
        if (_callInit) {
            initInternal();
        }

        try {
            run(static_cast<void*>(_behaviourTrigger));
        } catch (const std::exception& e) {
            std::string err = std::string("Exception caught:  ") + getName() + std::string(" - ") + std::string(e.what());
            sendLogMessage(4, err);
        }
        _behaviourTrigger->setNotifyCalled(false, &_runCV);
    }
}

void BasicBehaviour::sendLogMessage(int level, const std::string& message) const
{
    _engine->getCommunicator()->sendLogMessage(level, message);
}

bool BasicBehaviour::getParameter(const std::string& key, std::string& valueOut) const
{
    const auto& entry = this->_behaviour->getParameters().find(key);
    if (entry != this->_behaviour->getParameters().end()) {
        valueOut = entry->second;
        return true;
    } else {
        return false;
    }
}

} /* namespace alica */
