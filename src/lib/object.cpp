#include <iostream>

#include "gc.h"

#include "object.h"


std::map<std::string, Object> Object::symtable;


Object Object::Symbol(std::string name)
{
    auto it = symtable.find(name);
    if (it != symtable.end())
        return it->second;

    Object obj = GC::alloc<Symbol_>();
    obj.set_type(Type::Symbol);
    obj.set_string(name);
    symtable[name] = obj;
    return obj;
}

Object Object::String(std::string data)
{
    Object obj = GC::alloc<String_>();
    obj.set_type(Type::String);
    obj.set_string(data);
    return obj;
}

Object Object::Pair(Object car, Object cdr)
{
    Object obj = GC::alloc<Pair_>();
    obj.set_type(Type::Pair);
    obj.set_car(car);
    obj.set_cdr(cdr);
    return obj;
}

Object Object::Vector(uint64_t size)
{
    Object obj = GC::alloc<Vector_>();
    obj.set_type(Type::Vector);
    obj.set_size(size);
    return obj;
}

Type Object::type() const
{
    switch (data & 0x7) {
    case 0x0: case 0x2: case 0x4: case 0x6: return Type::Fixnum;
    case 0x1: return deref<Header>()->type;
    case 0x3: return Type::Character;
    case 0x5: return data == __FALSE ? Type::False : Type::True;
    }

    switch (data) {
    case __EMPTYLIST: return Type::EmptyList;
    }

    return Type::Undefined;
}

std::ostream& operator<<(std::ostream& out, Object obj)
{
    switch (obj.type()) {
    case Type::Fixnum: out << obj.fixnum(); break;
    case Type::Character: out << "#\\" << obj.character(); break;
    case Type::False: out << "#f"; break;
    case Type::True: out << "#t"; break;
    case Type::EmptyList: out << "()"; break;
    case Type::Undefined: out << "#<undefined>"; break;
    case Type::Symbol: out << obj.string(); break;
    case Type::String: out << '"' << obj.string() << '"'; break;
    case Type::Vector: {
        out << "#(";
        bool first = true;
        for (Object elt : obj) {
            if (!first) out << " ";
            out << elt;
            first = false;
        }
        out << ")";
        break;
    }
    case Type::Pair: {
        out << "(";
        bool first = true;
        FOR_LIST(obj, elt, tail)
        {
            if (!first) out << " ";
            out << elt;
            first = false;
        }
        if (tail != Object::EmptyList())
            out << " . " << tail;
        out << ")";
        break;
    }
    }

    return out;
}

bool Object::proper_list(std::size_t nitems) const {
    return proper_list(nitems, nitems);
}

bool Object::proper_list(std::size_t min_items, std::size_t max_items) const
{
    Object obj = *this;
    std::size_t nitems = 0;
    while (obj.type() == Type::Pair) {
        nitems++;
        obj = obj.cdr();
    }

    if (obj.type() != Type::EmptyList)
        return false;
    return min_items <= nitems && nitems <= max_items;
}

Object Object::nth(std::size_t index) const
{
    Object obj = *this;
    for (; index > 0; index--)
        obj = obj.cdr();
    return obj.car();
}
