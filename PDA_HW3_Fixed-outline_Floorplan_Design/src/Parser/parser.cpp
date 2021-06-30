#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
void Parser::parse_block(std::string const &file, Input *input)
{
    std::fstream fin(file);
    int id = 0;
    std::string buff;
    while (std::getline(fin, buff))
    {
        if (buff[0] == 'N' || buff[0] == 'p')
            continue;
        else if (buff[0] == 's')
        {
            std::vector<std::string> block_info;
            std::size_t prev = 0, pos;
            while ((pos = buff.find_first_of(" (,)", prev)) != std::string::npos)
            {
                if (pos > prev)
                    block_info.emplace_back(buff.substr(prev, pos - prev));
                prev = pos + 1;
            }
            auto block = std::make_unique<Block>();
            block->id = id;
            block->width = std::stoi(block_info.at(7));
            block->high = std::stoi(block_info.at(8));
            if (block->width < block->high)
            {
                std::swap(block->width, block->high);
                block->rotate = !block->rotate;
            }
            block->area = block->width * block->high;
            input->blockVecTmp.emplace_back(block.get());
            input->blockVec.emplace_back(std::move(block));
            ++id;
        }
    }
    input->numBlock = input->blockVec.size();
}

void Parser::parse_terminal(std::string const &file, Input *input)
{
    std::fstream fin(file);
    std::string buff;
    std::string tmp;
    int i = 0;
    while (std::getline(fin, buff))
    {
        ++i;
        auto terminal = std::make_unique<Terminal>();
        terminal->id = i;
        std::stringstream buff_stream(buff);
        buff_stream >> tmp >> terminal->cor_x >> terminal->cor_y;
        input->terminalVec.emplace_back(std::move(terminal));
    }
}

void Parser::parse_net(std::string const &file, Input *input)
{
    std::fstream fin(file);
    std::string buff;
    std::getline(fin, buff);
    std::getline(fin, buff);
    std::string tmp;
    int count = -1;
    while (std::getline(fin, buff))
    {
        if (buff[0] == 'N')
        {
            std::stringstream buff_stream(buff);
            count += 1;
            auto net = std::make_unique<Net>();
            buff_stream >> tmp >> tmp >> net->degree;
            input->netVec.emplace_back(std::move(net));
            
        }
        else
        {
            if (buff[0] == 's')
            {
                input->netVec.at(count)->blocks.emplace_back(std::stoi(buff.substr(2, buff.length() - 2)));
            }
            else
            {
                input->netVec.at(count)->terminals.emplace_back(std::stoi(buff.substr(1, buff.length() - 1)));
            }
        }
    }
}
std::unique_ptr<Input> Parser::parse(char *argv[])
{
    auto input = std::make_unique<Input>();
    parse_block(argv[1], input.get());
    std::cerr << argv[1] << "\n";
    parse_terminal(argv[3], input.get());
    std::cerr << argv[3] << "\n";
    parse_net(argv[2], input.get());
    std::cerr << argv[2] << "\n";

    input->rate = std::stod(argv[5]);
    return input;
}
Parser::Parser() {}
void Parser::output_drawFile(Input *input)
{
    std::ofstream fout("floorplan.draw");
    fout << input->bound << "\n";
    for (auto &block : input->bestSol)
    {
        fout << "sb" << block->id << " " << block->cor_x << " " << block->cor_y << " " << block->width << " " << block->high << "\n";
    }
}
void Parser::write(std::string const &file, Input *input)
{

    std::ofstream fout(file);
    fout << "Wirelength " << input->totalWirelength << "\n";
    fout << "Blocks"
         << "\n";
    for (auto &block : input->bestSol)
    {
        fout << "sb" << block->id << " " << block->cor_x << " " << block->cor_y << " " << block->rotate << "\n";
    }
    output_drawFile(input);
}