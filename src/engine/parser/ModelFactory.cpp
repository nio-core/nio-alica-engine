/*
 * ModelFactory.cpp
 *
 *  Created on: Mar 27, 2014
 *      Author: Stephan Opfer
 */

#include "engine/parser/ModelFactory.h"
namespace alica
{
	const string ModelFactory::conditions = "conditions";
	const string ModelFactory::entryPoints = "entryPoints";
	const string ModelFactory::rating = "rating";
	const string ModelFactory::states = "states";
	const string ModelFactory::synchronisations = "synchronisations";
	const string ModelFactory::transitions = "transitions";
	const string ModelFactory::vars = "vars";
	const string ModelFactory::state = "state";
	const string ModelFactory::task = "task";
	const string ModelFactory::inTransitions = "inTransitions";
	const string ModelFactory::outTransitions = "outTransitions";
	const string ModelFactory::plans = "plans";
	const string ModelFactory::parametrisation = "parametrisation";
	const string ModelFactory::subplan = "subplan";
	const string ModelFactory::subvar = "subvar";
	const string ModelFactory::var = "var";
	const string ModelFactory::result = "result";

	ModelFactory::ModelFactory(PlanParser* p, shared_ptr<PlanRepository> rep)
	{
		this->parser = p;
		this->rep = rep;
		this->ignoreMasterPlanId = false;
	}

	ModelFactory::~ModelFactory()
	{
	}

	void ModelFactory::setIgnoreMasterPlanId(bool value)
	{
		this->ignoreMasterPlanId = value;
	}

	bool ModelFactory::getIgnoreMasterPlanId()
	{
		return this->ignoreMasterPlanId;
	}

	Plan* ModelFactory::createPlan(tinyxml2::XMLDocument* node)
	{
		tinyxml2::XMLElement* element = node->FirstChildElement();

		long id = this->parser->parserId(element);
		Plan* plan = new Plan(id);
		plan->setFilename(this->parser->getCurrentFile());
		setAlicaElementAttributes(plan, element);

		string isMasterPlanAttr = element->Attribute("masterPlan");

		if (!isMasterPlanAttr.empty())
		{
			transform(isMasterPlanAttr.begin(), isMasterPlanAttr.end(), isMasterPlanAttr.begin(), ::tolower);

			if (isMasterPlanAttr.compare("true") == 0)
			{
				plan->setMasterPlan(true);
			}
		}

		string attr = element->Attribute("minCardinality");
		if (!attr.empty())
		{
			plan->setMinCardinality(stoi(attr));
		}
		attr = element->Attribute("maxCardinality");
		if (!attr.empty())
		{
			plan->setMaxCardinality(stoi(attr));
		}
		attr = element->Attribute("utilityThreshold");
		if (!attr.empty())
		{
			plan->setMaxCardinality(stod(attr));
		}

		// insert into elements map
		addElement(plan);
		// insert into plan repository map
		this->rep.get()->getPlans().insert(pair<long, Plan*>(plan->getId(), plan));

		tinyxml2::XMLElement* curChild = element->FirstChildElement();
		while (curChild != nullptr)
		{

			if (isReferenceNode(curChild))
			{
				AlicaEngine::getInstance()->abort("MF: Plan child is reference", curChild);
			}

			const char* val = curChild->Value();

			if (entryPoints.compare(val) == 0)
			{
				EntryPoint* ep = createEntryPoint(curChild);
				plan->getEntryPoints().insert(pair<long, EntryPoint*>(ep->getId(), ep));
				ep->setPlan(plan);
			}
			else if (states.compare(val) == 0)
			{
				string name = "";
				const char* typePtr = curChild->Attribute("xsi:type");
				string typeString = "";
				//Normal State
				if (typePtr)
				{
					typeString = typePtr;
				}
				if (typeString.empty())
				{
					State* state = createState(curChild);
					plan->getStates().push_back(state);
					state->setInPlan(plan);

				}
				else if (typeString.compare("alica:SuccessState") == 0)
				{
					SuccessState* suc = createSuccessState(curChild);
					suc->setInPlan(plan);
					plan->getSuccessStates().push_front(suc);
					plan->getStates().push_front(suc);

				}
				else if (typeString.compare("alica:FailureState") == 0)
				{
					//TODO: FailureState
				}
				else
				{
					AlicaEngine::getInstance()->abort("MF: Unknown State type:", typePtr);
				}
			}
			else if (transitions.compare(val) == 0)
			{

			}
			else if (conditions.compare(val) == 0)
			{

			}
			else if (vars.compare(val) == 0)
			{

			}
			else if (synchronisations.compare(val) == 0)
			{

			}
			else if (rating.compare(val) == 0)
			{

			}
			else
			{
				AlicaEngine::getInstance()->abort("MF: Unhandled Plan Child: ", val);
			}
			curChild = curChild->NextSiblingElement();
		}

		return plan;
	}

