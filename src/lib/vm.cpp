#include <vector>

#include "object.h"

#include "gc.h"
#include "vm.h"


std::list<Frame> VM::_frames;
Object VM::_error;

Frame& VM::push_frame()
{
    Frame frame;
    _frames.push_front(frame);
    return _frames.front();
}

void VM::pop_frame()
{
    _frames.pop_front();
}

Object VM::String(const std::string& data)
{
    Object ret = Object::String(data);
    _frames.front().push(ret);
    return ret;
}

Object VM::Pair(Object car, Object cdr)
{
    Object ret = Object::Pair(car, cdr);
    _frames.front().push(ret);
    return ret;
}

Object VM::List(const std::vector<Object>& elements)
{
    GC::inhibit();

    Object head = Object::EmptyList;
    for (auto i = elements.rbegin(); i != elements.rend(); i++)
        head = Object::Pair(*i, head);
    _frames.front().push(head);

    GC::allow();
    return head;
}

Object VM::Vector(const std::vector<Object>& elements)
{
    Object ret = Object::Vector(elements.size());
    for (std::size_t i = 0; i < elements.size(); i++)
        ret[i] = elements[i];
    _frames.front().push(ret);
    return ret;
}

void Op::intern(const std::string& name)
{
    Object obj = Object::Symbol(name);
    VM::push(obj);
}

void Op::string(const std::string& data)
{
    Object obj = Object::String(data);
    VM::push(obj);
}

void Op::error()
{
    Object error = Object::Error(VM::peek(1), VM::peek(0));
    VM::set_error(error);
    VM::pop(2);
}

void Op::cons()
{
    VM::push(Object::Pair(VM::peek(1), VM::peek(0)), 2);
}

void Op::list(std::size_t nelems, bool fix_tail)
{
    if (fix_tail)
        VM::push(Object::EmptyList);
    for (; nelems > 0; nelems--)
        Op::cons();
}

void Op::vector(std::size_t nelems)
{
    Object vec = Object::Vector(nelems);
    for (std::size_t i = 0; i < nelems; i++)
        vec[i] = VM::peek(nelems - i - 1);
    VM::push(vec);
}

void Op::ret()
{
    Object retval = VM::peek();
    VM::pop_frame();
    VM::push(retval);
}
