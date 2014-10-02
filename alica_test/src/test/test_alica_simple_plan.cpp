#include <gtest/gtest.h>
#include <engine/AlicaEngine.h>
#include <engine/IAlicaClock.h>
#include "TestBehaviourCreator.h"
#include "engine/model/Behaviour.h"
#include "engine/PlanRepository.h"
#include <clock/AlicaROSClock.h>
#include  "engine/DefaultUtilityFunction.h"
#include "engine/model/Plan.h"

class AlicaSimplePlan : public ::testing::Test
{
protected:
	supplementary::SystemConfig* sc;
	alica::AlicaEngine* ae;
	virtual void SetUp()
	{
		// determine the path to the test config
		string path = supplementary::FileSystem::getSelfPath();
		int place = path.rfind("devel");
		path = path.substr(0, place);
		path = path + "src/alica/alica_test/src/test";

		// bring up the SystemConfig with the corresponding path
		sc = supplementary::SystemConfig::getInstance();
		sc->setRootPath(path);
		sc->setConfigPath(path + "/etc");

		// setup the engine
		ae = alica::AlicaEngine::getInstance();
	}

	virtual void TearDown()
	{
		ae->shutdown();
		sc->shutdown();
	}
};
/**
 * Tests whether it is possible to run a behaviour in a primitive plan.
 */
TEST_F(AlicaSimplePlan, runBehaviourInSimplePlan)
{
	alica::TestBehaviourCreator* bc = new alica::TestBehaviourCreator();
	ae->setIAlicaClock(new alicaRosProxy::AlicaROSClock());
	EXPECT_TRUE(ae->init(bc, "Roleset", "SimpleTestPlan", ".", false))
			<< "Unable to initialise the Alica Engine!";

	ae->start();

	sleep(5);
	delete bc;
}
