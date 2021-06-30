#include "Structure/structure.hpp"
#include "Parser/parser.hpp"
#include "Router/router.hpp"

int main(int argc, char *argv[])
{
    
    Parser parser;
    auto input = parser.read_file(argv[1]);

    Router router(input.get());
    router.route();

    parser.output_file(argv[2], input.get());
}