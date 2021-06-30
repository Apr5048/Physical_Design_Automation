#include "solver.hpp"
#include <math.h>
#include <algorithm>
#include <random>
#include <iostream>
std::minstd_rand0 g1(7122);
void Solver::compute_wh()
{
    int totalArea = 0;
    for (auto &block : input->blockVec)
    {
        totalArea += block->area;
    }
    input->bound = sqrt(totalArea * (1 + input->rate));
}

int Solver::compute_contour(int x, Block *block)
{
    int max_y = 0;
    for (int i = x; i < x + block->width; ++i)
        if (input->contour.at(i) > max_y)
            max_y = input->contour.at(i);
    for (int i = x; i < x + block->width; ++i)
        input->contour.at(i) = max_y + block->high;
    return max_y;
}
void Solver::initial_floorplan()
{
    Node *cur_r_node;
    Node *cur_parent;
    bool first_node = false;
    int x = 0;
    
    for (auto b : input->blockVecTmp)
    {
        if (!first_node)
        {
            auto node = new Node();
            b->cor_x = 0;
            b->cor_y = compute_contour(b->cor_x, b);
            node->block = b;
            x += b->width;
            first_node = true;
            cur_r_node = node;
            cur_parent = node;
            input->blockVec.at(b->id)->node = node;
            input->root->left = node;
        }
        else if (x + b->width < input->bound)
        {
            auto node = new Node();
            b->cor_x = x;
            b->cor_y = compute_contour(b->cor_x, b);
            x += b->width;
            node->block = b;
            cur_parent->left = node;
            node->parent = cur_parent;
            cur_parent = node;
            input->blockVec.at(b->id)->node = node;
        }
        else
        {
            auto node = new Node();
            b->cor_x = 0;
            b->cor_y = compute_contour(b->cor_x, b);
            x = b->width;
            node->block = b;
            node->parent = cur_r_node;
            cur_r_node->right = node;
            cur_r_node = node;
            cur_parent = node;
            input->blockVec.at(b->id)->node = node;
        }
    }
}

void Solver::store_best()
{
    for (auto &block : input->blockVec)
    {
        int i = block->id;
        input->bestSol.at(i)->id = i;
        input->bestSol.at(i)->cor_x = block->cor_x;
        input->bestSol.at(i)->cor_y = block->cor_y;
        input->bestSol.at(i)->width = block->width;
        input->bestSol.at(i)->high = block->high;
        input->bestSol.at(i)->rotate = block->rotate;
    }
}

void Solver::compute_center(Block *block)
{
    block->cent_x = floor((block->cor_x + (block->cor_x + block->width)) / 2);
    block->cent_y = floor((block->cor_y + (block->cor_y + block->high)) / 2);
}

int Solver::compute_wirelength()
{
    for (auto &block : input->blockVec)
    {
        compute_center(block.get());
    }
    int max_x, max_y, min_x, min_y;
    int total_wirelength = 0;
    for (auto &net : input->netVec)
    {
        max_x = 0, max_y = 0;
        min_x = input->bound, min_y = input->bound;
        for (auto i : net->blocks)
        {
            if (input->blockVec.at(i)->cent_x > max_x)
                max_x = input->blockVec.at(i)->cent_x;
            if (input->blockVec.at(i)->cent_y > max_y)
                max_y = input->blockVec.at(i)->cent_y;
            if (input->blockVec.at(i)->cent_x < min_x)
                min_x = input->blockVec.at(i)->cent_x;
            if (input->blockVec.at(i)->cent_y < min_y)
                min_y = input->blockVec.at(i)->cent_y;
        }
        for (auto j : net->terminals)
        {
            int i = j - 1;
            if (input->terminalVec.at(i)->cor_x > max_x)
                max_x = input->terminalVec.at(i)->cor_x;
            if (input->terminalVec.at(i)->cor_y > max_y)
                max_y = input->terminalVec.at(i)->cor_y;
            if (input->terminalVec.at(i)->cor_x < min_x)
                min_x = input->terminalVec.at(i)->cor_x;
            if (input->terminalVec.at(i)->cor_y < min_y)
                min_y = input->terminalVec.at(i)->cor_y;
        }
        total_wirelength += ((max_x - min_x) + (max_y - min_y));
    }
    return total_wirelength;
}

