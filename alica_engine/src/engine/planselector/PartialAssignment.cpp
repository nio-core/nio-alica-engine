/*
 * PartialAssignment.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: Stefan Jakob
 */

#include <engine/planselector/PartialAssignment.h>

#include "engine/planselector/EpByTaskComparer.h"
#include "engine/planselector/DynCardinality.h"
#include "engine/collections/AssignmentCollection.h"
#include "engine/model/Plan.h"
#include "engine/model/EntryPoint.h"
#include "engine/collections/SuccessCollection.h"
#include "engine/model/Task.h"
#include "engine/SimplePlanTree.h"

namespace alica
{
	int PartialAssignment::maxCount = 10100;
	int PartialAssignment::maxEpsCount = 20;
	int PartialAssignment::curIndex = 0;
	vector<PartialAssignment*> PartialAssignment::daPAs = vector<PartialAssignment*>(maxCount);
	EpByTaskComparer PartialAssignment::epByTaskComparer = EpByTaskComparer();
	bool PartialAssignment::allowIdling = (*supplementary::SystemConfig::getInstance())["Alica"]->get<bool>("Alica.AllowIdling", NULL);
	EntryPoint* PartialAssignment::idleEP;

	int PartialAssignment::getHash()
	{
		return hash;
	}

	void PartialAssignment::setHash(int hash = 0)
	{
		this->hash = hash;
	}

	bool PartialAssignment::isHashCalculated()
	{
		return hashCalculated;
	}

	void PartialAssignment::setHashCalculated(bool hashCalculated)
	{
		this->hashCalculated = hashCalculated;
	}

	PartialAssignment::PartialAssignment()
	{
		this->utilFunc = nullptr;
		this->epSuccessMapping = nullptr;
		this->hashCalculated = false;
		this->plan = nullptr;
		this->epRobotsMapping = new AssignmentCollection(maxEpsCount);
		this->unAssignedRobots = vector<int>();
		this->dynCardinalities = vector<shared_ptr<DynCardinality>>(maxEpsCount);
		this->compareVal = PRECISION;
		for (int i = 0; i < maxEpsCount; i++)
		{
			this->dynCardinalities[i] = make_shared<DynCardinality>();
		}

	}

	PartialAssignment::~PartialAssignment()
	{
		delete epRobotsMapping;
	}

	AssignmentCollection* PartialAssignment::getEpRobotsMapping()
	{
		return epRobotsMapping;
	}

	Plan* PartialAssignment::getPlan()
	{
		return plan;
	}

	UtilityFunction* PartialAssignment::getUtilFunc()
	{
		return utilFunc;
	}

	shared_ptr<SuccessCollection> PartialAssignment::getEpSuccessMapping()
	{
		return epSuccessMapping;
	}

	vector<int>& PartialAssignment::getUnAssignedRobots()
	{
		return unAssignedRobots;
	}

	shared_ptr<vector<EntryPoint*> > PartialAssignment::getEntryPoints()
	{
		return this->epRobotsMapping->getEntryPoints();
	}

	void PartialAssignment::init()
	{
		// IDLE-EntryPoint
		idleEP = new EntryPoint();
		idleEP->setName("IDLE-ep");
		idleEP->setId(EntryPoint::IDLEID);
		idleEP->setMinCardinality(0);
		idleEP->setMaxCardinality(numeric_limits<int>::max());
		// Add IDLE-Task
		idleEP->setTask(new Task(true));
		idleEP->getTask()->setName("IDLE-TASK");
		idleEP->getTask()->setId(Task::IDLEID);

		for (int i = 0; i < maxCount; i++)
		{
			daPAs[i] = new PartialAssignment();
		}

	}

	void PartialAssignment::cleanUp()
	{
		for (int i = 0; i < maxCount; i++)
		{
			delete daPAs[i];
			daPAs[i] = nullptr;
		}
		delete idleEP->getTask();
		idleEP->setTask(nullptr);
		delete idleEP;
		idleEP = nullptr;
	}

	void PartialAssignment::clear()
	{
		this->min = 0.0;
		this->max = 1.0;
		this->compareVal = PRECISION;
		this->unAssignedRobots.clear();
		for (int i = 0; i < this->epRobotsMapping->getCount(); i++)
		{
			this->epRobotsMapping->getRobots()->at(i)->clear();
		}
		this->hashCalculated = false;
	}

