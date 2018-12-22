#include <mzx/ecs.h>
#include <mzx/singleton.h>

struct Body
{
    explicit Body(int x, int y, int z, int d)
        : pos_x(x)
        , pos_y(y)
        , pos_z(z)
        , direction(d)
    {
    }
    int pos_x{ 0 };
    int pos_y{ 0 };
    int pos_z{ 0 };
    int direction{ 0 };
};

class World
{
public:
    World()
    {

    }
    ~World()
    {

    }
private:
};

int main(int argc, char *argv[])
{
    return 0;
}
