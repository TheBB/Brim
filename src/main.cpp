#include <iostream>
#include <sstream>

#include "object.h"
#include "parse.h"


int main(int argc, char** argv)
{
    VM::push_frame();
    parse_toplevel(std::cin);

    if (VM::has_error())
        std::cout << ">> " << VM::get_error() << std::endl;
    else
        std::cout << ">> " << VM::peek() << std::endl;

    return 0;
}
