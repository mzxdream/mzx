#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <mzx/algorithm/sort/buddle_sort.h>
#include <mzx/algorithm/sort/select_sort.h>
#include <mzx/algorithm/sort/insert_sort.h>
#include <mzx/algorithm/sort/shell_sort.h>

void PrintInfo(const std::vector<int> &sort_list)
{
    std::cout << "[";
    for (auto &num : sort_list)
    {
        std::cout << num << ",";
    }
    std::cout << "]";
}

#define TEST_SORT(name) \
    std::random_shuffle(test.begin(), test.end()); \
    std::cout << std::setw(10) << #name; \
    PrintInfo(test); \
    std::cout << "->"; \
    mzx::name(test.begin(), test.end()); \
    PrintInfo(test); \
    std::cout << std::endl

int main(int argc, char *argv[])
{
    std::vector<int> test{-1, 1, 3, 5, 6, 4, 4, 10, -2};
    TEST_SORT(BuddleSort);
    TEST_SORT(InsertSort);
    TEST_SORT(SelectSort);
    TEST_SORT(ShellSort);
    return 0;
}
