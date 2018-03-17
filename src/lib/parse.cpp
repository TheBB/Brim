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

static std::string initials = "!$%&*/:<=>?~_^";
static std::string subsequents = ".+-";

static bool legal_symbol(std::string token)
{
    if (token == "+" || token == "-" || token == "...")
        return true;

    char init = token[0];
    if ((init < 'a' || init > 'z') && (init < 'A' || init > 'Z')
        && initials.find(init) == std::string::npos)
        return false;

    for (char c : token.substr(1))
        if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z')
            && initials.find(c) == std::string::npos && subsequents.find(c) == std::string::npos)
            return false;

    return true;
}

static Object read_datum(Lexer& source)
{
    if (!source)
        return Object::Undefined();

    std::string token;
    source >> token;

    if (token == "#t")
        return Object::True();
    else if (token == "#f")
        return Object::False();
    else if (token[0] == '"') {
        if (token.size() < 2 || token[token.size()-1] != '"')
            return Object::Undefined();
        token = token.substr(1, token.size()-2);
        std::string value;
        bool escaped = false;
        for (char c : token) {
            if (!escaped && c == '\\') {
                escaped = true;
                continue;
            }
            else if (!escaped)
                value.push_back(c);
            else if (escaped) {
                switch (c) {
                case '\\': case '"':
                    value.push_back(c); break;
                case 'n': value.push_back('\n'); break;
                case 't': value.push_back('\t'); break;
                default: return Object::Undefined();
                }
                escaped = false;
            }
        }
        return escaped ? Object::Undefined() : Object::String(value);
    }
    else if (token == "(") {
        Object head = Object::EmptyList(), tail;

        while (source && source.peek() != ")" && source.peek() != ".") {
            Object elt = read_datum(source);
            if (elt.type() == Type::Undefined)
                return Object::Undefined();
            if (head.type() == Type::EmptyList) {
                head = Object::Pair(elt, Object::EmptyList());
                tail = head;
            }
            else {
                tail.set_cdr(Object::Pair(elt, Object::EmptyList()));
                tail = tail.cdr();
            }
        }

        if (!source)
            return Object::Undefined();
        source >> token;

        if (token == ".") {
            Object elt = read_datum(source);
            if (elt.type() == Type::Undefined)
                return Object::Undefined();
            tail.set_cdr(elt);

            if (!source)
                return Object::Undefined();
            source >> token;
        }

        if (token != ")")
            return Object::Undefined();
        return head;
    }
    else if (token == "#(") {
        std::vector<Object> elements;

        while (source && source.peek() != ")") {
            Object elt = read_datum(source);
            if (elt.type() == Type::Undefined)
                return Object::Undefined();
            elements.push_back(elt);
        }

        if (!source)
            return Object::Undefined();
        source >> token;        // Closing parenthesis

        Object ret = Object::Vector(elements.size());
        for (std::size_t i = 0; i < elements.size(); i++)
            ret[i] = elements[i];
        return ret;
    }
    else if (legal_symbol(token))
        return Object::Symbol(token);
    else {
        std::cout << "Weird token: '" << token << "'" << std::endl;
        return Object::Undefined();
    }
}

void Parser::read()
{
    obj = read_datum(source);
}

// void parse(std::istream& stream, bool toplevel)
// {
//     Lexer lexer(stream);

//     std::string s;
//     while (lexer) {
//         lexer >> s;
//         std::cout << ">> " << s << std::endl;
//     }
