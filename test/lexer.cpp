#include <iostream>
#include <sstream>

#include "catch.h"
#include "test.h"

#include "parse.h"


std::vector<std::string> tokenize(std::string code)
{
    std::istringstream stream(code);
    Lexer lexer(stream);
    std::vector<std::string> ret;
    while (lexer) {
        std::string token;
        lexer >> token;
        ret.push_back(token);
    }
    return ret;
}

TEST_CASE("Symbols", "[lexer]") {
    auto tokens = tokenize("a bcd ef");

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == "bcd");
    REQUIRE(tokens[2] == "ef");
}

TEST_CASE("Hash constructs", "[lexer]") {
    auto tokens = tokenize("#t #f #(");

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "#t");
    REQUIRE(tokens[1] == "#f");
    REQUIRE(tokens[2] == "#(");
}

TEST_CASE("Strings", "[lexer]") {
    auto tokens = tokenize("\"here's a string\"");

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0] == "\"here's a string\"");
}

TEST_CASE("String with escape sequence", "[lexer]") {
    auto tokens = tokenize("\"string with \\\" in it\"");

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0] == "\"string with \\\" in it\"");
}

TEST_CASE("Numbers", "[lexer]") {
    auto tokens = tokenize("123 -14 3.123 4.1e4 2.11e-2 -5.6 -8.1e3 -.66e-5");

    REQUIRE(tokens.size() == 8);
    REQUIRE(tokens[0] == "123");
    REQUIRE(tokens[1] == "-14");
    REQUIRE(tokens[2] == "3.123");
    REQUIRE(tokens[3] == "4.1e4");
    REQUIRE(tokens[4] == "2.11e-2");
    REQUIRE(tokens[5] == "-5.6");
    REQUIRE(tokens[6] == "-8.1e3");
    REQUIRE(tokens[7] == "-.66e-5");
}

TEST_CASE("Parens", "[lexer]") {
    auto tokens = tokenize("(()[]][)#(]");

    REQUIRE(tokens.size() == 10);
    REQUIRE(tokens[0] == "(");
    REQUIRE(tokens[1] == "(");
    REQUIRE(tokens[2] == ")");
    REQUIRE(tokens[3] == "[");
    REQUIRE(tokens[4] == "]");
    REQUIRE(tokens[5] == "]");
    REQUIRE(tokens[6] == "[");
    REQUIRE(tokens[7] == ")");
    REQUIRE(tokens[8] == "#(");
    REQUIRE(tokens[9] == "]");
}

TEST_CASE("Quotations", "[lexer]") {
    auto tokens = tokenize("'`,something,@list");

    REQUIRE(tokens.size() == 6);
    REQUIRE(tokens[0] == "'");
    REQUIRE(tokens[1] == "`");
    REQUIRE(tokens[2] == ",");
    REQUIRE(tokens[3] == "something");
    REQUIRE(tokens[4] == ",@");
    REQUIRE(tokens[5] == "list");
}
