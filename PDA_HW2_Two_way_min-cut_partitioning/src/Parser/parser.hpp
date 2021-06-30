#include "../Structure/structure.hpp"
#include <istream>
#include <string>

class Parser
{
private:
    void readCell(std::string const &file, FMInput *FMInput);
    void readNet(std::string const &file, FMInput *FMInput);

public:
    std::unique_ptr<FMInput> parse(char *argv[]);
    void write(std::string file, FMInput *FMInput);
};
