#include <iostream>
#include <vector>
#include <mzx/algorithm/sort/buddle_sort.h>
#include <mzx/algorithm/sort/select_sort.h>
#include <mzx/algorithm/sort/insert_sort.h>

void PrintInfo(const std::string &name, const std::vector<int> &sort_list)
{
    std::cout << name << "[";
    for (auto &num : sort_list)
    {
        std::cout << num << ",";
    }
    std::cout << "]" << std::endl;
}

int main(int argc, char *argv[])
{
    std::vector<int> test{-1, 1, 3, 5, 6, 4, 4, 10, -2};
    mzx::BuddleSort(test.begin(), test.end());
    PrintInfo("BuddleSort", test);
    mzx::InsertSort(test.begin(), test.end());
    PrintInfo("InsertSort", test);
    mzx::SelectSort(test.begin(), test.end());
    PrintInfo("SelectSort", test);
    return 0;
}
