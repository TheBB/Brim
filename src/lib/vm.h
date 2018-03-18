#include <iostream>
#include <vector>
#include <list>

#include "object.h"


#ifndef VM_H
#define VM_H


#define RETURN_IF_ERROR                                                 \
    do { if (VM::has_error()) return; } while (0)
#define RET_IF_ERROR                                                    \
    do { if (VM::has_error()) { Op::ret(); return; } } while (0)

#ifdef DEBUG
#define ASSERT(cond, msg)                               \
    if (!(cond)) { std::cerr << (msg) << std::endl; }
#else
#define ASSERT(cond, msg) ;
#endif


class Frame
{
private:
    std::vector<Object> _stack;

public:
    Frame() { };

    inline Object peek() {
        ASSERT(_stack.size() > 0, "peek(): stack size zero");
        return _stack.back();
    }
    inline Object peek(std::size_t index) { return *(_stack.end() - index - 1); }
    inline void push(Object obj) { _stack.push_back(obj); }
    inline void push(Object obj, std::size_t n) { pop(n); _stack.push_back(obj); }
    inline Object pop() {
        Object ret = _stack.back();
        _stack.pop_back();
        return ret;
    }
    inline void pop(std::size_t n) { _stack.erase(_stack.end() - n, _stack.end()); }

    inline const std::vector<Object>& stack() const { return _stack; }
};

class VM
{
private:
    static std::list<Frame> _frames;
    static Object _error;

public:
    // Frame manipulation
    static Frame& push_frame();
    static void pop_frame();
    static inline const std::list<Frame>& frames() { return _frames; }

    // Raw constructors
    static inline Object Fixnum(int64_t num) { return Object::Fixnum(num); }
    static inline Object Character(char c) { return Object::Character(c); }
    static inline Object Intern(const std::string& name) { return Object::Symbol(name); }
    static Object String(const std::string& data);
    static Object Pair(Object car, Object cdr);
    static Object List(const std::vector<Object>& elements);
    static Object Vector(const std::vector<Object>& elements);

    // Frame inspection
    static inline Object peek() { return _frames.front().peek(); }
    static inline Object peek(std::size_t index) { return _frames.front().peek(index); }
    static inline void push(Object obj) { _frames.front().push(obj); }
    static inline void push(Object obj, std::size_t n) { _frames.front().push(obj, n); }
    static inline Object pop() { return _frames.front().pop(); }
    static inline void pop(std::size_t n) { _frames.front().pop(n); }

    static inline bool has_error() { return _error.defined(); }
    static inline void set_error(Object error) { _error = error; }
    static inline Object get_error() { return _error; }

    static inline std::size_t stack_size() { return _frames.front().stack().size(); }
};

class Op
{
public:
    static void intern(const std::string& name);
    static void string(const std::string& data);

    static void error();
    static void cons();
    static void list(std::size_t nelems, bool fix_tail = true);
    static void vector(std::size_t nelems);

    static void ret();
};


#endif /* VM_H */
