/*
 * RoleDefinitionSet.h
 *
 *  Created on: Mar 5, 2014
 *      Author: Stephan Opfer
 */

#ifndef ROLEDEFINITIONSET_H_
#define ROLEDEFINITIONSET_H_

#include <string>

#include "AlicaElement.h"
#include "engine/Types.h"

namespace alica {
class ModelFactory;

class RoleDefinitionSet : public AlicaElement {
public:
    RoleDefinitionSet();
    virtual ~RoleDefinitionSet();
    const std::string& getFileName() const { return _fileName; }
    const RoleVector& getRoles() const { return _roles; }

private:
    friend ModelFactory;
    void setRoles(const RoleVector& roles);
    void setFileName(const std::string& fileName);

    RoleVector _roles;
    std::string _fileName;
};

}  // namespace alica

#endif /* ROLEDEFINITIONSET_H_ */
