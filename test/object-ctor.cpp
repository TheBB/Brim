#include "catch.h"
#include "test.h"

#include "object.h"


TEST_CASE("Fixnum constructor", "[object-ctor]") {
    Object obj;

    obj = Object::Fixnum(43);
    assert_fixnum(obj, 43);

    obj = Object::Fixnum(-12);
    assert_fixnum(obj, -12);
}

TEST_CASE("Character constructor", "[object-ctor]") {
    Object obj = Object::Character('u');
    assert_character(obj, 'u');
}

TEST_CASE("Symbol constructor", "[object-ctor]") {
    Object obj = Object::Symbol("alpha");
    assert_symbol(obj, "alpha");

    Object cmp = Object::Symbol("alpha");
    REQUIRE(cmp == obj);
}

TEST_CASE("String constructor", "[object-ctor]") {
    Object obj = Object::String("alpha");
    assert_string(obj, "alpha");
}

TEST_CASE("Pair constructor", "[object-ctor]") {
    Object obj = Object::Pair(
        Object::Symbol("a"),
        Object::Pair(Object::Symbol("b"), Object::EmptyList())
    );

    REQUIRE(obj.proper_list(2));
    assert_symbol(obj.nth(0), "a");
    assert_symbol(obj.nth(1), "b");
    assert_symbol(obj.car(), "a");
    assert_symbol(obj.cadr(), "b");
    REQUIRE(obj.cddr().type() == Type::EmptyList);
}

TEST_CASE("Vector constructor", "[object-ctor]") {
    Object obj = Object::Vector(4);
    obj[0] = Object::Symbol("a");
    obj[1] = Object::Symbol("b");
    obj[2] = Object::Symbol("c");
    obj[3] = Object::Symbol("d");

    assert_vector(obj, 4);
    assert_symbol(obj[0], "a");
    assert_symbol(obj[1], "b");
    assert_symbol(obj[2], "c");
    assert_symbol(obj[3], "d");
}
