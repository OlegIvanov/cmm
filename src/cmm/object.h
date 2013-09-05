#ifndef _object_h
#define _object_h

#include <cmm/gc.h>

#define New(Type, Ptr)			Object_new(__GC__, sizeof(Type), (void **)&(Ptr))
#define Release(Obj)			Object_release((void *)Obj)

#define Copy(LObj, RObj) \
if(RObj == NULL) { \
	void *__RObj__ = NULL; \
	Object_retain((void **)&(LObj), (void **)&(__RObj__)); \
} else { \
	void *__RObj__ = RObj; \
	Object_retain((void **)&(LObj), (void **)&(__RObj__)); \
}

void Object_new(GC *gc, size_t type_size, void **obj);
void Object_release(void *obj);
void Object_retain(void **lobj, void **robj);

#endif
