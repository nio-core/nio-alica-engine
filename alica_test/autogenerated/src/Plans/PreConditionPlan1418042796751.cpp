#include "Plans/PreConditionPlan1418042796751.h"
using namespace alica;
/*PROTECTED REGION ID(eph1418042796751) ENABLED START*/  // Add additional using directives here
#include <DistXContourTest.h>
#include "engine/USummand.h"
#include "TestConstantValueSummand.h"
#include "TestWorldModel.h"
/*PROTECTED REGION END*/
namespace alicaAutogenerated {
// Plan:PreConditionPlan

// Check of PreCondition - (Name): NewPreCondition, (ConditionString): Test , (Comment) :

/*
 * Available Vars:
 */
bool PreCondition1418042929966::evaluate(shared_ptr<RunningPlan> rp) {
    /*PROTECTED REGION ID(1418042929966) ENABLED START*/
    //--> "PreCondition:1418042929966  not implemented";
    //    	return true;
    return alicaTests::TestWorldModel::getOne()->isPreCondition1418042929966();
    /*PROTECTED REGION END*/
}

/* generated comment

 Task: DefaultTask  -> EntryPoint-ID: 1418042796753

 */
shared_ptr<UtilityFunction> UtilityFunction1418042796751::getUtilityFunction(Plan* plan) {
    /*PROTECTED REGION ID(1418042796751) ENABLED START*/
    TestConstantValueSummand* us = new TestConstantValueSummand(0.5, "PreConditionPlan", 1, 1.0);
    list<USummand*> utilSummands;
    utilSummands.push_back(us);
    shared_ptr<UtilityFunction> function =
            make_shared<UtilityFunction>("PreConitionTestUtilSummand", utilSummands, 0.5, 0.1, plan);

    return function;
    /*PROTECTED REGION END*/
}

// State: PreConditionTest in Plan: PreConditionPlan

}  // namespace alicaAutogenerated
