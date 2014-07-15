/*
 * IPlanSelector.h
 *
 *  Created on: Jul 9, 2014
 *      Author: Paul Panin
 */

#ifndef IPLANSELECTOR_H_
#define IPLANSELECTOR_H_

#include <vector>
#include <list>

using namespace std;

namespace alica
{
	class RunningPlan;
	class AbstractPlan;

	class IPlanSelector
	{
	public:
		virtual ~IPlanSelector() {}
		//normal plan selection
		virtual shared_ptr<list<RunningPlan*> > getPlansForState(RunningPlan* planningParent,list<AbstractPlan*> plans, shared_ptr<vector<int> > robotIDs) = 0;

		/**
		 * Get the best Assignment for this RP with its plan and old Assignment, which is also similar to the old Assignment
		 * @param rp
		 * @return The new and similar
		 */
		virtual RunningPlan* getBestSimilarAssignment(RunningPlan* rp) = 0;
		virtual RunningPlan* getBestSimilarAssignment(RunningPlan* rp, shared_ptr<vector<int> > robots) = 0;

	};
}
#endif /* IPLANSELECTOR_H_ */
