#pragma once
#include <vector>
#include <set>
#include <memory>

struct Grid
{
    int x;
    int y;
    double cost;
    bool calculated;
    bool open;
    double gn;
    int vCapacity;
    int hCapacity;
    Grid *parent;
    Grid(int x, int y, int vCapacity, int hCapacity) : x(x), y(y), vCapacity(vCapacity), hCapacity(hCapacity) {}
};

struct Net
{
    int id;
    std::vector<std::pair<int, int>> cor;
    std::vector<Grid *> path;
    std::vector<int> verEdges;
    std::vector<int> horEdges;
    int ripOff;
};

struct Input
{
    int X_BOUND, Y_BOUND;
    int vCapacity, hCapacity;

    std::vector<std::unique_ptr<Net>> netVec;
    std::vector<Net *> rerouteNetVec;
    std::vector<std::vector<std::unique_ptr<Grid>>> grids;
    std::set<Net *> overflowNetSet;
};
