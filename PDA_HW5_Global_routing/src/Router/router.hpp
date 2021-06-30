#include "../Structure/structure.hpp"

class Router
{
private:
    Input *input;
    void init_grid();
    void Astar_route();
    bool avalible(int x, int y);
    //int computeOverflow();
    void updateCapacity(Net *net, Grid *start, Grid *end);
    void Lshape();
    void ripUpUpdateCapacity(Grid *A, Grid *B);
    void ripUp(Net *net);

public:
    Router(Input *input);
    void route();
};
