#include "parser.hpp"
#include "../Structure/structure.hpp"
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
std::unique_ptr<Input> Parser::read_file(std::string const &file)
{
    auto input = std::make_unique<Input>();
    std::ifstream fin(file);
    std::string buff;
    while (getline(fin, buff))
    {
        std::stringstream buff_stream(buff);
        std::string tmp;
        if (buff[0] == 'g')
        {
            buff_stream >> tmp >> input->X_BOUND >> input->Y_BOUND;
        }
        else if (buff[0] == 'v')
        {
            buff_stream >> tmp >> tmp >> input->vCapacity;
        }
        else if (buff[0] == 'h')
        {
            buff_stream >> tmp >> tmp >> input->hCapacity;
        }
        else if (buff[1] == 'u')
        {
            continue;
        }
        else if (buff[0] == 'n')
        {
            auto net = std::make_unique<Net>();
            buff_stream >> tmp >> net->id >> tmp;
            net->ripOff = 0;
            input->rerouteNetVec.emplace_back(net.get());
            input->netVec.emplace_back(std::move(net));
        }
        else
        {
            int x, y;
            buff_stream >> x >> y;
            input->netVec.back()->cor.emplace_back(std::make_pair(x, y));
        }
    }
    
    input->grids.resize(input->X_BOUND);
    
    for (int i = 0; i < input->X_BOUND; ++i)
    {
        for (int j = 0; j < input->Y_BOUND; ++j)
        {
            auto grid = std::make_unique<Grid>(i, j, input->vCapacity, input->hCapacity);
            input->grids.at(i).emplace_back(std::move(grid));
        }
    }
    return input;
}
void Parser::output_file(std::string file, Input *input)
{
    std::ofstream fout(file);
    for (auto &net : input->netVec)
    {
        fout << "net" << net->id << " " << net->id << "\n";
        for (size_t i = 0; i < net->path.size() - 1; ++i)
        {
            fout << "(" << net->path.at(i)->x << "," << net->path.at(i)->y << ",1)-("
                 << net->path.at(i + 1)->x << "," << net->path.at(i + 1)->y << ",1)\n";
        }
        fout << "!\n";
    }
}