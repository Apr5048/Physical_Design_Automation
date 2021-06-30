#include "solver.hpp"
#include <algorithm>
#include <iostream>
#include <random>

void Solver::initialize_solution(int seed)
{
    std::shuffle(fminput->cellVec.begin(), fminput->cellVec.end(), std::default_random_engine(seed));
    for (auto &cell : fminput->cellVec)
    {
        if (fminput->A <= fminput->B)
        {
            cell->set = 0;
            fminput->A += cell->size;
        }
        else
        {
            cell->set = 1;
            fminput->B += cell->size;
        }
    }
}
void Solver::comput_netAB()
{
    for (auto &net : fminput->netVec)
    {
        net->aNum = 0;
        net->bNum = 0;
    }
    for (auto &net : fminput->netVec)
    {
        for (auto cell : net->relativeCells)
        {
            if (cell->set == 0)
                net->aNum += 1;
            else
                net->bNum += 1;
        }
    }
}
void Solver::initialize_gain_and_unlock()
{
    for (auto &cell : fminput->cellVec)
    {
        cell->bucketNode->leftNode = nullptr;
        cell->bucketNode->rightNode = nullptr;
        cell->gain = 0;
        cell->locked = false;
        if (cell->set == 0)
        {
            for (auto net : cell->relativeNets)
            {
                if (net->aNum == 1)
                    cell->gain += 1;
                if (net->bNum == 0)
                    cell->gain -= 1;
            }
        }
        else
        {
            for (auto net : cell->relativeNets)
            {
                if (net->aNum == 0)
                    cell->gain -= 1;
                if (net->bNum == 1)
                    cell->gain += 1;
            }
        }
    }
}
void Solver::construct_bucketlist()
{
    for (size_t i = 0; i <= fminput->maxP * 2; ++i)
        fminput->bucketList.at(i)->rightNode = nullptr;
    for (auto &cell : fminput->cellVec)
    {
    
        auto node = cell->bucketNode.get();
        // std::cerr<<node<<"  node: cell "<< node->cell<<" ";
        node->leftNode = nullptr;
        node->rightNode = nullptr;
        int g = cell->gain + fminput->maxP;
        // std::cerr<<"g: "<<g<<" \n";
        if (fminput->bucketList.at(g)->rightNode == nullptr)
        {
            fminput->bucketList.at(g)->rightNode = node;
            node->leftNode = fminput->bucketList.at(g).get();
        }
        else
        {
            node->rightNode = fminput->bucketList.at(g)->rightNode;
            fminput->bucketList.at(g)->rightNode->leftNode = node;
            fminput->bucketList.at(g)->rightNode = node;
            node->leftNode = fminput->bucketList.at(g).get();
        }
    }
    // for(size_t i = 0; i <= fminput->maxP * 2; ++i)
    // {
    //     std::cerr<<"i :"<< i <<"\n";
    //     auto node = fminput->bucketList.at(i).get();
    //     while(node!=nullptr)
    //     {
    //         std::cerr<<node<<"\n";
    //         node = node->rightNode;
    //     }
    //     std::cerr<<"\n";
    // }
}
void Solver::update_bucketlist(Cell *cell)
{
    auto node = cell->bucketNode.get();
    // free node(cellId) in bucketlist
    if (node->rightNode == nullptr)
        node->leftNode->rightNode = nullptr;
    else
    {
        node->leftNode->rightNode = node->rightNode;
        node->rightNode->leftNode = node->leftNode;
    }
    node->leftNode = nullptr;
    node->rightNode = nullptr;
    // insert in bucketlist
    int g = cell->gain + fminput->maxP;
    if (fminput->bucketList.at(g)->rightNode == nullptr)
    {
        fminput->bucketList.at(g)->rightNode = node;
        node->leftNode = fminput->bucketList.at(g).get();
    }
    else
    {
        node->rightNode = fminput->bucketList.at(g)->rightNode;
        fminput->bucketList.at(g)->rightNode->leftNode = node;
        fminput->bucketList.at(g)->rightNode = node;
        node->leftNode = fminput->bucketList.at(g).get();
    }
}

bool Solver::balance(int a, int b)
{
    if (abs(a - b) * 10 < fminput->sumCellArea)
        return true;
    return false;
}

