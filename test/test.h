#ifndef TEST_H
#define TEST_H


#define assert_fixnum(obj,val) do {             \
        REQUIRE((obj).type() == Type::Fixnum);  \
        REQUIRE((obj).fixnum() == (val));       \
    } while(0)

#define assert_character(obj,val) do {                  \
        REQUIRE((obj).type() == Type::Character);       \
        REQUIRE((obj).character() == (val));            \
    } while(0)

#define assert_symbol(obj,val) do {             \
        REQUIRE((obj).type() == Type::Symbol);  \
        REQUIRE((obj).string() == (val));       \
    } while(0)

#define assert_string(obj,val) do {             \
        REQUIRE((obj).type() == Type::String);  \
        REQUIRE((obj).string() == (val));       \
    } while(0)

#define assert_vector(obj,length) do {          \
        REQUIRE((obj).type() == Type::Vector);  \
        REQUIRE((obj).size() == (length));      \
    } while(0)

#define assert_tostring(obj,val) do {           \
        std::ostringstream __str;               \
        __str << (obj);                         \
        REQUIRE(__str.str() == (val));          \
    } while(0)


#endif /* TEST_H */
