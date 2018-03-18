#include <iostream>
#include <sstream>

#include "object.h"
#include "parse.h"


int main(int argc, char** argv)
{
    Object obj = parse_toplevel(std::cin);
    std::cout << ">> " << obj << std::endl;

    return 0;
}
