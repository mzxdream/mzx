#include <mzx/sort/buddle_sort.h>
#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
    std::vector<int> test{1, 3, 5, 6, 4, 4, 10};
    mzx::BuddleSort(test.begin(), test.end());
    mzx::BuddleSort(test.begin(), test.end(), std::greater<int>());
    int tmp[] = {0, 1, 2, 3, 4, 5, 4, 4};
    mzx::BuddleSort(tmp, tmp + 8);
    for (auto &num : test)
    {
        std::cout << num << ",";
    }
    std::cout << std::endl;
    for (auto &num : tmp)
    {
        std::cout << num << ",";
    }
    std::cout << std::endl;
    return 0;
}
