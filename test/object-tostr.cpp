#include <sstream>

#include "catch.h"
#include "test.h"

#include "object.h"


TEST_CASE("Fixnum to-string", "[object-tostr]") {
    assert_tostring(Object::Fixnum(33), "33");
    assert_tostring(Object::Fixnum(0), "0");
    assert_tostring(Object::Fixnum(-1), "-1");
}

TEST_CASE("Character to-string", "[object-tostr]") {
    assert_tostring(Object::Character('u'), "#\\u");
}

TEST_CASE("Symbol to-string", "[object-tostr]") {
    assert_tostring(Object::Symbol("beta"), "beta");
}

TEST_CASE("String to-string", "[object-tostr]") {
    assert_tostring(Object::String("beta"), "\"beta\"");
}

TEST_CASE("Special to-string", "[object-tostr]") {
    assert_tostring(Object::False(), "#f");
    assert_tostring(Object::True(), "#t");
    assert_tostring(Object::EmptyList(), "()");
    assert_tostring(Object::Undefined(), "#<undefined>");
}

TEST_CASE("List to-string", "[object-tostr]") {
    Object obj;

    obj = Object::Pair(Object::Symbol("a"), Object::Symbol("b"));
    assert_tostring(obj, "(a . b)");

    obj = Object::Pair(
        Object::Symbol("a"),
        Object::Pair(Object::Symbol("b"), Object::EmptyList())
    );
    assert_tostring(obj, "(a b)");

    obj = Object::Pair(
        Object::Symbol("a"),
        Object::Pair(Object::Symbol("b"), Object::Symbol("c"))
    );
    assert_tostring(obj, "(a b . c)");
}

TEST_CASE("Vector to-string", "[object-tostr]") {
    Object obj = Object::Vector(4);
    obj[0] = Object::Symbol("a");
    obj[1] = Object::Symbol("b");
    obj[2] = Object::Symbol("c");
    obj[3] = Object::Symbol("d");

    assert_tostring(obj, "#(a b c d)");
}
