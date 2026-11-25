#pragma once
#include <cstddef>
#include <memory>

#include "lab05/employee.hpp"
#include "lab05/selection.hpp"
#include "lab05/selector.hpp"
namespace lab05 {
class EmployeeTree {
public:
    EmployeeTree();
    EmployeeTree(const EmployeeTree&) = delete;
    EmployeeTree& operator=(const EmployeeTree&) = delete;
    EmployeeTree(EmployeeTree&& other) noexcept;
    EmployeeTree& operator=(EmployeeTree&& other) noexcept;
    ~EmployeeTree();
    bool Insert(const Employee& employee);
    bool Remove(const Employee& key);
    std::weak_ptr<const Employee> Find(const Employee& key) const;
    std::size_t size() const;
    std::size_t height() const;
    Selection Select(const Selector& selector) const;
    Selection InOrder() const;
    Selection ReverseOrder() const;

private:
    struct Node;
    static std::size_t Height(const Node* node);
    static void Traverse(const Node* node, const Selector& selector, Selection& result, bool reverse);
    std::unique_ptr<Node> root_;
    std::size_t size_{};
};
}  // namespace lab05
