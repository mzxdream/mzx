#include <mzx/vector3.h>
#include <iostream>

int main(int argc, char *argv[])
{
    mzx::Vector3<int> a(1, 2, 3);
    std::cout << "(" << a.X() << "," << a.Y() << "," << a.Z() << ")" << std::endl;
    mzx::Vector3<int> b(3, 2, 3);
    b.Y(3);
    std::cout << "(" << b.X() << "," << b.Y() << "," << b.Z() << ")" << std::endl;
    a *= b;
    std::cout << "(" << a.X() << "," << a.Y() << "," << a.Z() << ")" << std::endl;
    return 0;
}