	SuccessState* ModelFactory::createSuccessState(tinyxml2::XMLElement* element)
	{
		SuccessState* suc = new SuccessState();
		suc->setId(this->parser->parserId(element));
		setAlicaElementAttributes(suc, element);

		addElement(suc);
		this->rep.get()->getStates().insert(pair<long, State*>(suc->getId(), suc));

		tinyxml2::XMLElement* curChild = element->FirstChildElement();
		while (curChild != nullptr)
		{
			const char* val = curChild->Value();
			long cid = this->parser->parserId(curChild);
			if (inTransitions.compare(val) == 0)
			{
				this->stateInTransitionReferences.push_back(pair<long, long>(suc->getId(), cid));
			}
			else if (result.compare(val) == 0)
			{
				PostCondition* postCon = createPostCondition(curChild);
				suc->setPosCondition(postCon);
			}
			else
			{
				AlicaEngine::getInstance()->abort("MF: Unhandled State Child:", curChild);
			}

			curChild = curChild->NextSiblingElement();
		}
		return suc;
	}
	PostCondition* ModelFactory::createPostCondition(tinyxml2::XMLElement* element)
	{
		PostCondition* pos = new PostCondition();
		pos->setId(this->parser->parserId(element));
		setAlicaElementAttributes(pos, element);
		addElement(pos);

		string conditionString = "";
		const char* conditionPtr = element->Attribute("conditionString");
		if(conditionPtr){
			conditionString = conditionPtr;
			pos->setConditionString(conditionString);
		}
		if(!conditionString.empty()){
			//TODO: ANTLRBUILDER
		}
		else
		{
			//TODO: aus c#
			//pos->ConditionFOL = null;
		}

		if(element->FirstChild()){
			AlicaEngine::getInstance()->abort("MF: Unhandled Result child", element->FirstChild());
		}

		return pos;
	}

	EntryPoint* ModelFactory::createEntryPoint(tinyxml2::XMLElement* element)
	{

		EntryPoint* ep = new EntryPoint();
		ep->setId(this->parser->parserId(element));
		setAlicaElementAttributes(ep, element);
		string attr = element->Attribute("minCardinality");
		if (!attr.empty())
		{
			ep->setMinCardinality(stoi(attr));
		}
		attr = element->Attribute("maxCardinality");
		if (!attr.empty())
		{
			ep->setMaxCardinality(stoi(attr));
		}
		attr = element->Attribute("successRequired");
		if (!attr.empty())
		{
			transform(attr.begin(), attr.end(), attr.begin(), ::tolower);
			ep->setSuccessRequired(attr.compare("true") == 0);
		}

		addElement(ep);
		this->rep->getEntryPoints().insert(pair<long, EntryPoint*>(ep->getId(), ep));
		tinyxml2::XMLElement* curChild = element->FirstChildElement();
		bool haveState = false;
		long curChildId;

		while (curChild != nullptr)
		{
			const char* val = curChild->Value();
			curChildId = this->parser->parserId(curChild);

			if (state.compare(val) == 0)
			{
				// TODO siehe c#
				haveState = true;
			}
			else if (task.compare(val) == 0)
			{
				// TODO siehe c#
			}
			else
			{
				AlicaEngine::getInstance()->abort("MF: Unhandled EntryPoint Child: ", val);
			}
			curChild = curChild->NextSiblingElement();
		}

		if (!haveState)
		{
			AlicaEngine::getInstance()->abort("MF: No initial state identified for EntryPoint: ", ep->getId());
		}

		return ep;
	}
	/**
	 * Create a new state
	 * @param element the xml state tag
	 * @return state pointer
	 */
	State* ModelFactory::createState(tinyxml2::XMLElement* element)
	{
		State* s = new State();
		s->setId(this->parser->parserId(element));
		setAlicaElementAttributes(s, element);

		addElement(s);
		this->rep.get()->getStates().insert(pair<long, State*>(s->getId(), s));

		tinyxml2::XMLElement* curChild = element->FirstChildElement();
		while (curChild != nullptr)
		{
			const char* val = curChild->Value();
			long cid = this->parser->parserId(curChild);
			if (inTransitions.compare(val) == 0)
			{
				this->stateInTransitionReferences.push_back(pair<long, long>(s->getId(), cid));
			}
			else if (outTransitions.compare(val) == 0)
			{
				this->stateOutTransitionReferences.push_back(pair<long, long>(s->getId(), cid));
			}
			else if (plans.compare(val) == 0)
			{
				this->statePlanReferences.push_back(pair<long, long>(s->getId(), cid));
			}
			else if (parametrisation.compare(val) == 0)
			{
				Parametrisation* para = createParametrisation(curChild);
				s->getParametrisation().push_back(para);
			}
			else
			{
				AlicaEngine::getInstance()->abort("MF: Unhandled State Child:", curChild);
			}

			curChild = curChild->NextSiblingElement();
		}
		return s;
	}

