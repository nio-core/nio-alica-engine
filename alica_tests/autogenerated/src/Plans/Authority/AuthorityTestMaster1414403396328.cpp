#include "Plans/Authority/AuthorityTestMaster1414403396328.h"
using namespace alica;
/*PROTECTED REGION ID(eph1414403396328) ENABLED START*/  // Add additional using directives here
#include <memory>
using namespace std;
/*PROTECTED REGION END*/
namespace alicaAutogenerated {
// Plan:AuthorityTestMaster

/* generated comment

 Task: DefaultTask  -> EntryPoint-ID: 1414403396331

 */
shared_ptr<UtilityFunction> UtilityFunction1414403396328::getUtilityFunction(Plan* plan) {
    /*PROTECTED REGION ID(1414403396328) ENABLED START*/

    shared_ptr<UtilityFunction> defaultFunction = make_shared<DefaultUtilityFunction>(plan);
    return defaultFunction;

    /*PROTECTED REGION END*/
}

// State: testState in Plan: AuthorityTestMaster

// State: Init in Plan: AuthorityTestMaster

/*
 *
 * Transition:
 *   - Name: , ConditionString: , Comment :
 *
 * Plans in State:
 *
 * Tasks:
 *   - DefaultTask (1225112227903) (Entrypoint: 1414403396331)
 *
 * States:
 *   - testState (1414403396329)
 *   - Init (1414403820806)
 *
 * Vars:
 */
bool TransitionCondition1414403842622::evaluate(shared_ptr<RunningPlan> rp) {
    /*PROTECTED REGION ID(1414403840950) ENABLED START*/
    return true;
    /*PROTECTED REGION END*/
}

}  // namespace alicaAutogenerated
