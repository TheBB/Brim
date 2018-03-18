#include <list>

#include "object.h"


#ifndef VM_H
#define VM_H


#define RETURN_IF_ERROR                                                 \
    do { if (VM::has_error()) return Object::Undefined; } while (0)


class Frame
{
private:
    std::list<Object> _stack;
    Object _error;

public:
    Frame() { };

    inline void push(Object obj) { _stack.push_front(obj); }
    inline Object pop() {
        Object ret = _stack.front();
        _stack.pop_front();
        return ret;
    }

    inline bool has_error() const { return _error.defined(); }
    inline void set_error(Object error) { _error = error; }
    inline Object get_error() const { return _error; }

    inline const std::list<Object> stack() const { return _stack; }
};

class VM
{
private:
    static std::list<Frame> _frames;

public:
    static Frame& push_frame();
    static void pop_frame();
    static inline const std::list<Frame>& frames() { return _frames; }

    static inline Object Fixnum(int64_t num) { return Object::Fixnum(num); }
    static inline Object Character(char c) { return Object::Character(c); }
    static inline Object Intern(const std::string& name) { return Object::Symbol(name); }
    static Object String(const std::string& data);
    static Object Pair(Object car, Object cdr);
    static Object List(const std::vector<Object>& elements);
    static Object Vector(const std::vector<Object>& elements);

    static void error(Object signal, Object payload);
    static inline bool has_error() { return _frames.front().has_error(); }
};


#endif /* VM_H */
