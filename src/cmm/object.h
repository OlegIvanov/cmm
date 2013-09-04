#ifndef _object_h
#define _object_h

#include <cmm/gc.h>

#define New(Type, Ptr)			Object_new(__GC__, sizeof(Type), (void **)&(Ptr))
#define Copy(LObj, RObj)		Object_retain((void **)&(LObj),(void **)&(RObj))
#define Release(Obj)			Object_release((void *)Obj)

void Object_new(GC *gc, size_t size, void **obj);
void Object_release(void *obj);
int Object_retain(void **lobj, void **robj);

#endif
