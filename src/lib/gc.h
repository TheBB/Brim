#include <list>
#include <stdlib.h>

#include "object.h"


#ifndef GC_H
#define GC_H


class GC
{
private:
    static std::list<Object> objects;
    static std::size_t limit;
    static std::size_t inhibitors;

public:
    template <typename T> static Object alloc() {
        if (objects.size() >= limit && inhibitors == 0)
            collect();

        T* t = new T;
        Object obj = Object(t);
        obj.set_mark(false);
        objects.push_front(obj);
        return obj;
    }

    static inline void inhibit() { inhibitors += 1; }
    static inline void allow() { inhibitors -= 1; }
    static inline std::size_t size() { return objects.size(); }

    static void collect();
};


#endif /* GC_H */
