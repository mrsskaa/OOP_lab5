#pragma once
#include <cstddef>
#include <iosfwd>
#include <string>
namespace lab05 {
class Employee {
public:
    explicit Employee(double salary, std::string name = {}, std::string department = {});
    Employee(const Employee& other);
    Employee(Employee&& other) noexcept;
    Employee& operator=(const Employee& other) = default;
    Employee& operator=(Employee&& other) noexcept = default;
    ~Employee();
    const std::string& name() const;
    const std::string& department() const;
    double salary() const;
    static std::size_t live_count();
    bool operator<(const Employee& other) const;
    bool operator==(const Employee& other) const;

private:
    std::string name_;
    std::string department_;
    double salary_;
    static std::size_t live_count_;
};
std::ostream& operator<<(std::ostream& out, const Employee& employee);
}  // namespace lab05
