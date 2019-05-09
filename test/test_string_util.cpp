#include <iostream>
#include <mzx/string_util.h>

int main(int argc, char *argv[])
{
    const char *t = "gdfgfdgdgg";
    char a[] = "xxxxxxx12331111";
    std::string c = mzx::Concat(1, "ffff", 1.03, t, a);
    std::cout << c << std::endl;
    // std::cout << mzx::TrimLeft(c, '1') << std::endl;
    // std::cout << mzx::TrimRight(c, '1') << std::endl;
    // std::cout << mzx::TrimLeft(c, "1f") << std::endl;
    // std::cout << mzx::TrimRight(c, "13") << std::endl;
    // std::cout << mzx::Trim(c, "13f") << std::endl;
    // std::cout << mzx::Trim(c, '1') << std::endl;
    auto tmp = mzx::Split(c, '1');
    for (auto &s : tmp)
    {
        std::cout << s << ", ";
    }
    std::cout << std::endl;
    tmp = mzx::Split(c, "123");
    for (auto &s : tmp)
    {
        std::cout << s << ", ";
    }
    std::cout << std::endl;
    return 0;
}
