#ifndef _object_h
#define _object_h

#include <cmm/gc.h>

#define New(Type, Ptr)			Object_new(__GC__, sizeof(Type), (void **)&(Ptr))
#define Release(Obj)			Object_release(__GC__, (void *)(Obj))
#define Copy(LObj, RObj) 		Object_copy(__GC__, (void **)&(LObj), (void *)(RObj))
#define RetainCount(Obj)		Object_retain_count(__GC__, (void *)(Obj))

typedef struct ObjectHeader {
	uintptr_t ref_count;
} ObjectHeader;

#define hdr(obj)				((ObjectHeader *)obj - 1)
#define obj(hdr)				(hdr + 1)
#define rtn(hdr)				(hdr)->ref_count++
#define rls(hdr)				(hdr)->ref_count--

void Object_new(GC *gc, size_t type_size, void **obj);
void Object_release(GC *gc, void *obj);
void Object_copy(GC *gc, void **lobj, void *robj);
int Object_retain_count(GC *gc, void *obj);

#endif
