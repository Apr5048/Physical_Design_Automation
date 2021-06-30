#pragma once
#include <memory>
#include <vector>

struct Node;
struct Block
{
    int id;
    int cor_x, cor_y;
    bool rotate;
    int width, high, area;
    int cent_x, cent_y;
    Node *node;
    Block() : id(0), cor_x(0), cor_y(0), rotate(false), width(0), high(0), area(0), cent_x(0), cent_y(0) {}
};

struct Node
{
    Block *block;
    Node *right, *left, *parent;
    Node() : block(nullptr), right(nullptr), left(nullptr), parent(nullptr) {}
};
struct Terminal
{
    int id;
    int cor_x, cor_y;
    Terminal() : id(0), cor_x(0), cor_y(0){};
};
struct Net
{
    int degree;
    std::vector<int> blocks;
    std::vector<int> terminals;
    Net() : degree(0) {}
};

struct Input
{
    std::vector<std::unique_ptr<Block>> blockVec;
    std::vector<Block *> blockVecTmp;
    std::vector<std::unique_ptr<Terminal>> terminalVec;
    std::vector<std::unique_ptr<Net>> netVec;
    std::vector<int> contour;
    std::vector<int> traversalSeq;
    int numBlock;
    int bound;
    int block1, block2, child;
    std::unique_ptr<Node> root;
    double rate;
    std::vector<std::unique_ptr<Block>> bestSol;
    double beta;
    int totalWirelength;
    Input() : numBlock(0), bound(0), block1(0), block2(0), child(0), root(std::make_unique<Node>()), rate(0), beta(0), totalWirelength(0) {}
};
