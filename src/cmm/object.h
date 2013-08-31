#ifndef _object_h
#define _object_h

#include <cmm/gc.h>

#define New(Type, Ptr) Object_new(_gc_, sizeof(Type), &(Ptr))

void Object_new(GC gc, size_t size, void **obj);

#endif
