#ifndef AttackPlan_H_
#define AttackPlan_H_

#include "engine/BasicCondition.h"
#include "engine/BasicUtilityFunction.h"
#include "engine/UtilityFunction.h"
#include "engine/DefaultUtilityFunction.h"
/*PROTECTED REGION ID(incl1402488634525) ENABLED START*/
//Add inlcudes here
/*PROTECTED REGION END*/
using namespace alica;

namespace alicaAutogenerated
{
/*PROTECTED REGION ID(meth1402488634525) ENABLED START*/
//Add other things here
/*PROTECTED REGION END*/
class UtilityFunction1402488634525 : public BasicUtilityFunction
{
  shared_ptr<UtilityFunction> getUtilityFunction(Plan* plan);
};

class TransitionCondition1402489460549 : public BasicCondition
{
  bool evaluate(shared_ptr<RunningPlan> rp);
};

class TransitionCondition1402489462088 : public BasicCondition
{
  bool evaluate(shared_ptr<RunningPlan> rp);
};

} /* namespace alica */

#endif