#include <iostream>
#include <sstream>

#include "object.h"
#include "parse.h"
#include "gc.h"


int main(int argc, char** argv)
{
    Frame& frame = Frame::make();
    Object obj = Object::Pair(Object::EmptyList(), Object::EmptyList());

    std::cout << GC::size() << std::endl;
    GC::collect();
    std::cout << GC::size() << std::endl;

    return 0;
}
