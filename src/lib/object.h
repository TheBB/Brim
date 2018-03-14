#include <map>
#include <ostream>
#include <string>
#include <vector>
#include <stdint.h>


#ifndef OBJECT_H
#define OBJECT_H

#define __FALSE 0x5
#define __TRUE 0xd
#define __EMPTYLIST 0x7
#define __UNDEFINED 0xf

enum class Type {
    Fixnum,                     //     0
    Character,                  //   011
    False,                      //  0101
    True,                       //  1101

    EmptyList,                  // 00111
    Undefined,                  // 01111

    Symbol, String, Pair, Vector,
};

struct Header {
    Type type;
    bool mark;
};

class Object
{
private:
    static std::map<std::string, Object> symtable;

public:
    static Object Fixnum(int64_t num) { return Object(2*num); }
    static Object Character(char c) { return Object((c << 3) | 0x3); }
    static Object False() { return Object(__FALSE); }
    static Object True() { return Object(__TRUE); }
    static Object EmptyList() { return Object(__EMPTYLIST); }
    static Object Undefined() { return Object(); }
    static Object Symbol(std::string name);
    static Object String(std::string data);
    static Object Pair(Object car, Object cdr);
    static Object Vector(uint64_t n);

private:
    uint64_t data;

    template <typename T> inline T* deref() const;

    inline void set_string(std::string data);

public:
    Object() : data(__UNDEFINED) { }
    Object(uint64_t data) : data(data) { }
    Object(void* data) : data((uint64_t)(data) + 1) { }

    uint64_t view() { return data; }

    Type type() const;
    inline void set_type(Type type);

    inline int64_t fixnum() const { return ((int64_t)data) / 2; }
    inline char character() const { return (char)(data >> 3); }

    inline const std::string string() const;

    inline Object car() const;
    inline Object cdr() const;
    inline void set_car(Object car);
    inline void set_cdr(Object cdr);

    inline std::size_t size() const;
    inline void set_size(std::size_t size);
    inline Object& operator[](std::size_t idx);
    inline const Object& operator[](std::size_t idx) const;

    inline std::vector<Object>::const_iterator begin();
    inline std::vector<Object>::const_iterator end();

    inline friend bool operator==(const Object lhs, const Object rhs);
    inline friend bool operator!=(const Object lhs, const Object rhs);

    bool proper_list(std::size_t nitems) const;
    bool proper_list(std::size_t min_items, std::size_t max_items) const;
    Object nth(std::size_t index) const;
};

struct Symbol_ {
    Header hdr;
    std::string name;
};

struct String_ {
    Header hdr;
    std::string data;
};

struct Pair_ {
    Header hdr;
    Object car;
    Object cdr;
};

struct Vector_ {
    Header hdr;
    std::vector<Object> array;
};

template <typename T> inline T* Object::deref() const { return (T*)(data - 1); }
inline void Object::set_type(Type type) { deref<Header>()->type = type; }

inline const std::string Object::string() const { return deref<String_>()->data; }
inline void Object::set_string(std::string name) { deref<String_>()->data = name; }

inline Object Object::car() const { return deref<Pair_>()->car; }
inline Object Object::cdr() const { return deref<Pair_>()->cdr; }
inline void Object::set_car(Object car) { deref<Pair_>()->car = car; }
inline void Object::set_cdr(Object cdr) { deref<Pair_>()->cdr = cdr; }

inline std::size_t Object::size() const { return deref<Vector_>()->array.size(); }
inline void Object::set_size(std::size_t size) { deref<Vector_>()->array.resize(size); }
inline Object& Object::operator[](std::size_t idx) { return deref<Vector_>()->array[idx]; }
inline const Object& Object::operator[](std::size_t idx) const { return deref<Vector_>()->array[idx]; }

inline std::vector<Object>::const_iterator Object::begin() { return deref<Vector_>()->array.begin(); }
inline std::vector<Object>::const_iterator Object::end() { return deref<Vector_>()->array.end(); }

inline bool operator==(const Object lhs, const Object rhs) { return lhs.data == rhs.data; }
inline bool operator!=(const Object lhs, const Object rhs) { return lhs.data != rhs.data; }

std::ostream& operator<<(std::ostream& out, Object obj);

#define FOR_LIST(obj, elt, tail)                \
    Object tail = obj;                          \
    Object elt = obj.car();                     \
    for(; tail.type() == Type::Pair;            \
        tail = tail.cdr(),                      \
        elt = tail.type() == Type::Pair ?       \
        tail.car() : Object::Undefined())       \

#endif /* OBJECT_H */