	void PartialAssignment::reset()
	{
		curIndex = 0;
	}

	PartialAssignment* PartialAssignment::getNew(shared_ptr<vector<int> > robots, Plan* plan, shared_ptr<SuccessCollection> sucCol)
	{
		if (curIndex >= maxCount)
		{
			cerr << "max PA count reached!" << endl;
		}
		PartialAssignment* ret = daPAs[curIndex++];
		ret->clear();
		ret->robots = robots; // Should already be sorted! (look at TaskAssignment, or PlanSelector)
		ret->plan = plan;
		ret->utilFunc = plan->getUtilityFunction();
		ret->epSuccessMapping = sucCol;
		// Create EP-Array
		if (allowIdling)
		{
			ret->epRobotsMapping->setCount(plan->getEntryPoints().size() + 1);
			// Insert IDLE-EntryPoint
			ret->epRobotsMapping->getEntryPoints()->at(ret->epRobotsMapping->getCount() - 1) = idleEP;
		}
		else
		{
			ret->epRobotsMapping->setCount(plan->getEntryPoints().size());
		}
		// Insert plan entrypoints
		int i = 0;
		for (auto iter : plan->getEntryPoints())
		{
			ret->epRobotsMapping->getEntryPoints()->at(i++) = iter.second;
		}

		// Sort the entrypoint array
		if (allowIdling)
		{
			auto iter1 = ret->epRobotsMapping->getEntryPoints()->begin();
			auto iter2 = ret->epRobotsMapping->getEntryPoints()->begin();
			advance(iter2, ret->epRobotsMapping->getCount() - 1);
			sort(iter1, iter2, EpByTaskComparer::compareTo);
		}
		else
		{
			auto iter1 = ret->epRobotsMapping->getEntryPoints()->begin();
			auto iter2 = ret->epRobotsMapping->getEntryPoints()->begin();
			advance(iter2, ret->epRobotsMapping->getCount());
			sort(iter1, iter2, EpByTaskComparer::compareTo);
		}
		for (int i = 0; i < ret->epRobotsMapping->getCount(); i++)
		{
			ret->dynCardinalities[i]->setMin(ret->epRobotsMapping->getEntryPoints()->at(i)->getMinCardinality());
			ret->dynCardinalities[i]->setMax(ret->epRobotsMapping->getEntryPoints()->at(i)->getMaxCardinality());
			shared_ptr<list<int> > suc = sucCol->getRobots(ret->epRobotsMapping->getEntryPoints()->at(i));

			if (suc != nullptr)
			{
				ret->dynCardinalities[i]->setMin(ret->dynCardinalities[i]->getMin() - suc->size());
				ret->dynCardinalities[i]->setMax(ret->dynCardinalities[i]->getMax() - suc->size());
				if (ret->dynCardinalities[i]->getMin() < 0)
				{
					ret->dynCardinalities[i]->setMin(0);
				}
				if (ret->dynCardinalities[i]->getMax() < 0)
				{
					ret->dynCardinalities[i]->setMax(0);
				}

#ifdef SUCDEBUG
				cout << "SuccessCollection" << endl;
				cout << "EntryPoint: " << ret->epRobotsMapping->getEntryPoints()->at(i)->toString() << endl;
				cout << "DynMax: " << ret->dynCardinalities[i]->getMax() << endl;
				cout << "DynMin: " << ret->dynCardinalities[i]->getMin() << endl;
				cout << "SucCol: ";
				for (int j : (*suc))
				{
					cout << j << ", ";
				}
				cout << "-----------" << endl;
#endif
			}
		}

		// At the beginning all robots are unassigned
		for (int i : (*robots))
		{
			ret->unAssignedRobots.push_back(i);
		}
		return ret;
	}

