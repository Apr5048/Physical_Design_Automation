#include "Parser/parser.hpp"
#include "Solver/solver.hpp"
#include "Structure/structure.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Argument Error\n";
        return 0;
    }
    Parser parser;
    auto fminput = parser.parse(argv);
    Solver solver(fminput.get());
    solver.solve();
    parser.write(argv[3], fminput.get());
}