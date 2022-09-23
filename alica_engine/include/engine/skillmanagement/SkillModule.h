#pragma once

#include "engine/skillmanagement/ISkill.h"

#include <unordered_map>
#include <iostream> 

namespace alica {

    class SkillModule {
        public:
            SkillModule(){};
            ~SkillModule(){};
            
            alica::ISkill * get(std::string id) {
                    std::unordered_map<std::string, alica::ISkill * >::const_iterator itr = this->skills.find(id);
                    return (itr == this->skills.end()) ? nullptr : itr->second;
            }
        protected: 
          std::unordered_map<std::string, alica::ISkill * > skills;
    };
}