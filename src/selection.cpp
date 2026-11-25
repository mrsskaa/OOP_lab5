#include "lab05/selection.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
namespace lab05 {
Selection::Selection(const Selection& other) : size_(other.size_), capacity_(other.capacity_) {
    if (capacity_ != 0) {
        data_ = std::make_unique<std::weak_ptr<const Employee>[]>(capacity_);
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
}
Selection& Selection::operator=(const Selection& other) {
    if (this != &other) {
        Selection copy(other);
        Swap(copy);
    }
    return *this;
}
Selection::Selection(Selection&& other) noexcept {
    Swap(other);
}
Selection& Selection::operator=(Selection&& other) noexcept {
    if (this != &other) {
        Selection moved(std::move(other));
        Swap(moved);
    }
    return *this;
}
void Selection::Swap(Selection& other) noexcept {
    data_.swap(other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}
void Selection::Add(std::weak_ptr<const Employee> employee) {
    if (size_ == capacity_) {
        if (capacity_ > std::numeric_limits<std::size_t>::max() / 2) {
            throw std::length_error("Selection is too large");
        }
        const std::size_t next_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
        auto next = std::make_unique<std::weak_ptr<const Employee>[]>(next_capacity);
        for (std::size_t i = 0; i < size_; ++i) {
            next[i] = std::move(data_[i]);
        }
        data_ = std::move(next);
        capacity_ = next_capacity;
    }
    data_[size_++] = std::move(employee);
}
std::size_t Selection::size() const {
    return size_;
}
std::size_t Selection::capacity() const {
    return capacity_;
}
bool Selection::empty() const {
    return size_ == 0;
}
const std::weak_ptr<const Employee>& Selection::At(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Selection index out of range");
    }
    return data_[index];
}
}  // namespace lab05