void Solver::update_gain(ListNode *node)
{
    auto baseCell = node->cell;
    int frontBlock = baseCell->set;
    baseCell->locked = true;
    baseCell->set = (!frontBlock); // 1->0, 0->1
    int toBlock = baseCell->set;
    for (auto net : baseCell->relativeNets)
    {
        /* check critical nets before the move */
        if (toBlock == 0)
        {
            if (net->aNum == 0)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->locked == false)
                    {
                        cell->gain += 1;
                        update_bucketlist(cell);
                    }
                }
            }
            else if (net->aNum == 1)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->set == 0 && cell->locked == false)
                    {
                        cell->gain -= 1;
                        update_bucketlist(cell);
                    }
                }
            }
        }
        if (toBlock == 1)
        {
            if (net->bNum == 0)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->locked == false)
                    {
                        cell->gain += 1;
                        update_bucketlist(cell);
                    }
                }
            }
            if (net->bNum == 1)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->set == 1 && cell->locked == false)
                    {
                        cell->gain -= 1;
                        update_bucketlist(cell);
                    }
                }
            }
        }
        /* change F(n) and T(n) to reflect the move */
        if (toBlock == 0)
        {
            net->aNum += 1;
            net->bNum -= 1;
        }
        if (toBlock == 1)
        {
            net->bNum += 1;
            net->aNum -= 1;
        }
        /* check for critical nets after the move */
        if (frontBlock == 0)
        {
            if (net->aNum == 0)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->locked == false && cell->cellId != baseCell->cellId)
                    {
                        cell->gain -= 1;
                        update_bucketlist(cell);
                    }
                }
            }
            if (net->aNum == 1)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->set == 0 && cell->locked == false && cell->cellId != baseCell->cellId)
                    {
                        cell->gain += 1;
                        update_bucketlist(cell);
                    }
                }
            }
        }
        if (frontBlock == 1)
        {
            if (net->bNum == 0)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->locked == false && cell->cellId != baseCell->cellId)
                    {
                        cell->gain -= 1;
                        update_bucketlist(cell);
                    }
                }
            }
            if (net->bNum == 1)
            {
                for (auto cell : net->relativeCells)
                {
                    if (cell->set == 1 && cell->locked == false && cell->cellId != baseCell->cellId)
                    {
                        cell->gain += 1;
                        update_bucketlist(cell);
                    }
                }
            }
        }
    }
}
ListNode *Solver::select_cell()
{
    for (int i = 2 * fminput->maxP; i >= 0; --i)
    {
        // std::cerr<<"i "<<i<<"\n";
        auto node = fminput->bucketList.at(i).get();
        node = node->rightNode;
        // std::cerr << node<<node->rightNode<<'\n';
        while (node != nullptr)
        {
            // std::cerr << node<<"\n";
            // std::cerr<< node->cell<<"\n";
            // exit(0);
            int tmpA = 0, tmpB = 0;
            if (node->cell->set == 0)
            {
                tmpA = fminput->A - node->cell->size;
                tmpB = fminput->B + node->cell->size;
                if (balance(tmpA, tmpB))
                {
                    fminput->A -= node->cell->size;
                    fminput->B += node->cell->size;
                    if (node->rightNode == nullptr)
                    {
                        node->leftNode->rightNode = nullptr;
                        node->leftNode = nullptr;
                    }
                    else
                    {
                        node->leftNode->rightNode = node->rightNode;
                        node->rightNode->leftNode = node->leftNode;
                        node->rightNode = nullptr;
                        node->leftNode = nullptr;
                    }
                    return node;
                }
            }
            else if (node->cell->set == 1)
            {
                tmpA = fminput->A + node->cell->size;
                tmpB = fminput->B - node->cell->size;
                if (balance(tmpA, tmpB))
                {
                    fminput->A += node->cell->size;
                    fminput->B -= node->cell->size;
                    if (node->rightNode == nullptr)
                    {
                        node->leftNode->rightNode = nullptr;
                        node->leftNode = nullptr;
                    }
                    else
                    {
                        node->leftNode->rightNode = node->rightNode;
                        node->rightNode->leftNode = node->leftNode;
                        node->rightNode = nullptr;
                        node->leftNode = nullptr;
                    }
                    return node;
                }
            }
            node = node->rightNode;
        }
    }
    return nullptr;
}
int Solver::FM()
{
    int partial_sum = 0;
    int max_partial_sum = 0;
    fminput->topK = 0;
    while (1)
    {   
        auto node = select_cell();
        if (node == nullptr)
            break;
        fminput->movedCell.emplace_back(node->cell);
        partial_sum += node->cell->gain;

        update_gain(node);
        if (partial_sum > max_partial_sum)
        {
            max_partial_sum = partial_sum;
            fminput->topK = fminput->movedCell.size();
        }
    }
    std::cerr<<max_partial_sum<<"\n";
    return max_partial_sum;
    
}
void Solver::traceback()
{
    for (size_t i = fminput->movedCell.size() - 1; i > static_cast<size_t>(fminput->topK - 1); --i)
    {
        fminput->movedCell.at(i)->set = (!fminput->movedCell.at(i)->set);
        if (fminput->movedCell.at(i)->set == 0)
        {
            fminput->A += fminput->movedCell.at(i)->size;
            fminput->B -= fminput->movedCell.at(i)->size;
        }
        else
        {
            fminput->A -= fminput->movedCell.at(i)->size;
            fminput->B += fminput->movedCell.at(i)->size;
        }
    }
    fminput->topK = 0;
    fminput->movedCell.clear();
}
void Solver::solve()
{
    int seed = 997;
    initialize_solution(seed);
    comput_netAB();
    initialize_gain_and_unlock();
    construct_bucketlist();

    while (1)
    {
        if (FM() > 0)
        {
            traceback();
            comput_netAB();
            initialize_gain_and_unlock();
            construct_bucketlist();
        }
        else
        {
            traceback();
            comput_netAB();
            break;
        }
    }
}
Solver::Solver(FMInput *fminput) : fminput(fminput)
{

}
