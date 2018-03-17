#include <algorithm>
#include <iostream>
#include <stdlib.h>

#include "gc.h"


std::list<Frame> Frame::_frames;

Frame& Frame::make()
{
    Frame frame;
    _frames.push_front(frame);
    return _frames.front();
}


std::list<Object> GC::objects;
std::size_t GC::limit = 1000;
std::size_t GC::inhibitors = 0;

static void mark(Object obj)
{
    if (obj.immediate() || obj.marked())
        return;
    obj.set_mark(true);
    switch (obj.type()) {
    case Type::Pair:
        mark(obj.car());
        mark(obj.cdr());
        break;
    case Type::Vector:
        for (Object elt : obj) { mark(elt); }
        break;
    case Type::Error:
        mark(obj.signal());
        mark(obj.payload());
    default:
        break;
    }
}

void GC::collect()
{
    for (const Frame& frame : Frame::frames())
        for (Object obj : frame.stack())
            mark(obj);
    objects.remove_if(
        [] (Object obj) {
            bool destroy = !obj.marked();
            if (destroy)
                obj.destroy();
            return destroy;
        }
    );
    for (Object obj : objects)
        obj.set_mark(false);
}
