#include "lab05/employee_tree.hpp"

#include <algorithm>
#include <utility>
namespace lab05 {
struct EmployeeTree::Node {
    explicit Node(const Employee& value) : employee(std::make_shared<Employee>(value)) {}
    std::shared_ptr<Employee> employee;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};
EmployeeTree::EmployeeTree() = default;
EmployeeTree::~EmployeeTree() = default;
EmployeeTree::EmployeeTree(EmployeeTree&& other) noexcept
    : root_(std::move(other.root_)), size_(std::exchange(other.size_, 0)) {}
EmployeeTree& EmployeeTree::operator=(EmployeeTree&& other) noexcept {
    if (this != &other) {
        root_ = std::move(other.root_);
        size_ = std::exchange(other.size_, 0);
    }
    return *this;
}
bool EmployeeTree::Insert(const Employee& employee) {
    auto* link = &root_;
    while (*link) {
        if (employee < *(*link)->employee) {
            link = &(*link)->left;
        } else if (*(*link)->employee < employee) {
            link = &(*link)->right;
        } else {
            return false;
        }
    }
    *link = std::make_unique<Node>(employee);
    ++size_;
    return true;
}
std::weak_ptr<const Employee> EmployeeTree::Find(const Employee& key) const {
    const Node* node = root_.get();
    while (node) {
        if (key < *node->employee) {
            node = node->left.get();
        } else if (*node->employee < key) {
            node = node->right.get();
        } else {
            return node->employee;
        }
    }
    return {};
}
bool EmployeeTree::Remove(const Employee& key) {
    auto* link = &root_;
    while (*link && !(*(*link)->employee == key)) {
        link = key < *(*link)->employee ? &(*link)->left : &(*link)->right;
    }
    if (!*link) {
        return false;
    }
    // Сначала забираем узел: источник перемещения не должен лежать внутри уничтожаемого объекта.
    auto removed = std::move(*link);
    if (!removed->left) {
        *link = std::move(removed->right);
    } else if (!removed->right) {
        *link = std::move(removed->left);
    } else {
        auto* successor_link = &removed->right;
        while ((*successor_link)->left) {
            successor_link = &(*successor_link)->left;
        }
        auto successor = std::move(*successor_link);
        removed->employee = std::move(successor->employee);
        *successor_link = std::move(successor->right);
        *link = std::move(removed);
    }
    --size_;
    return true;
}
std::size_t EmployeeTree::size() const {
    return size_;
}
std::size_t EmployeeTree::Height(const Node* node) {
    if (!node) {
        return 0;
    }
    return 1 + std::max(Height(node->left.get()), Height(node->right.get()));
}
std::size_t EmployeeTree::height() const {
    return Height(root_.get());
}
void EmployeeTree::Traverse(const Node* node, const Selector& selector, Selection& result, bool reverse) {
    if (!node) {
        return;
    }
    Traverse(reverse ? node->right.get() : node->left.get(), selector, result, reverse);
    if (selector.Matches(*node->employee)) {
        result.Add(node->employee);
    }
    Traverse(reverse ? node->left.get() : node->right.get(), selector, result, reverse);
}
Selection EmployeeTree::Select(const Selector& selector) const {
    Selection result;
    Traverse(root_.get(), selector, result, false);
    return result;
}
Selection EmployeeTree::InOrder() const {
    return Select(AllSelector{});
}
Selection EmployeeTree::ReverseOrder() const {
    Selection result;
    Traverse(root_.get(), AllSelector{}, result, true);
    return result;
}
}  // namespace lab05
