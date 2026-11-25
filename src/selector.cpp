#include "lab05/selector.hpp"

#include <utility>
namespace lab05 {
DepartmentSelector::DepartmentSelector(std::string department) : department_(std::move(department)) {}
bool DepartmentSelector::Matches(const Employee& employee) const {
    return employee.department() == department_;
}
bool AllSelector::Matches(const Employee&) const {
    return true;
}
}  // namespace lab05
