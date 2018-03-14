#include <list>
#include <stdlib.h>

#include "object.h"


#ifndef GC_H
#define GC_H

class GC
{
private:
    static std::list<Object> objects;

public:
    template <typename T> static Object alloc() {
        T* t = new T;
        Object obj = Object(t);
        objects.push_back(obj);
        return obj;
    }
};

#endif /* GC_H */
