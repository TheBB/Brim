#include <iostream>

#include "parse.h"


void parse(std::istream& stream)
{
    Lexer lexer(stream);
    std::string s;
    while (lexer) {
        lexer >> s;
        std::cout << ">> " << s << std::endl;
    }
}

void Lexer::read()
{
    while (std::isspace(source.peek()))
        source.get();

    token = "";
    int c, d, e;

    switch ((c = source.get()))
    {
    case EOF: return;
    case '(': token = "("; return;
    case ')': token = ")"; return;
    case '[': token = "["; return;
    case ']': token = "]"; return;
    case '"': {
        token = "\"";
        while (true) {
            e = source.get();
            if (e != EOF)
                token.push_back(c);
            if (e == '"' || e == EOF)
                return;
        }
    }
    case '#':
        switch ((d = source.get())) {
        case '(': token = "#("; return;
        case 't': token = "#t"; return;
        case 'f': token = "#f"; return;
        default: token.insert(0, 1, d);
        }
    default: {
        token.insert(0, 1, c);
        while (true) {
            e = source.get();
            if (e != '(' && e != ')' && e != '[' && e != ']' && e != '"' && e != EOF && !std::isspace(e))
                token.push_back(e);
            else {
                source.unget();
                break;
            }
        }
    }
    }
}
