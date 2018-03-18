#include <istream>

#include "object.h"


#ifndef PARSE_H
#define PARSE_H


class Token {
public:
    Token() : value(""), pos(0) { }
    Token(std::size_t pos) : pos(pos) { }
    Token(std::string value, std::size_t pos) : value(value), pos(pos) { }
    inline std::size_t position() const { return pos; }
    inline const std::string& string() const { return value; }

    inline friend bool operator==(const Token& lhs, const std::string& rhs);
    inline friend bool operator!=(const Token& lhs, const std::string& rhs);
    inline Token& operator=(const std::string& rhs) { value = rhs; return *this; }

    inline void push_back(char c) { value.push_back(c); }
    inline char operator[](std::size_t idx) const { return value[idx]; }
    inline std::size_t size() const { return value.size(); }
    inline Token substr(std::size_t start = 0, std::size_t len = std::string::npos) const {
        return Token(value.substr(start, len), pos + start);
    }

    inline std::string::const_iterator begin() const { return value.begin(); }
    inline std::string::const_iterator end() const { return value.end(); }

private:
    std::string value;
    std::size_t pos;
};

inline bool operator==(const Token& lhs, const std::string& rhs) { return lhs.value == rhs; }
inline bool operator!=(const Token& lhs, const std::string& rhs) { return lhs.value != rhs; }

std::ostream& operator<<(std::ostream& out, Token& token);


class Lexer {
public:
    Lexer(std::istream& s) : pos(0), source(s) { read(); }
    explicit operator bool() const { return token != ""; }

    Lexer& operator>>(Token& t) {
        t = token;
        read();
        return *this;
    }

    const Token& peek() const { return token; }

private:
    void read();
    inline char get() { pos++; return source.get(); }
    inline void unget() { pos--; source.unget(); }

    std::size_t pos;
    std::istream& source;
    Token token;
};


class Parser {
public:
    Parser(std::istream& s) : source(Lexer(s)) { read(); }
    explicit operator bool() const { return obj.defined(); }

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


std::vector<Object> parse_all(std::istream& stream);
Object parse_toplevel(std::istream& stream);
Object parse_single(std::istream& stream);


#endif /* PARSE_H */
