#include "lab05/employee.hpp"

#include <cmath>
#include <ostream>
#include <stdexcept>
#include <utility>
namespace lab05 {
std::size_t Employee::live_count_ = 0;
Employee::Employee(double salary, std::string name, std::string department)
    : name_(std::move(name)), department_(std::move(department)), salary_(salary) {
    if (!std::isfinite(salary) || salary < 0) {
        throw std::invalid_argument("Salary must be finite and nonnegative");
    }
    ++live_count_;
}
Employee::Employee(const Employee& other) : name_(other.name_), department_(other.department_), salary_(other.salary_) {
    ++live_count_;
}
Employee::Employee(Employee&& other) noexcept
    : name_(std::move(other.name_)), department_(std::move(other.department_)), salary_(other.salary_) {
    ++live_count_;
}
Employee::~Employee() {
    --live_count_;
}
const std::string& Employee::name() const {
    return name_;
}
const std::string& Employee::department() const {
    return department_;
}
double Employee::salary() const {
    return salary_;
}
std::size_t Employee::live_count() {
    return live_count_;
}
bool Employee::operator<(const Employee& other) const {
    return salary_ < other.salary_;
}
bool Employee::operator==(const Employee& other) const {
    return salary_ == other.salary_;
}
std::ostream& operator<<(std::ostream& out, const Employee& employee) {
    return out << employee.name() << ' ' << employee.department() << ' ' << employee.salary();
}
}  // namespace lab05
