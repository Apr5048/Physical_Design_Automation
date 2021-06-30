#pragma once
#include "../Structure/structure.hpp"
#include <random>
class Solver
{
private:
    Input *input;
    bool cmp(Block *a, Block *b);
    void compute_wh();
    void initial_floorplan();
    int compute_contour(int x, Block *block);
    void store_best();
    void compute_center(Block *block);
    int compute_wirelength();
    double cost(int WL);
    void rotate(int blockId);
    void initialize_traversalSeq();
    void dfs(Node *node, int blockId);
    void swap_block();
    int perturbation();
    void initialize_contour();
    void compute_cor(Node *node, int LR);
    void move();
    void restore(int perturbationId);
    void SA(double P, double eps, double r, int k);

public:
    Solver(Input *input);
    void solve();
};
