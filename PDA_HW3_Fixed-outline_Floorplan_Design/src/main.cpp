#include "Parser/parser.hpp"
#include "Solver/solver.hpp"

int main(int argc, char *argv[])
{
    Parser parser;
    auto input = parser.parse(argv);

    Solver solver(input.get());
    solver.solve();

    parser.write(argv[4], input.get());
    return 0;
}