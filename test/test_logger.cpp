#include <iostream>
#include <mzx/logger.h>

int main(int argc, char *argv[])
{
    //MZX_CHECK_STATIC(2 == 1);
    int a = 2;
    MZX_CHECK(a == 2);
    MZX_DEBUG("12312312", "  dffdfd");
    MZX_WARN("ggggg ", "  dfdf", 123, "44");
    MZX_ERR("ffff ", "  ", " pp");
    mzx::Logger::SetLevel(mzx::Logger::Level::Debug);
    MZX_DEBUG("test debug:", 123123123);
    std::cout << "==================" << std::endl;
    MZX_INFO("test info:", 44444, 5555, 6666);
    std::cout << "==================" << std::endl;
    MZX_WARN("test warn:", "tt", 123, "44");
    std::cout << "==================" << std::endl;
    MZX_FATAL("test fatal");
    MZX_CHECK(a != 2, "ffdfdfdfdfdf");
    std::cout << "==================" << std::endl;
}
