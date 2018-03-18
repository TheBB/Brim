#include <vector>

#include "object.h"

#include "gc.h"
#include "vm.h"


std::list<Frame> VM::_frames;

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

void VM::error(Object signal, Object payload)
{
    Object ret = Object::Error(signal, payload);
    _frames.front().set_error(ret);
}
