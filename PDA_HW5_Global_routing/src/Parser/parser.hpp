#pragma once
#include "../Structure/structure.hpp"
#include <memory>
#include <string>
class Parser
{
public:
    std::unique_ptr<Input> read_file(std::string const &file);
    void output_file(std::string file,  Input *input);
};