	PartialAssignment* PartialAssignment::getNew(PartialAssignment* oldPA)
	{
		if (curIndex >= maxCount)
		{
			cerr << "max PA count reached!" << endl;
		}
		PartialAssignment* ret = daPAs[curIndex++];
		ret->clear();
		ret->min = oldPA->min;
		ret->max = oldPA->max;
		ret->plan = oldPA->plan;
		ret->robots = oldPA->robots;
		ret->utilFunc = oldPA->utilFunc;
		ret->epSuccessMapping = oldPA->epSuccessMapping;
		for (int i = 0; i < oldPA->unAssignedRobots.size(); i++)
		{
			ret->unAssignedRobots.push_back(oldPA->unAssignedRobots[i]);
		}

		ret->dynCardinalities = oldPA->dynCardinalities;

		shared_ptr<vector<shared_ptr<vector<int> > > > oldRobotLists = oldPA->epRobotsMapping->getRobots();

		for (int i = 0; i < oldPA->epRobotsMapping->getCount(); i++)
		{
			ret->epRobotsMapping->getEntryPoints()->at(i) = oldPA->epRobotsMapping->getEntryPoints()->at(i);
			for (int j = 0; j < oldRobotLists->at(i)->size(); j++)
			{
				ret->epRobotsMapping->getRobots()->at(i)->push_back(oldRobotLists->at(i).get()->at(j));
			}

		}

		ret->epRobotsMapping->setCount(oldPA->epRobotsMapping->getCount());
		return ret;

	}

	int PartialAssignment::getEntryPointCount()
	{
		return this->epRobotsMapping->getCount();
	}

	int PartialAssignment::totalRobotCount()
	{
		int c = 0;
		for (int i = 0; i < this->epRobotsMapping->getRobots()->size(); i++)
		{
			c += this->epRobotsMapping->getRobots()->at(i)->size();
		}
		return this->numUnAssignedRobots() + c;
	}

	shared_ptr<vector<int> > PartialAssignment::getRobotsWorking(EntryPoint* ep)
	{
		return this->epRobotsMapping->getRobots(ep);
	}

	shared_ptr<vector<int> > PartialAssignment::getRobotsWorking(long epid)
	{
		return this->epRobotsMapping->getRobotsById(epid);
	}

	shared_ptr<list<int> > PartialAssignment::getRobotsWorkingAndFinished(EntryPoint* ep)
	{
		shared_ptr<list<int> > ret = make_shared<list<int> >(list<int>());
		auto robots = this->epRobotsMapping->getRobots(ep);
		if (robots != nullptr)
		{
			for (auto iter : (*robots))
			{
				ret->push_back(iter);
			}
		}
		auto successes = this->epSuccessMapping->getRobots(ep);
		if (successes != nullptr)
		{
			for (auto iter : (*successes))
			{
				ret->push_back(iter);
			}
		}
		return ret;
	}

	shared_ptr<list<int> > PartialAssignment::getRobotsWorkingAndFinished(long epid)
	{
		shared_ptr<list<int> > ret = make_shared<list<int> >(list<int>());
		auto robots = this->epRobotsMapping->getRobotsById(epid);
		if (robots != nullptr)
		{
			for (auto iter : (*robots))
			{
				ret->push_back(iter);
			}
		}
		auto successes = this->epSuccessMapping->getRobotsById(epid);
		if (successes != nullptr)
		{
			for (auto iter : (*successes))
			{
				ret->push_back(iter);
			}
		}
		return ret;
	}

	shared_ptr<list<int> > PartialAssignment::getUniqueRobotsWorkingAndFinished(EntryPoint* ep)
	{
		shared_ptr<list<int> > ret = make_shared<list<int> >(list<int>());
		auto robots = this->epRobotsMapping->getRobots(ep);

		for (auto iter : (*robots))
		{
			ret->push_back(iter);
		}

		auto successes = this->epSuccessMapping->getRobots(ep);
		if (successes != nullptr)
		{
			for (auto iter : (*successes))
			{
				if (find(ret->begin(), ret->end(), iter) == ret->end())
				{
					ret->push_back(iter);
				}
			}
		}
		return ret;

	}

