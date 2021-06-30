#include "../Structure/structure.hpp"

class Solver
{
private:
    FMInput *fminput;
    void initialize_solution(int seed);
    void comput_netAB();
    void initialize_gain_and_unlock();
    void construct_bucketlist();
    int FM();
    void update_bucketlist(Cell *cell);
    void update_gain(ListNode *node);
    ListNode *select_cell();
    bool balance(int a, int b);
    void traceback();
public:
    Solver(FMInput *fminput);
    void solve();
};
