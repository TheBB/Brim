#include <iostream>
#include <sstream>

#include "object.h"
#include "parse.h"
#include "gc.h"


int main(int argc, char** argv)
{
    Lexer lexer(std::cin);

    std::vector<Token> ret;
    while (lexer) {
        Token token;
        lexer >> token;
        std::cout << ">> " << token << std::endl;
        ret.push_back(token);
    }

    std::cout << ret.size() << std::endl;

    return 0;
}
