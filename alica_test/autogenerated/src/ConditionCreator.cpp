#include "ConditionCreator.h"

#include  "Plans/PlanThree1407153663917.h"

#include  "Plans/GoalPlan1402488870347.h"

#include  "Plans/MultiAgentTestMaster1413200842973.h"

#include  "Plans/MultiAgentTestPlan1413200862180.h"

#include  "Plans/SimpleTestPlan1412252439925.h"

#include  "Plans/PlanOne1407153611768.h"

#include  "Plans/AttackPlan1402488634525.h"

#include  "Plans/PlanTwo1407153645238.h"

#include  "Plans/MasterPlanTaskAssignment1407152758497.h"

#include  "Plans/PlanFive1407153703092.h"

#include  "Plans/Tackle1402489318663.h"

#include  "Plans/Defend1402488893641.h"

#include  "Plans/PlanFour1407153683051.h"

#include  "Plans/MidFieldPlayPlan1402488770050.h"

#include  "Plans/MasterPlan1402488437260.h"

using namespace alicaAutogenerated;
namespace alica
{

ConditionCreator::ConditionCreator()
{
}
ConditionCreator::~ConditionCreator()
{
}

shared_ptr<BasicCondition> ConditionCreator::createConditions(long conditionConfId)
{
  switch (conditionConfId)
  {

    case 1402489131988:

      return make_shared<PreCondition1402489131988>();

      break;

    case 1403773741874:

      return make_shared<RunTimeCondition1403773741874>();

      break;

    case 1402489174338:

      return make_shared<TransitionCondition1402489174338>();

      break;

    case 1402489206278:

      return make_shared<TransitionCondition1402489206278>();

      break;

    case 1402489218027:

      return make_shared<TransitionCondition1402489218027>();

      break;

    case 1413201227586:

      return make_shared<TransitionCondition1413201227586>();

      break;

    case 1413201389955:

      return make_shared<TransitionCondition1413201389955>();

      break;

    case 1413201052549:

      return make_shared<TransitionCondition1413201052549>();

      break;

    case 1413201367990:

      return make_shared<TransitionCondition1413201367990>();

      break;

    case 1413201370590:

      return make_shared<TransitionCondition1413201370590>();

      break;

    case 1412781693884:

      return make_shared<RunTimeCondition1412781693884>();

      break;

    case 1412781707952:

      return make_shared<PreCondition1412781707952>();

      break;

    case 1412761926856:

      return make_shared<TransitionCondition1412761926856>();

      break;

    case 1402489460549:

      return make_shared<TransitionCondition1402489460549>();

      break;

    case 1402489462088:

      return make_shared<TransitionCondition1402489462088>();

      break;

    case 1402488991641:

      return make_shared<TransitionCondition1402488991641>();

      break;

    case 1402488993122:

      return make_shared<TransitionCondition1402488993122>();

      break;

    case 1402489065962:

      return make_shared<TransitionCondition1402489065962>();

      break;

    case 1402489073613:

      return make_shared<TransitionCondition1402489073613>();

      break;

    case 1402489260911:

      return make_shared<RunTimeCondition1402489260911>();

      break;

    case 1402489258509:

      return make_shared<TransitionCondition1402489258509>();

      break;

    case 1402489278408:

      return make_shared<TransitionCondition1402489278408>();

      break;

    case 1402500844446:

      return make_shared<TransitionCondition1402500844446>();

      break;

    case 1402488519140:

      return make_shared<TransitionCondition1402488519140>();

      break;

    case 1402488520968:

      return make_shared<TransitionCondition1402488520968>();

      break;

    case 1402488558741:

      return make_shared<TransitionCondition1402488558741>();

      break;

    case 1409218319990:

      return make_shared<TransitionCondition1409218319990>();

      break;

    default:
      cerr << "ConditionCreator: Unknown condition id requested: " << conditionConfId << endl;
      throw new exception();
      break;
  }
}
}