	/**
	 * If the robot has already assigned itself, this method updates the partial assignment accordingly
	 * @param A shared_ptr<SimplePlanTree>
	 * @param An int
	 * @return A bool
	 */
	bool PartialAssignment::addIfAlreadyAssigned(shared_ptr<SimplePlanTree> spt, int robot)
	{
		if (spt->getEntryPoint()->getPlan() == this->plan)
		{
			shared_ptr<vector<EntryPoint*> > eps = this->epRobotsMapping->getEntryPoints();
			EntryPoint* curEp;
			int max = this->epRobotsMapping->getCount();
			if (allowIdling)
			{
				max--;
			}
			for (int i = 0; i < max; ++i)
			{
				curEp = eps->at(i);
				if (spt->getEntryPoint()->getId() == curEp->getId())
				{
					if (!this->assignRobot(robot, i))
					{
						break;
					}
					//remove robot from "To-Add-List"
					auto iter = find(this->unAssignedRobots.begin(), this->unAssignedRobots.end(), robot);
					if (this->unAssignedRobots.erase(iter) == this->unAssignedRobots.end())
					{
						cerr << "PA: Tried to assign robot " << robot << ", but it was not UNassigned!" << endl;
						throw new exception;
					}
					//return true, because we are ready, when we found the robot here
					return true;
				}
			}
			return false;
		}
		// If there are children and we didnt find the robot until now, then go on recursive
		else if (spt->getChildren().size() > 0)
		{
			for (auto sptChild : spt->getChildren())
			{
				if (this->addIfAlreadyAssigned(sptChild, robot))
				{
					return true;
				}
			}
		}
		// Did not find the robot in any relevant entry point
		return false;
	}

	/**
	 * Assigns the robot into the datastructures according to the given index.
	 * @return True, when it was possible to assign the robot. False, otherwise.
	 */
	bool PartialAssignment::assignRobot(int robot, int index)
	{
		if (this->dynCardinalities[index]->getMax() > 0)
		{
			this->epRobotsMapping->getRobots()->at(index)->push_back(robot);
			if (this->dynCardinalities[index]->getMin() > 0)
			{
				this->dynCardinalities[index]->setMin(this->dynCardinalities[index]->getMin() - 1);
			}
			if (this->dynCardinalities[index]->getMax() != INFINITY)
			{
				this->dynCardinalities[index]->setMax(this->dynCardinalities[index]->getMax() - 1);
			}
			return true;
		}
		return false;
	}

	shared_ptr<list<PartialAssignment*> > PartialAssignment::expand()
	{
		shared_ptr<list<PartialAssignment*> > newPas = make_shared<list<PartialAssignment*> >();
		if (this->unAssignedRobots.size() == 0)
		{
			// No robot left to expand
			return newPas;
		}
		// Robot which should be assigned next
		int robot = this->unAssignedRobots[0];
		this->unAssignedRobots.erase(this->unAssignedRobots.begin());
		PartialAssignment* newPa;
		for (int i = 0; i < this->epRobotsMapping->getCount(); ++i)
		{
			if (this->dynCardinalities[i]->getMax() > 0)
			{
				// Update the cardinalities and assign the robot
				newPa = PartialAssignment::getNew(this);
				newPa->assignRobot(robot, i);
				newPas->push_back(newPa);
			}
		}
		return newPas;
	}

	/**
	 * Checks whether the current assignment is valid
	 */
	bool PartialAssignment::isValid()
	{
		int min = 0;
		for (int i = 0; i < this->epRobotsMapping->getCount(); ++i)
		{
			min += dynCardinalities[i]->getMin();
		}
		return min <= this->numUnAssignedRobots();
	}

	/**
	 * Checks if this PartialAssignment is a complete Assignment.
	 * @return True, if it is, false otherwise.
	 */
	bool PartialAssignment::isGoal()
	{
		// There should be no unassigned robots anymore
		if (this->unAssignedRobots.size() > 0)
		{
			return false;
		}
		// Every EntryPoint should be satisfied according to his minCar
		for (int i = 0; i < this->epRobotsMapping->getCount(); ++i)
		{
			if (this->dynCardinalities[i]->getMin() != 0)
			{
				return false;
			}
		}
		return true;
	}