	Parametrisation* ModelFactory::createParametrisation(tinyxml2::XMLElement* element)
	{
		Parametrisation* para = new Parametrisation();
		para->setId(this->parser->parserId(element));
		setAlicaElementAttributes(para, element);

		addElement(para);
		tinyxml2::XMLElement* curChild = element->FirstChildElement();
		while (curChild != nullptr)
		{
			const char* val = curChild->Value();
			long cid = this->parser->parserId(curChild);
			if (subplan.compare(val) == 0)
			{
				this->paramSubPlanReferences.push_back(pair<long, long>(para->getId(), cid));
			}
			else if (subvar.compare(val) == 0)
			{
				this->paramSubVarReferences.push_back(pair<long, long>(para->getId(), cid));
			}
			else if (var.compare(val) == 0)
			{
				this->paramVarReferences.push_back(pair<long, long>(para->getId(), cid));
			}
			else
			{
				AlicaEngine::getInstance()->abort("MF: Unhandled Parametrisation Child:", curChild);
			}

		}
		return para;
	}

	/**
	 * check if the element has a name
	 * @param node actual xml element
	 * @return the name of the xml element or missing name
	 */
	string ModelFactory::getNameOfNode(tinyxml2::XMLElement* node)
	{
		string name = "";
		const char* namePtr = node->Attribute("name");
		if (namePtr)
		{
			name = namePtr;
			return name;
		}
		else
		{
			return "MISSING-NAME";
		}
	}

	/**
	 *
	 * @param node xmlElement that represent the current xml tag
	 * @return true if the child node is a text element otherwise false
	 */
	bool ModelFactory::isReferenceNode(tinyxml2::XMLElement* node)
	{

		tinyxml2::XMLNode* curChild = node->FirstChild();
		while (curChild != nullptr)
		{
			const tinyxml2::XMLText* textNode = curChild->ToText();
			if (textNode)
			{
				return true;
			}
			curChild = curChild->NextSibling();

		}
		return false;
	}

	void ModelFactory::setAlicaElementAttributes(AlicaElement* ae, tinyxml2::XMLElement* ele)
	{
		string name = ele->Attribute("name");
		string comment = ele->Attribute("comment");

		if (!name.empty())
		{
			ae->setName(name);
		}
		else
			ae->setName("MISSING_NAME");
		if (!comment.empty())
		{
			ae->setComment(comment);
		}
		else
			ae->setComment("");
	}

	const map<long, AlicaElement*>& ModelFactory::getElements() const
	{
		return this->elements;
	}

	void ModelFactory::setElements(const map<long, AlicaElement*>& elements)
	{
		this->elements = elements;
	}

	void ModelFactory::addElement(AlicaElement* ae)
	{
		if (this->elements.find(ae->getId()) != this->elements.end())
		{
			stringstream ss;
			ss << "MF: ERROR Double IDs: " << ae->getId();
			AlicaEngine::getInstance()->abort(ss.str());
		}
		elements.insert(pair<long, AlicaElement*>(ae->getId(), ae));
	}

} /* namespace Alica */

