#include <deque>
#include <istream>


#ifndef PARSE_H
#define PARSE_H


class Lexer {
public:
    Lexer(std::istream& s) : source(s) { read(); }

    explicit operator bool() const {
        return token != "";
    }

    Lexer& operator>>(std::string& s) {
        s = token;
        token = "";
        read();

        return *this;
    }

private:
    void read();

    std::istream& source;
    std::string token;
};


void parse(std::istream& stream, bool toplevel);


#endif /* PARSE_H */