	/**
	 * Compares this PartialAssignment with another one.
	 * @return false if it is the same object or they have the same utility, assignment and plan id
	 * false if this PartialAssignment has a higher utility, or plan id
	 * Difference between Hashcodes, if they have the same utility and plan id
	 * true if the other PartialAssignment has a higher utility, or plan id
	 */
	bool PartialAssignment::compareTo(PartialAssignment* thisPa, PartialAssignment* newPa)
	{
		//TODO has perhaps to be changed
		// 0 , -1 = false
		// 1 true
		if (&thisPa == &newPa) // Same reference -> same object
		{
			return false;
		}
		if (newPa->compareVal < thisPa->compareVal)
		{
			// other has higher possible utility
			return true;
		}
		else if (newPa->compareVal > thisPa->compareVal)
		{
			// this has higher possible utility
			return false;
		}
		// Now we are sure that both partial assignments have the same utility
		else if (newPa->plan->getId() > thisPa->plan->getId())
		{
			return false;
		}
		else if (newPa->plan->getId() < thisPa->plan->getId())
		{
			return true;
		}
		// Now we are sure that both partial assignments have the same utility and the same plan id
		if (thisPa->unAssignedRobots.size() < newPa->unAssignedRobots.size())
		{
			return true;
		}
		else if (thisPa->unAssignedRobots.size() > newPa->unAssignedRobots.size())
		{
			return false;
		}
		if (newPa->min < thisPa->min)
		{
			// other has higher actual utility
			return true;
		}
		else if (newPa->min > thisPa->min)
		{
			// this has higher actual utility
			return false;
		}
		auto me = thisPa->epRobotsMapping->getRobots();
		auto you = newPa->epRobotsMapping->getRobots();
		for (int i = 0; i < thisPa->epRobotsMapping->getCount(); ++i)
		{
			if (me->at(i)->size() < you->at(i)->size())
			{
				return true;
			}
			else if (me->at(i)->size() < you->at(i)->size())
			{
				return false;
			}
		}
		for (int i = 0; thisPa->epRobotsMapping->getCount(); ++i)
		{
			for (int j = 0; j < me->at(i)->size(); ++j)
			{
				if (me->at(i)->at(j) > you->at(i)->at(j))
				{
					return true;
				}
				else if (me->at(i)->at(j) > you->at(i)->at(j))
				{
					return false;
				}
			}
		}
		return false;

	}

	//TODO c# variant has to be adapted to c++
	/**
	 * Calculates a HashCode, which depends on the assignments and the plan.
	 * @return A HashCode, which should be robot independent.
	 */
	int PartialAssignment::getHashCode()
	{
		if (this->hashCalculated)
		{
			return this->hash;
		}
		int basei = this->epRobotsMapping->getCount() + 1;
		vector<int> robots;
		for (int i = 0; i < this->epRobotsMapping->getCount(); ++i)
		{
			robots = (*this->epRobotsMapping->getRobots()->at(i));
			for (int robot : robots)
			{

				//TODO find replacement for c# array.binarysearch
				this->hash += (i + 1) * pow(basei, robots[i]);
			}
		}
		this->hashCalculated = true;
		return this->hash;
	}

	string PartialAssignment::toString()
	{
		stringstream ss;
		ss << "Plan: " << this->plan->getName() << endl;
		ss << "Utility: " << this->min << ".." << this->max << endl;
		ss << "UnAssignedRobots: ";
		for (int robot : this->unAssignedRobots)
		{
			ss << robot << " ";
		}
		ss << endl;
		shared_ptr<vector<EntryPoint*> > ownEps = this->epRobotsMapping->getEntryPoints();
		vector<int> robots;
		for (int i = 0; i < this->epRobotsMapping->getCount(); ++i)
		{
			robots = (*this->epRobotsMapping->getRobots()->at(i));
			ss << "EPid: " << ownEps->at(i)->getId() << " Task: " << ownEps->at(i)->getTask()->getName() << " minCar: " << this->dynCardinalities[i]->getMin() << " maxCar: " << (this->dynCardinalities[i]->getMax() == INFINIT ? "*" : to_string(this->dynCardinalities[i]->getMax())) << " Assigned Robots: ";
			for (int robot : robots)
			{
				ss << robot << " ";
			}
			ss << endl;
		}

		ss << this->epRobotsMapping->toString();
		ss << "HashCode: " << this->getHashCode() << endl;

		return ss.str();

	}

	int PartialAssignment::numUnAssignedRobots()
	{
		return this->unAssignedRobots.size();
	}

	string PartialAssignment::assignmentCollectionToString()
	{
		return "PA: \n" + toString();
	}

	/**
	 * little helper to calculate the y-th power of x with integers
	 */
	int PartialAssignment::pow(int x, int y)
	{
		int ret = 1;
		for (int i = 0; i < y; i++)
		{
			ret *= x;
		}
		return ret;
	}

	void PartialAssignment::setMax(double max)
	{
		this->max = max;
		this->compareVal = (long)round(max * PRECISION);
	}

} /* namespace alica */

