#pragma once
#include <string>
#include <unordered_map>

#include "AlicaElement.h"

namespace alica
{
class Characteristic;
class ModelFactory;
class RoleFactory;
class RoleSet;
class Task;

class Role : public AlicaElement
{
public:
    Role();
    virtual ~Role();

    double getPriority(int64_t taskId) const;
    RoleSet* getRoleSet() {  return _roleSet; }
    std::unordered_map<Task*, double> getPriorities() {  return _taskPriorities; }
    std::unordered_map<std::string, Characteristic*> getCharacteristics() const {  return _characteristics; }

    void setRoleSet(RoleSet* roleSet) {  _roleSet = roleSet;}
    void setPriorities(std::unordered_map<Task*, double> taskPriorities) {  _taskPriorities = taskPriorities;}
    void setCharacteristics(std::unordered_map<std::string, Characteristic*> characteristics) {  _characteristics = characteristics;}
    void addCharacteristic(const std::string& name, Characteristic* characteristic) {  _characteristics.insert({name, characteristic}); }
    
    std::string toString(std::string indent = "") const override;

private:
    friend ModelFactory;
    friend RoleFactory;

    std::unordered_map<Task*, double> _taskPriorities;
    std::unordered_map<std::string, Characteristic*> _characteristics;
    RoleSet* _roleSet;
};

} // namespace alica
