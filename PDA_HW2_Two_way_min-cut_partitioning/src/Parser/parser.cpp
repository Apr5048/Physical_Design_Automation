#include "parser.hpp"
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <iostream>

void Parser::readCell(std::string const &file, FMInput *fminput)
{
    std::ifstream fin(file);
    std::string buff;
    int cId = 0;
    while (std::getline(fin, buff))
    {
        auto cell = std::make_unique<Cell>();
        // std::cerr << cell.get() << '\n';
        std::stringstream buff_stream(buff);
        buff_stream >> cell->cellName >> cell->size;
        cell->cellId = cId;
        auto node = std::make_unique<ListNode>();
        node->cell = cell.get();
        cell->bucketNode = std::move(node);

        fminput->sumCellArea += cell->size;
        fminput->cellName2Id.emplace(cell->cellName, cId);
        fminput->cellVec.emplace_back(std::move(cell));
        cId += 1;
    }
}

void Parser::readNet(std::string const &file, FMInput *fminput)
{
    std::ifstream fin(file);
    std::string buff;
    int nId = 0;
    while (std::getline(fin, buff))
    {
        std::stringstream buff_stream(buff);
        std::string tmp;
        while (buff_stream >> tmp)
        {
            if (tmp[0] == 'n')
            {
                auto net = new Net();
                net->netName = tmp;
                fminput->netVec.emplace_back(net);
            }
            else if (tmp[0] == 'c')
            {
                auto cId = fminput->cellName2Id.at(tmp);
                fminput->netVec.at(nId)->relativeCells.emplace_back(fminput->cellVec.at(cId).get());
                fminput->cellVec.at(cId)->relativeNets.emplace_back(fminput->netVec.at(nId).get());
            }
            else if (tmp[0] == '}')
            {
                nId += 1;
            }
        }
    }
}

std::unique_ptr<FMInput> Parser::parse(char *argv[])
{
    auto fminput = std::make_unique<FMInput>();
    readCell(argv[2], fminput.get());
    readNet(argv[1], fminput.get());
    for (auto const &cell : fminput->cellVec)
    {
        if (fminput->maxP < cell->relativeNets.size())
            fminput->maxP = cell->relativeNets.size();
    }
    fminput->bucketList.resize(fminput->maxP * 2 + 1);
    for (size_t i = 0; i < fminput->maxP * 2 + 1; ++i)
    {
        fminput->bucketList.at(i) = std::make_unique<ListNode>();
    }
    return fminput;
}

void Parser::write(std::string file, FMInput *fminput)
{
    std::ofstream fout(file);
    int Anum = 0, Bnum = 0;
    for (auto &cell : fminput->cellVec)
    {
        if (cell->set == 0)
            ++Anum;
        else
            ++Bnum;
    }
    int cutSize = 0;
    for (auto &net : fminput->netVec)
    {
        if (net->aNum > 0 && net->bNum > 0)
            cutSize += 1;
    }
    fout << "cut_size " << cutSize << "\n";
    fout << "A " << Anum << "\n";
    for (auto &cell : fminput->cellVec)
    {
        if (cell->set == 0)
            fout << cell->cellName << "\n";
    }
    fout << "B " << Bnum << "\n";
    for (auto &cell : fminput->cellVec)
    {
        if (cell->set == 1)
            fout << cell->cellName << "\n";
    }
}