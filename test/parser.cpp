#include <sstream>

#include "catch.h"
#include "test.h"

#include "object.h"
#include "parse.h"
#include "vm.h"


Object parse(std::string code)
{
    VM::push_frame();
    std::istringstream stream(code);
    parse_all(stream);

    Object retval = VM::has_error() ? VM::get_error() : VM::peek();
    VM::pop_frame();
    return retval;
}

TEST_CASE("Parse symbols", "[parser]") {
    auto objects = parse("onesym twosym");

    REQUIRE(objects.proper_list(2));
    assert_symbol(objects.nth(0), "onesym");
    assert_symbol(objects.nth(1), "twosym");
}

TEST_CASE("Parse booleans", "[parser]") {
    auto objects = parse("#t #f");

    REQUIRE(objects.proper_list(2));
    assert_boolean(objects.nth(0), true);
    assert_boolean(objects.nth(1), false);
}

TEST_CASE("Parse strings", "[parser]") {
    auto objects = parse("\"a string \\n \\t \\\\ \\\"\"");

    REQUIRE(objects.proper_list(1));
    assert_string(objects.nth(0), "a string \n \t \\ \"");
}

TEST_CASE("Parse empty list", "[parser]") {
    auto objects = parse("()");

    REQUIRE(objects.proper_list(1));
    REQUIRE(objects.nth(0).type() == Type::EmptyList);
}

TEST_CASE("Parse list", "[parser]") {
    auto objects = parse("(a b c)");

    REQUIRE(objects.proper_list(1));
    REQUIRE(objects.nth(0).proper_list(3));
    assert_symbol(objects.nth(0).nth(0), "a");
    assert_symbol(objects.nth(0).nth(1), "b");
    assert_symbol(objects.nth(0).nth(2), "c");
}

TEST_CASE("Parse list (explicit empty list)", "[parser]") {
    auto objects = parse("(a b c . ())");

    REQUIRE(objects.proper_list(1));
    REQUIRE(objects.nth(0).proper_list(3));
    assert_symbol(objects.nth(0).nth(0), "a");
    assert_symbol(objects.nth(0).nth(1), "b");
    assert_symbol(objects.nth(0).nth(2), "c");
}

TEST_CASE("Parse dotted list", "[parser]") {
    auto objects = parse("(a b c . d)");

    REQUIRE(objects.proper_list(1));
    REQUIRE(objects.nth(0).improper_list(3));
    assert_symbol(objects.nth(0).nth(0), "a");
    assert_symbol(objects.nth(0).nth(1), "b");
    assert_symbol(objects.nth(0).nth(2), "c");
    assert_symbol(objects.nth(0).cdddr(), "d");
}

TEST_CASE("Parse vector", "[parser]") {
    auto objects = parse("#(a b c)");

    REQUIRE(objects.proper_list(1));
    assert_vector(objects.nth(0), 3);
    assert_symbol(objects.nth(0)[0], "a");
    assert_symbol(objects.nth(0)[1], "b");
    assert_symbol(objects.nth(0)[2], "c");
}
