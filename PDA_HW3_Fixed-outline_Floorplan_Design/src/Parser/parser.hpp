#pragma once
#include "../Structure/structure.hpp"
#include <string>

class Parser
{
private:
    void parse_block(std::string const &file, Input *input);
    void parse_terminal(std::string const &file, Input *input);
    void parse_net(std::string const &file, Input *Input);
    void output_drawFile(Input *input);
public:
    Parser();
    std::unique_ptr<Input> parse(char *argv[]);
    void write(std::string const &file, Input *input);
};