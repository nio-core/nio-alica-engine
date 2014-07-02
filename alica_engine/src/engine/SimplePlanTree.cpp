/*
 * SimplePlanTree.cpp
 *
 *  Created on: Jun 16, 2014
 *      Author: Stefan Jakob
 */

#include <engine/SimplePlanTree.h>
#include "engine/model/EntryPoint.h"
#include "engine/model/Plan.h"

namespace alica
{

	SimplePlanTree::SimplePlanTree()
	{

	}

	SimplePlanTree::~SimplePlanTree()
	{

	}

	bool SimplePlanTree::containsPlan(AbstractPlan* plan)
	{
		if(this->getEntryPoint()->getPlan() == plan)
		{
			return true;
		}
		for(shared_ptr<SimplePlanTree> spt : this->getChildren())
		{
			if(spt->containsPlan(plan))
			{
				return true;
			}
		}
		return false;
	}

	EntryPoint* SimplePlanTree::getEntryPoint() const
	{
		return entryPoint;
	}

	void SimplePlanTree::setEntryPoint(EntryPoint* entryPoint)
	{
		this->entryPoint = entryPoint;
	}

	State* SimplePlanTree::getState() const
	{
		return state;
	}

	void SimplePlanTree::setState(State* state)
	{
		this->state = state;
	}

	unordered_set<shared_ptr<SimplePlanTree> > SimplePlanTree::getChildren()
	{
		return children;
	}

	void SimplePlanTree::setChildren(unordered_set<shared_ptr<SimplePlanTree> > children)
	{
		this->children = children;
	}

	int SimplePlanTree::getRobotId() const
	{
		return robotId;
	}

	void SimplePlanTree::setRobotId(int robotId)
	{
		this->robotId = robotId;
	}

	bool SimplePlanTree::isNewSimplePlanTree() const
	{
		return newSimplePlanTree;
	}

	void SimplePlanTree::setNewSimplePlanTree(bool newSimplePlanTree)
	{
		this->newSimplePlanTree = newSimplePlanTree;
	}
	long SimplePlanTree::getReceiveTime() const
	{
		return receiveTime;
	}

	void SimplePlanTree::setReceiveTime(long receiveTime)
	{
		this->receiveTime = receiveTime;
	}

	const list<long>& SimplePlanTree::getStateIds() const
	{
		return stateIds;
	}

	void SimplePlanTree::setStateIds(const list<long>& stateIds)
	{
		this->stateIds = stateIds;
	}

} /* namespace alica */

