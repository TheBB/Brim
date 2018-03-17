#include <sstream>

#include "catch.h"
#include "test.h"

#include "object.h"
#include "parse.h"


std::vector<Object> parse(std::string code)
{
    std::istringstream stream(code);
    Parser parser(stream);
    std::vector<Object> ret;
    while (parser) {
        Object obj;
        parser >> obj;
        ret.push_back(obj);
    }
    return ret;
}

TEST_CASE("Parse symbols", "[parser]") {
    auto objects = parse("onesym twosym");

    REQUIRE(objects.size() == 2);
    assert_symbol(objects[0], "onesym");
    assert_symbol(objects[1], "twosym");
}

TEST_CASE("Parse booleans", "[parser]") {
    auto objects = parse("#t #f");

    REQUIRE(objects.size() == 2);
    assert_boolean(objects[0], true);
    assert_boolean(objects[1], false);
}

TEST_CASE("Parse strings", "[parser]") {
    auto objects = parse("\"a string \\n \\t \\\\ \\\"\"");

    REQUIRE(objects.size() == 1);
    assert_string(objects[0], "a string \n \t \\ \"");
}

TEST_CASE("Parse empty list", "[parser]") {
    auto objects = parse("()");

    REQUIRE(objects.size() == 1);
    REQUIRE(objects[0].type() == Type::EmptyList);
}

TEST_CASE("Parse list", "[parser]") {
    auto objects = parse("(a b c)");

    REQUIRE(objects.size() == 1);
    REQUIRE(objects[0].proper_list(3));
    assert_symbol(objects[0].nth(0), "a");
    assert_symbol(objects[0].nth(1), "b");
    assert_symbol(objects[0].nth(2), "c");
}

TEST_CASE("Parse dotted list", "[parser]") {
    auto objects = parse("(a b c . d)");

    REQUIRE(objects.size() == 1);
    REQUIRE(objects[0].improper_list(3));
    assert_symbol(objects[0].nth(0), "a");
    assert_symbol(objects[0].nth(1), "b");
    assert_symbol(objects[0].nth(2), "c");
    assert_symbol(objects[0].cdddr(), "d");
}

TEST_CASE("Parse vector", "[parser]") {
    auto objects = parse("#(a b c)");

    REQUIRE(objects.size() == 1);
    assert_vector(objects[0], 3);
    assert_symbol(objects[0][0], "a");
    assert_symbol(objects[0][1], "b");
    assert_symbol(objects[0][2], "c");
}
