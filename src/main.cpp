#include <iomanip>
#include <iostream>

#include "lab05/employee_tree.hpp"
namespace {
void Print(const lab05::Selection& selection) {
    std::cout << selection.size() << '\n';
    for (std::size_t i = 0; i < selection.size(); ++i) {
        if (const auto employee = selection.At(i).lock()) {
            std::cout << *employee << '\n';
        } else {
            std::cout << "[expired]\n";
        }
    }
}
}  // namespace
int main() {
    std::cout << std::setprecision(6);
    lab05::EmployeeTree tree;
    tree.Insert(lab05::Employee(90000, "Иванов", "ИТ"));
    tree.Insert(lab05::Employee(60000, "Петров", "Продажи"));
    tree.Insert(lab05::Employee(120000, "Сидоров", "ИТ"));
    tree.Insert(lab05::Employee(50000, "Орлов", "ИТ"));
    tree.Insert(lab05::Employee(75000, "Волков", "Бухгалтерия"));
    tree.Insert(lab05::Employee(100000, "Гусев", "ИТ"));
    std::cout << "Симметричный обход:\n";
    Print(tree.InOrder());
    const lab05::DepartmentSelector selector("ИТ");
    const auto selection = tree.Select(selector);
    std::cout << "Выборка по отделу ИТ:\n";
    Print(selection);
    tree.Remove(lab05::Employee(90000));
    std::cout << "После удаления зарплаты 90000:\n";
    Print(selection);
    return 0;
}
