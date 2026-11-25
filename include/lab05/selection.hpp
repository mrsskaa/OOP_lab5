#pragma once
#include <cstddef>
#include <memory>

#include "lab05/employee.hpp"
namespace lab05 {
class Selection {
public:
    Selection() = default;
    Selection(const Selection& other);
    Selection& operator=(const Selection& other);
    Selection(Selection&& other) noexcept;
    Selection& operator=(Selection&& other) noexcept;
    ~Selection() = default;
    void Add(std::weak_ptr<const Employee> employee);
    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;
    const std::weak_ptr<const Employee>& At(std::size_t index) const;

private:
    void Swap(Selection& other) noexcept;
    std::unique_ptr<std::weak_ptr<const Employee>[]> data_;
    std::size_t size_{};
    std::size_t capacity_{};
};
}  // namespace lab05
