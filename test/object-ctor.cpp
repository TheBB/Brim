#include "catch.h"
#include "test.h"

#include "object.h"
#include "vm.h"


TEST_CASE("Fixnum constructor", "[object-ctor]") {
    Object obj;

    obj = VM::Fixnum(43);
    assert_fixnum(obj, 43);

    obj = VM::Fixnum(-12);
    assert_fixnum(obj, -12);
}

TEST_CASE("Character constructor", "[object-ctor]") {
    Object obj = VM::Character('u');
    assert_character(obj, 'u');
}

TEST_CASE("Symbol constructor", "[object-ctor]") {
    Object obj = VM::Intern("alpha");
    assert_symbol(obj, "alpha");

    Object cmp = VM::Intern("alpha");
    REQUIRE(cmp == obj);
}

TEST_CASE("String constructor", "[object-ctor]") {
    VM::push_frame();

    Object obj = VM::String("alpha");
    assert_string(obj, "alpha");

    VM::pop_frame();
}

TEST_CASE("Pair constructor", "[object-ctor]") {
    VM::push_frame();

    Object obj = VM::Pair(
        VM::Intern("a"),
        VM::Pair(VM::Intern("b"), Object::EmptyList)
    );

    REQUIRE(obj.proper_list(2));
    assert_symbol(obj.nth(0), "a");
    assert_symbol(obj.nth(1), "b");
    assert_symbol(obj.car(), "a");
    assert_symbol(obj.cadr(), "b");
    REQUIRE(obj.cddr().type() == Type::EmptyList);

    VM::pop_frame();
}

TEST_CASE("Vector constructor", "[object-ctor]") {
    VM::push_frame();

    std::vector<Object> objs = {
        VM::Intern("a"),
        VM::Intern("b"),
        VM::Intern("c"),
        VM::Intern("d")
    };
    Object obj = VM::Vector(objs);

    assert_vector(obj, 4);
    assert_symbol(obj[0], "a");
    assert_symbol(obj[1], "b");
    assert_symbol(obj[2], "c");
    assert_symbol(obj[3], "d");

    VM::pop_frame();
}