double Solver::cost(int WL)
{
    int A = 0;
    for (auto &block : input->blockVec)
    {
        if (block->cor_x + block->width > input->bound)
            A += (block->cor_x + block->width - input->bound);
        if (block->cor_y + block->high > input->bound)
            A += (block->cor_y + block->high - input->bound);
    }
    return (100 * A) + WL * input->beta;
}
void Solver::rotate(int blockId)
{
    input->blockVec.at(blockId)->rotate = !input->blockVec.at(blockId)->rotate;
    std::swap(input->blockVec.at(blockId)->width, input->blockVec.at(blockId)->high);
}
void Solver::initialize_traversalSeq()
{
    input->traversalSeq.clear();
}
void Solver::dfs(Node *node, int blockId)
{
    if (node->left == nullptr || node->right == nullptr)
        input->traversalSeq.emplace_back(node->block->id);
    if (node->left != nullptr && node->left->block->id != blockId)
        dfs(node->left, blockId);
    if (node->right != nullptr && node->right->block->id != blockId)
        dfs(node->right, blockId);
}
void Solver::swap_block()
{
    auto tmpNode = input->blockVec.at(input->block1)->node;
    input->blockVec.at(input->block1)->node->block = input->blockVec.at(input->block2).get();
    input->blockVec.at(input->block2)->node->block = input->blockVec.at(input->block1).get();
    input->blockVec.at(input->block1)->node = input->blockVec.at(input->block2)->node;
    input->blockVec.at(input->block2)->node = tmpNode;
}
int Solver::perturbation()
{
    int perturbId = g1() % 3;
    if (perturbId == 0)
    {
        //rotate
        input->block1 = g1() % input->numBlock;
        rotate(input->block1);
        return 0;
    }
    else if (perturbId == 1)
    {
        input->block1 = g1() % input->numBlock;
        while (input->blockVec.at(input->block1)->node->parent == nullptr)
            input->block1 = g1() % input->numBlock;

        initialize_traversalSeq();
        dfs(input->root->left, input->block1);

        auto moveBlock = input->blockVec.at(input->block1)->node;
        if (moveBlock->parent->left == moveBlock)
        {
            moveBlock->parent->left = nullptr;
            input->block2 = moveBlock->parent->block->id;
            input->child = 0;
            moveBlock->parent = nullptr;
        }
        else if (moveBlock->parent->right == moveBlock)
        {
            moveBlock->parent->right = nullptr;
            input->block2 = moveBlock->parent->block->id;
            input->child = 1;
            moveBlock->parent = nullptr;
        }
        int idx = g1() % input->traversalSeq.size();
        int pId = input->traversalSeq.at(idx);
        auto placeNode = input->blockVec.at(pId)->node;
        if (placeNode->left == nullptr && placeNode->right != nullptr)
        {
            placeNode->left = moveBlock;
            moveBlock->parent = placeNode;
        }
        else if (placeNode->left != nullptr && placeNode->right == nullptr)
        {
            placeNode->right = moveBlock;
            moveBlock->parent = placeNode;
        }
        else if (placeNode->left == nullptr && placeNode->right == nullptr)
        {
            int lr = g1() % 1;
            if (lr == 0)
            {
                placeNode->left = moveBlock;
                moveBlock->parent = placeNode;
            }
            else
            {
                placeNode->right = moveBlock;
                moveBlock->parent = placeNode;
            }
        }
        return 1;
    }
    else
    {
        input->block1 = g1() % input->numBlock;
        input->block2 = g1() % input->numBlock;
        while (input->block1 == input->block2)
            input->block2 = g1() % input->numBlock;

        swap_block();
        return 2;
    }
}
void Solver::initialize_contour()
{
    for (size_t i = 0; i < input->contour.size(); ++i)
        input->contour.at(i) = 0;
}
void Solver::compute_cor(Node *node, int LR)
{
    if (node->parent == nullptr)
    {
        node->block->cor_x = 0;
        node->block->cor_y = compute_contour(0, node->block);
    }
    if (LR == 0)
    {
        node->block->cor_x = node->parent->block->cor_x + node->parent->block->width;
        node->block->cor_y = compute_contour(node->block->cor_x, node->block);
    }
    else if (LR == 1)
    {
        node->block->cor_x = node->parent->block->cor_x;
        node->block->cor_y = compute_contour(node->block->cor_x, node->block);
    }
    if (node->left != nullptr)
        compute_cor(node->left, 0);
    if (node->right != nullptr)
        compute_cor(node->right, 1);
}
void Solver::move()
{
    auto moveBlock = input->blockVec.at(input->block1)->node;
    if (moveBlock->parent->left == moveBlock)
    {
        moveBlock->parent->left = nullptr;
        moveBlock->parent = nullptr;
    }
    else if (moveBlock->parent->right == moveBlock)
    {
        moveBlock->parent->right = nullptr;
        moveBlock->parent = nullptr;
    }
    //moveing
    auto parentBlock = input->blockVec.at(input->block2)->node;
    if (input->child == 0)
    {
        parentBlock->left = moveBlock;
        moveBlock->parent = parentBlock;
    }
    else if (input->child == 1)
    {
        parentBlock->right = moveBlock;
        moveBlock->parent = parentBlock;
    }
}
void Solver::restore(int perturbationId)
{
    if (perturbationId == 0)
        rotate(input->block1);
    if (perturbationId == 1)
        move();
    if (perturbationId == 2)
        swap_block();
}
void Solver::SA(double P, double eps, double r, int k)
{   
    std::cerr<<"SA start!"<<"\n";
    store_best();
    int WL = compute_wirelength();
    double localCost = cost(WL);
    double bestCost = localCost;
    double delta_avg = 100000;
    int N = input->numBlock * k;
    int MT = 0;
    int uphill = 0;
    int reject = 0;
    double T = delta_avg / std::log(P);
    double deltaCost, curCost;
    do
    {
        MT = 0;
        uphill = 0;
        reject = 0;
        do
        {
            
            input->block1 = input->block2 = input->child = -1;
            int perturbationId = perturbation();
            initialize_contour();
            compute_cor(input->root->left, -1);
            WL = compute_wirelength();
            curCost = cost(WL);
            ++MT;
            deltaCost = curCost - localCost;
            if (deltaCost <= 0 || (double)g1() / RAND_MAX < exp((deltaCost * -1) / T))
            {

                if (deltaCost > 0)
                {
                    ++uphill;
                }
                localCost = curCost;
                if (localCost < bestCost)
                {
                    bestCost = localCost;
                    store_best();
                }
            }
            else
            {
                restore(perturbationId);
                initialize_contour();
                compute_cor(input->root->left, -1);
                ++reject;
            }
        } while (uphill < N && MT < N * 2);
        T = r * T;
    } while (T > eps);
}

void Solver::solve()
{
    std::shuffle(input->blockVecTmp.begin(), input->blockVecTmp.end(), std::default_random_engine(778548));
    compute_wh();
    
    input->contour.resize(input->bound * 4, 0);
    input->traversalSeq.resize(input->numBlock, 0);
    std::cerr<<"compute_wh\n";
    initial_floorplan();
    std::cerr<<"initial_floorplan\n";
    input->bestSol.resize(input->numBlock);
    for(auto &b: input->bestSol)
    {
        b = std::make_unique<Block>();
    }
    if (input->numBlock == 100)
    {
        input->beta = 0.5;
        SA(1.1, 1e-6, 0.99, 9);
    }
    else if (input->numBlock == 200)
    {
        input->beta = 0.45;
        SA(1.1, 1e-6, 0.98, 8);
    }
    else
    {
        input->beta = 0.25;
        SA(1.1, 1e-6, 0.95, 7);
    }
    store_best();
    input->totalWirelength = compute_wirelength();
}
Solver::Solver(Input *input) : input(input)
{
}