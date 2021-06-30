#include "router.hpp"
#include <queue>
#include <vector>
#include <algorithm>

bool netcompare(const Net *A, const Net *B)
{
    return (abs(A->cor.at(0).first - A->cor.at(1).first) + abs(A->cor.at(0).second - A->cor.at(1).second)) <
           (abs(B->cor.at(0).first - B->cor.at(1).first) + abs(B->cor.at(0).second - B->cor.at(1).second));
}

void Router::init_grid()
{
    for (auto &i : input->grids)
    {
        for (auto &j : i)
        {
            j->open = false;
            j->gn = 0;
            j->cost = 0x3f3f3f3f;
            j->calculated = false;
            j->parent = nullptr;
        }
    }
}
bool Router::avalible(int x, int y)
{
    if ((x < input->X_BOUND && x >= 0) && (y < input->Y_BOUND && y >= 0))
        return true;
    return false;
}
// int Router::computeOverflow()
// {
//     int overflow = 0;
//     for (auto binVec : grids)
//     {
//         for (auto bin : binVec)
//         {
//             if (bin->hCapacity < 0)
//                 overflow += abs(bin->hCapacity);
//             if (bin->vCapacity < 0)
//                 overflow += abs(bin->vCapacity);
//         }
//     }
//     return overflow;
// }
void Router::updateCapacity(Net *net, Grid *start, Grid *end)
{

    if (start->x == end->x && start->y != end->y)
    {

        if (start->y > end->y)
        {
            for (int j = end->y; j < start->y; ++j)
            {
                input->grids.at(start->x).at(j)->vCapacity -= 1;
                if (input->grids.at(start->x).at(j)->vCapacity < 0)
                    input->overflowNetSet.emplace(net);
            }
        }
        else
        {
            for (int j = start->y; j < end->y; ++j)
            {
                input->grids.at(start->x).at(j)->vCapacity -= 1;
                if (input->grids.at(start->x).at(j)->vCapacity < 0)
                    input->overflowNetSet.emplace(net);
            }
        }
    }
    else if (start->x != end->x && start->y == end->y)
    {
        if (start->x > end->x)
        {
            for (int i = end->x; i < start->x; ++i)
            {
                input->grids.at(i).at(start->y)->hCapacity -= 1;
                if (input->grids.at(i).at(start->y)->hCapacity < 0)
                    input->overflowNetSet.emplace(net);
            }
        }
        else
        {
            for (int i = start->x; i < end->x; ++i)
            {
                input->grids.at(i).at(start->y)->hCapacity -= 1;
                if (input->grids.at(i).at(start->y)->hCapacity < 0)
                    input->overflowNetSet.emplace(net);
            }
        }
    }
}
void Router::Astar_route()
{
    for (auto net : input->overflowNetSet)
    {
        init_grid();

        std::priority_queue<std::pair<double, Grid *>, std::vector<std::pair<double, Grid *>>, std::greater<std::pair<double, Grid *>>> openList;
        auto source = input->grids.at(net->cor.at(0).first).at(net->cor.at(0).second).get();
        auto sink = input->grids.at(net->cor.at(1).first).at(net->cor.at(1).second).get();
        openList.push(std::make_pair(source->cost, source));

        bool find = false;

        while (!openList.empty())
        {

            auto cur_grid = openList.top().second;
            openList.pop();

            cur_grid->open = false;
            if (cur_grid->x == net->cor.at(1).first && cur_grid->y == net->cor.at(1).second)
            {
                find = true;
                break;
            }

            cur_grid->calculated = true;

            std::vector<std::pair<int, int>> neighborVec = {
                std::make_pair(cur_grid->x + 1, cur_grid->y), // r
                std::make_pair(cur_grid->x - 1, cur_grid->y), // l
                std::make_pair(cur_grid->x, cur_grid->y + 1), // u
                std::make_pair(cur_grid->x, cur_grid->y - 1)  // d
            };

            for (int i = 0; i < 4; ++i)
            {
                if (avalible(neighborVec.at(i).first, neighborVec.at(i).second))
                {

                    auto neighbor_grid = input->grids.at(neighborVec.at(i).first).at(neighborVec.at(i).second).get();
                    int x = neighbor_grid->x;
                    int y = neighbor_grid->y;

                    if (!neighbor_grid->calculated)
                    {

                        neighbor_grid->gn = cur_grid->gn + 1;
                        double hValue = (abs(neighbor_grid->x - net->cor.at(1).first) + abs(neighbor_grid->y - net->cor.at(1).second)) * 0.1;
                        double cost;
                        int A = 1000;
                        int B = 100;
                        if (i == 0) // r
                        {
                            double term = (input->hCapacity - input->grids.at(x - 1).at(y)->hCapacity) / input->hCapacity * B;
                            cost = (neighbor_grid->gn) + A * std::max(0, -(input->grids.at(x - 1).at(y)->hCapacity)) + term + hValue;
                        }
                        else if (i == 1) //l
                        {
                            int term = (input->hCapacity - input->grids.at(x).at(y)->hCapacity) / input->hCapacity * B;
                            cost = (neighbor_grid->gn) + A * std::max(0, -(input->grids.at(x).at(y)->hCapacity)) + term + hValue;
                        }
                        else if (i == 2) //u
                        {
                            double term = (input->vCapacity - input->grids.at(x).at(y-1)->vCapacity) / input->vCapacity * B;
                            cost = (neighbor_grid->gn) + A * std::max(0, -(input->grids.at(x).at(y-1)->vCapacity)) + term + hValue;
                        }
                        else //d
                        {
                            double term = (input->vCapacity - input->grids.at(x).at(y)->vCapacity) / input->vCapacity * B;
                            cost = (neighbor_grid->gn) + A * std::max(0, -(input->grids.at(x).at(y)->vCapacity)) + term + hValue;
                        }

                        if (cost < neighbor_grid->cost)
                        {

                            neighbor_grid->cost = cost;
                            neighbor_grid->parent = cur_grid;
                            openList.push(std::make_pair(neighbor_grid->cost, neighbor_grid));
                            neighbor_grid->open = true;
                            neighbor_grid->calculated = false;
                        }
                    }
                }
            }
        }
        if (find)
        {
            auto pathnode = sink;
            while (pathnode)
            {
                net->path.emplace_back(pathnode);
                pathnode = pathnode->parent;
            }
        }
        for (size_t i = 0; i < net->path.size() - 1; ++i)
        {
            updateCapacity(net, net->path.at(i), net->path.at(i + 1));
        }
    }
}
void Router::ripUpUpdateCapacity(Grid *A, Grid *B)
{
    if (A->x == B->x && A->y != B->y)
    {
        int i = A->x;
        if (A->y > B->y)
        {
            for (int j = B->y; j < A->y; ++j)
            {
                input->grids.at(i).at(j)->vCapacity += 1;
            }
        }
        else
        {
            for (int j = A->y; j < B->y; ++j)
            {
                input->grids.at(i).at(j)->vCapacity += 1;
            }
        }
    }
    else if (A->x != B->x && A->y == B->y)
    {
        int j = A->y;
        if (A->x > B->x)
        {
            for (int i = B->x; i < A->x; ++i)
            {
                input->grids.at(i).at(j)->hCapacity += 1;
            }
        }
        else
        {
            for (int i = A->x; i < B->x; ++i)
            {
                input->grids.at(i).at(j)->hCapacity += 1;
            }
        }
    }
}
void Router::ripUp(Net *net)
{
    for (size_t i = net->path.size() - 1; i > 0; --i)
    {
        ripUpUpdateCapacity(net->path.at(i), net->path.at(i - 1));
        net->path.pop_back();
    }
    net->path.pop_back();
}
void Router::Lshape()
{
    std::sort(input->rerouteNetVec.begin(), input->rerouteNetVec.end(), netcompare);
    for (auto net : input->rerouteNetVec)
    {
        
        auto source = input->grids.at(net->cor.at(0).first).at(net->cor.at(0).second).get();
        
        auto sink = input->grids.at(net->cor.at(1).first).at(net->cor.at(1).second).get();
        net->path.emplace_back(source);
        if (source->x == sink->x && source->y != sink->y)
        {
            net->path.emplace_back(sink);
            updateCapacity(net, source, sink);
        }
        else if (source->x != sink->x && source->y == sink->y)
        {
            net->path.emplace_back(sink);
            updateCapacity(net, source, sink);
        }
        else
        {
            auto midNode = input->grids.at(source->x).at(sink->y).get();
            net->path.emplace_back(midNode);
            updateCapacity(net, source, midNode);
            net->path.emplace_back(sink);
            updateCapacity(net, midNode, sink);
        }
    }
    for (auto net : input->overflowNetSet)
    {
        ripUp(net);
    }
}

void Router::route()
{
    Lshape();
    Astar_route();
}
Router::Router(Input *input) : input(input) {}