#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

#include <mzx/algorithm/sort/bucket_sort.h>
#include <mzx/algorithm/sort/buddle_sort.h>
#include <mzx/algorithm/sort/count_sort.h>
#include <mzx/algorithm/sort/heap_sort.h>
#include <mzx/algorithm/sort/insert_sort.h>
#include <mzx/algorithm/sort/merge_sort.h>
#include <mzx/algorithm/sort/quick_sort.h>
#include <mzx/algorithm/sort/radix_sort.h>
#include <mzx/algorithm/sort/select_sort.h>
#include <mzx/algorithm/sort/shell_sort.h>

template <typename T>
void PrintInfo(const T &sort_list)
{
    std::cout << "[";
    for (auto &num : sort_list)
    {
        std::cout << num << ",";
    }
    std::cout << "]";
}

#define TEST_SORT(name)                                                                                                \
    std::random_shuffle(test.begin(), test.end(), [](int i) { return std::rand() % i; });                              \
    std::cout << std::setw(10) << #name;                                                                               \
    PrintInfo(test);                                                                                                   \
    std::cout << "->";                                                                                                 \
    mzx::name(test.begin(), test.end());                                                                               \
    PrintInfo(test);                                                                                                   \
    std::cout << std::endl

int main(int argc, char *argv[])
{
    std::srand(time(0));
    std::vector<int> test;
    int len = std::rand() % 6 + 5;
    for (int i = 0; i < len; ++i)
    {
        test.emplace_back(std::rand() % 20);
    }
    TEST_SORT(BuddleSort);
    TEST_SORT(InsertSort);
    TEST_SORT(SelectSort);
    TEST_SORT(ShellSort);
    TEST_SORT(MergeSort);
    TEST_SORT(QuickSort);
    TEST_SORT(HeapSort);
    TEST_SORT(CountSort);
    TEST_SORT(BucketSort);
    TEST_SORT(RadixSort);
    return 0;
}
