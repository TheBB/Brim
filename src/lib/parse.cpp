#include <iostream>

#include "object.h"

#include "parse.h"


static void read_identifier(std::istream& source, std::string& token)
{
    while (true) {
        char e = source.get();
        if (e != '(' && e != ')' && e != '[' && e != ']' && e != '"' &&
            e != ',' && e != '`' && e != '\'' && e != EOF && !std::isspace(e))
            token.push_back(e);
        else {
            source.unget();
            break;
        }
    }
}

void Lexer::read()
{
    while (std::isspace(source.peek()))
        source.get();

    token = "";
    int c, d, e;
    bool escaped = false;

    switch ((c = source.get()))
    {
    case EOF: return;
    case '(': case ')': case '[': case ']': case '`': case '\'':
        token = c;
        return;
    case ',':
        if ((d = source.get()) == '@')
            token = ",@";
        else {
            source.unget();
            token = ",";
        }
        break;
    case '"':
        token = "\"";
        escaped = false;
        while (true) {
            e = source.get();
            if (escaped) {
                token.push_back(e);
                escaped = false;
            }
            else if (e == '\\') {
                token.push_back(e);
                escaped = true;
            }
            else {
                if (e != EOF)
                    token.push_back(e);
                if (e == '"' || e == EOF)
                    return;
            }
        }
    case '#':
        token = c;
        token.push_back(source.get());
        if (token == "#(" || token == "#t" || token == "#f")
            return;
        read_identifier(source, token);
        break;
    default:
        token = c;
        read_identifier(source, token);
    }
}

// void parse(std::istream& stream, bool toplevel)
// {
//     Lexer lexer(stream);

//     std::string s;
//     while (lexer) {
//         lexer >> s;
//         std::cout << ">> " << s << std::endl;
//     }
