#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
struct Cell;
struct ListNode
{
    ListNode *leftNode;
    ListNode *rightNode;
    Cell *cell;
    ListNode() : leftNode(nullptr), rightNode(nullptr), cell(nullptr) {}
};
struct Net;
struct Cell
{
    size_t size;
    std::string cellName;
    bool locked;
    int set, gain, cellId;
    std::vector<Net *> relativeNets;
    std::unique_ptr<ListNode> bucketNode;
    Cell() : size(0), locked(false), set(-1), gain(0), cellId(0) {}
};

struct Net
{
    size_t aNum, bNum;
    std::string netName;
    std::vector<Cell *> relativeCells;
    Net() : aNum(0), bNum(0) {}
};

struct FMInput
{
    size_t sumCellArea;
    std::unordered_map<std::string, int> cellName2Id;
    std::vector<std::unique_ptr<ListNode>> bucketList;
    std::vector<std::unique_ptr<Cell>> cellVec;
    std::vector<Cell *> movedCell;
    std::vector<std::unique_ptr<Net>> netVec;
    size_t A, B, maxP, netSize;
    int topK;
    FMInput() : sumCellArea(0), A(0), B(0), maxP(0), topK(0) {}
};
