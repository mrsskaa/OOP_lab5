#include "lab05/employee_tree.hpp"

#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

namespace lab05 {
namespace {
void Fill(EmployeeTree& tree) {
    tree.Insert(Employee(40, "Root", "IT"));
    tree.Insert(Employee(20, "Left", "Sales"));
    tree.Insert(Employee(60, "Right", "IT"));
    tree.Insert(Employee(10, "Leaf", "IT"));
    tree.Insert(Employee(30, "Middle", "Sales"));
    tree.Insert(Employee(50, "Successor", "IT"));
    tree.Insert(Employee(55, "Child", "Sales"));
}
void ExpectKeys(const Selection& selection, std::initializer_list<double> keys) {
    ASSERT_EQ(selection.size(), keys.size());
    std::size_t i = 0;
    for (double key : keys) {
        auto employee = selection.At(i++).lock();
        ASSERT_TRUE(employee);
        EXPECT_EQ(employee->salary(), key);
    }
}
TEST(EmployeeTest, ComparisonsUseOnlySalary) {
    Employee a(10, "A", "IT");
    Employee b(10, "B", "Sales");
    EXPECT_EQ(a, b);
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(b < a);
}
TEST(EmployeeTest_Throw, RejectsInvalidSalary) {
    EXPECT_THROW(Employee(-1), std::invalid_argument);
    EXPECT_THROW(Employee(std::numeric_limits<double>::infinity()), std::invalid_argument);
    EXPECT_THROW(Employee(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
    EXPECT_EQ(Employee::live_count(), 0U);
}
TEST(EmployeeTest, CountsCopyMoveAndAssignment) {
    EXPECT_EQ(Employee::live_count(), 0U);
    {
        Employee a(1);
        Employee b(a);
        Employee c(std::move(a));
        b = c;
        c = std::move(b);
        EXPECT_EQ(Employee::live_count(), 3U);
    }
    EXPECT_EQ(Employee::live_count(), 0U);
}
TEST(EmployeeTreeTest, EmptyAndSingleHeight) {
    EmployeeTree tree;
    EXPECT_EQ(tree.size(), 0U);
    EXPECT_EQ(tree.height(), 0U);
    EXPECT_TRUE(tree.InOrder().empty());
    EXPECT_TRUE(tree.Select(DepartmentSelector("IT")).empty());
    ASSERT_TRUE(tree.Insert(Employee(1)));
    EXPECT_EQ(tree.height(), 1U);
    EXPECT_EQ(tree.size(), 1U);
}
TEST(EmployeeTreeTest_Fail, RejectsDuplicateKey) {
    EmployeeTree tree;
    tree.Insert(Employee(10, "A", "IT"));
    EXPECT_FALSE(tree.Insert(Employee(10, "B", "Sales")));
    EXPECT_EQ(tree.size(), 1U);
    EXPECT_EQ(tree.Find(Employee(10)).lock()->name(), "A");
}
TEST(EmployeeTreeTest, FindsActualPayload) {
    EmployeeTree tree;
    Fill(tree);
    auto found = tree.Find(Employee(20)).lock();
    ASSERT_TRUE(found);
    EXPECT_EQ(found->name(), "Left");
    EXPECT_EQ(found->department(), "Sales");
}
TEST(EmployeeTreeTest_Fail, MissingSearchAndRemoval) {
    EmployeeTree tree;
    EXPECT_TRUE(tree.Find(Employee(1)).expired());
    EXPECT_FALSE(tree.Remove(Employee(1)));
    Fill(tree);
    EXPECT_TRUE(tree.Find(Employee(99)).expired());
    EXPECT_FALSE(tree.Remove(Employee(99)));
    EXPECT_EQ(tree.size(), 7U);
}
TEST(EmployeeTreeTest, TraversesBothDirections) {
    EmployeeTree tree;
    Fill(tree);
    EXPECT_EQ(tree.height(), 4U);
    ExpectKeys(tree.InOrder(), {10, 20, 30, 40, 50, 55, 60});
    ExpectKeys(tree.ReverseOrder(), {60, 55, 50, 40, 30, 20, 10});
}
TEST(EmployeeTreeTest, RemovesLeaf) {
    EmployeeTree tree;
    Fill(tree);
    EXPECT_TRUE(tree.Remove(Employee(10)));
    EXPECT_EQ(tree.size(), 6U);
    EXPECT_TRUE(tree.Find(Employee(10)).expired());
    ExpectKeys(tree.InOrder(), {20, 30, 40, 50, 55, 60});
}
TEST(EmployeeTreeTest, RemovesNodeWithOneChild) {
    EmployeeTree tree;
    Fill(tree);
    EXPECT_TRUE(tree.Remove(Employee(50)));
    EXPECT_EQ(tree.size(), 6U);
    ExpectKeys(tree.InOrder(), {10, 20, 30, 40, 55, 60});
}
TEST(EmployeeTreeTest, RemovesTwoChildrenPreservingSuccessorIdentity) {
    EmployeeTree tree;
    Fill(tree);
    auto removed = tree.Find(Employee(40));
    auto successor = tree.Find(Employee(50));
    EXPECT_TRUE(tree.Remove(Employee(40)));
    EXPECT_TRUE(removed.expired());
    ASSERT_FALSE(successor.expired());
    EXPECT_EQ(successor.lock(), tree.Find(Employee(50)).lock());
    EXPECT_EQ(tree.size(), 6U);
    ExpectKeys(tree.InOrder(), {10, 20, 30, 50, 55, 60});
}
TEST(EmployeeTreeTest, RemovesRootInAllShapes) {
    EmployeeTree tree;
    tree.Insert(Employee(2));
    tree.Insert(Employee(1));
    EXPECT_TRUE(tree.Remove(Employee(2)));
    ExpectKeys(tree.InOrder(), {1});
    EXPECT_TRUE(tree.Remove(Employee(1)));
    EXPECT_EQ(tree.height(), 0U);
    tree.Insert(Employee(1));
    tree.Insert(Employee(2));
    EXPECT_TRUE(tree.Remove(Employee(1)));
    ExpectKeys(tree.InOrder(), {2});
}
TEST(EmployeeTreeTest, SelectsDepartmentWithoutChangingTree) {
    EmployeeTree tree;
    Fill(tree);
    ExpectKeys(tree.Select(DepartmentSelector("IT")), {10, 40, 50, 60});
    ExpectKeys(tree.Select(DepartmentSelector("Sales")), {20, 30, 55});
    EXPECT_EQ(tree.Select(AllSelector{}).size(), 7U);
    EXPECT_TRUE(tree.Select(DepartmentSelector("Unknown")).empty());
    EXPECT_EQ(tree.size(), 7U);
    EXPECT_EQ(tree.height(), 4U);
    ExpectKeys(tree.InOrder(), {10, 20, 30, 40, 50, 55, 60});
}
TEST(SelectionTest, CopiesIndependentArrayWithWeakReferences) {
    EmployeeTree tree;
    Fill(tree);
    auto original = tree.InOrder();
    Selection copy(original);
    Selection assigned;
    assigned = original;
    copy.Add(tree.Find(Employee(10)));
    EXPECT_EQ(copy.size(), 8U);
    EXPECT_EQ(original.size(), 7U);
    tree.Remove(Employee(10));
    EXPECT_TRUE(original.At(0).expired());
    EXPECT_TRUE(copy.At(0).expired());
    EXPECT_TRUE(assigned.At(0).expired());
    EXPECT_EQ(assigned.size(), 7U);
}
TEST(SelectionTest, MovesAndEmptiesSource) {
    EmployeeTree tree;
    Fill(tree);
    auto source = tree.InOrder();
    Selection moved(std::move(source));
    EXPECT_TRUE(source.empty());
    EXPECT_EQ(source.capacity(), 0U);
    source = std::move(moved);
    EXPECT_TRUE(moved.empty());
    EXPECT_EQ(moved.capacity(), 0U);
    ExpectKeys(source, {10, 20, 30, 40, 50, 55, 60});
}
TEST(SelectionTest, GrowsByDoublingAndHandlesSelfAssignment) {
    Selection selection;
    EXPECT_EQ(selection.capacity(), 0U);
    for (int i = 0; i < 9; ++i) {
        selection.Add({});
    }
    EXPECT_EQ(selection.size(), 9U);
    EXPECT_EQ(selection.capacity(), 16U);
    Selection& alias = selection;
    selection = alias;
    selection = std::move(alias);
    EXPECT_EQ(selection.size(), 9U);
}
TEST(SelectionTest_Throw, ChecksIndex) {
    Selection selection;
    EXPECT_THROW(selection.At(0), std::out_of_range);
    selection.Add({});
    EXPECT_THROW(selection.At(1), std::out_of_range);
}
TEST(EmployeeTreeTest, MovesTreeAndReleasesOldDestination) {
    EmployeeTree source;
    Fill(source);
    auto reference = source.Find(Employee(40));
    EmployeeTree moved(std::move(source));
    EXPECT_EQ(source.size(), 0U);
    EXPECT_EQ(source.height(), 0U);
    source.Insert(Employee(99));
    auto old = source.Find(Employee(99));
    source = std::move(moved);
    EXPECT_TRUE(old.expired());
    EXPECT_EQ(moved.size(), 0U);
    EXPECT_EQ(moved.height(), 0U);
    EXPECT_FALSE(reference.expired());
    ExpectKeys(source.InOrder(), {10, 20, 30, 40, 50, 55, 60});
}
TEST(EmployeeTreeTest, WeakReferencesDoNotKeepRecordsAlive) {
    EXPECT_EQ(Employee::live_count(), 0U);
    Selection selection;
    {
        EmployeeTree tree;
        Fill(tree);
        selection = tree.InOrder();
        EXPECT_EQ(Employee::live_count(), 7U);
        tree.Remove(Employee(40));
        EXPECT_EQ(Employee::live_count(), 6U);
        EXPECT_EQ(selection.size(), 7U);
        EXPECT_TRUE(selection.At(3).expired());
    }
    EXPECT_EQ(Employee::live_count(), 0U);
    for (std::size_t i = 0; i < selection.size(); ++i) {
        EXPECT_TRUE(selection.At(i).expired());
    }
}
}  // namespace
}  // namespace lab05
