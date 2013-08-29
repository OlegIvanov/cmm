#ifndef _gc_h
#define _gc_h

#define BLOCK_SIZE			4096

#define LOG_BLOCK_SIZE		12

#define LOG_TOP_SIZE		11

#define LOG_BOTTOM_SIZE		11

typedef struct GarbageCollector {
	void *top_index[LOG_BLOCK_SIZE];
} GarbageCollector;

#endif
