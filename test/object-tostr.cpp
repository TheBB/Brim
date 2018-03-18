#include <sstream>

#include "catch.h"
#include "test.h"

#include "object.h"
#include "vm.h"


TEST_CASE("Fixnum to-string", "[object-tostr]") {
    assert_tostring(VM::Fixnum(33), "33");
    assert_tostring(VM::Fixnum(0), "0");
    assert_tostring(VM::Fixnum(-1), "-1");
}

TEST_CASE("Character to-string", "[object-tostr]") {
    assert_tostring(VM::Character('u'), "#\\u");
}

TEST_CASE("Symbol to-string", "[object-tostr]") {
    assert_tostring(VM::Intern("beta"), "beta");
}

TEST_CASE("String to-string", "[object-tostr]") {
    VM::push_frame();
    assert_tostring(VM::String("beta"), "\"beta\"");
    VM::pop_frame();
}

TEST_CASE("Special to-string", "[object-tostr]") {
    assert_tostring(Object::False, "#f");
    assert_tostring(Object::True, "#t");
    assert_tostring(Object::EmptyList, "()");
    assert_tostring(Object::Undefined, "#<undefined>");
}

TEST_CASE("List to-string", "[object-tostr]") {
    VM::push_frame();
    Object obj;

    obj = VM::Pair(VM::Intern("a"), VM::Intern("b"));
    assert_tostring(obj, "(a . b)");

    obj = VM::Pair(
        VM::Intern("a"),
        VM::Pair(VM::Intern("b"), Object::EmptyList)
    );
    assert_tostring(obj, "(a b)");

    obj = VM::Pair(
        VM::Intern("a"),
        VM::Pair(VM::Intern("b"), VM::Intern("c"))
    );
    assert_tostring(obj, "(a b . c)");

    VM::pop_frame();
}

TEST_CASE("Vector to-string", "[object-tostr]") {
    VM::push_frame();
    Object obj = VM::Vector({VM::Intern("a"), VM::Intern("b"), VM::Intern("c"), VM::Intern("d")});
    assert_tostring(obj, "#(a b c d)");
    VM::pop_frame();
}
