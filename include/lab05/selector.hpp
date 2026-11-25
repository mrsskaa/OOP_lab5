#pragma once
#include <string>

#include "lab05/employee.hpp"
namespace lab05 {
class Selector {
public:
    virtual ~Selector() = default;
    virtual bool Matches(const Employee& employee) const = 0;
};
class DepartmentSelector final : public Selector {
public:
    explicit DepartmentSelector(std::string department);
    bool Matches(const Employee& employee) const override;

private:
    std::string department_;
};
class AllSelector final : public Selector {
public:
    bool Matches(const Employee& employee) const override;
};
}  // namespace lab05
