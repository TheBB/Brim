#include <istream>

#include "object.h"


#ifndef PARSE_H
#define PARSE_H


class Lexer {
public:
    Lexer(std::istream& s) : source(s) { read(); }
    explicit operator bool() const { return token != ""; }

    Lexer& operator>>(std::string& s) {
        s = token;
        read();
        return *this;
    }

    const std::string& peek() const { return token; }

private:
    void read();

    std::istream& source;
    std::string token;
};


class Parser {
public:
    Parser(std::istream& s) : source(Lexer(s)) { read(); }
    explicit operator bool() const { return obj.type() != Type::Undefined; }

    Parser& operator>>(Object& tgt) {
        tgt = obj;
        read();
        return *this;
    }

private:
    void read();

    Lexer source;
    Object obj;
};


void parse(std::istream& stream, bool toplevel);


#endif /* PARSE_H */
