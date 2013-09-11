#include "minunit.h"
#include <cmm/arp.h>

GC *gc;

#define count(obj) 			RetainCount(obj)
#define count_msg			"Invalid reference count."

void test_setup()
{
	gc = __GC__;
}

typedef struct A {
	char F_char;
} A;

typedef struct B {
	char F_char;
	A *F_A;
} B;

typedef struct C {
	char F_char;
	B *F_B;
} C;

char *test_gc_init_size_map()
{
	mu_assert(SIZE_SZ == 9, "Invalid SIZE_SZ value.");

	mu_assert(gc->size_map[0] == 16, "Invalid min. object size.");
	mu_assert(gc->size_map[SIZE_SZ - 1] == 4096, "Invalid max. object size.");

	return NULL;
}

char *test_gc_init_obj_map()
{	
	mu_assert(MAX_BLOCK_OFFSET_WORDS_SZ == 1024, "Invalid max_block_offset_words_sz value.");

	int i = 0;

	for(i = 0; i < SIZE_SZ; i++) {
		uint16_t offset = *(gc->obj_map + i * MAX_BLOCK_OFFSET_WORDS_SZ);
		mu_assert(offset == 0, "Invalid offset value.");

		offset = *(gc->obj_map + (i + 1) * MAX_BLOCK_OFFSET_WORDS_SZ - 1);
		uint16_t obj_max_offset = gc->size_map[i] / sizeof(uintptr_t) - 1;

		mu_assert(offset == obj_max_offset, "Invalid offset value.");
	}

	return NULL;
}

char *test_gc_sweep()
{
	size_t objsize = sizeof(C) + sizeof(ObjectHeader);
	int size_index = GC_get_size(gc, objsize);

	int objects_number = BLOCK_SZ / gc->size_map[size_index];
	mu_assert(objects_number == 256, "Wrong number of objects.");

	C *c = NULL;	
	C *temp = NULL;

	New(C, c);
	
	// Not retain! Just simply copy pointer.
	temp = c;

	int i = 0;
	for(i = 1; i < objects_number; i++) {
		New(C, c);
	}

	New(C, c);
	mu_assert(c == temp, "Should be equal to address of the beginning of the block.");

	return NULL;
}

char *test_release_1()
{
	C *c = NULL;
	mu_assert(count(c) == -1, count_msg);

	New(C, c);
	mu_assert(count(c) == 1, count_msg);
	mu_assert(count(c->F_B) == -1, count_msg);

	New(B, c->F_B);
	mu_assert(count(c->F_B) == 1, count_msg);

	New(A, c->F_B->F_A);
	mu_assert(count(c->F_B->F_A) == 1, count_msg);
	
	Release(c->F_B);
	mu_assert(count(c->F_B) == -1, count_msg);
	mu_assert(count(c->F_B->F_A) == -1, count_msg);

	return NULL;
}

char *test_release_2()
{
	B *b = NULL;
	mu_assert(count(b) == -1, count_msg);

	New(B, b);
	mu_assert(count(b) == 1, count_msg);

	// cyclical reference
	Copy(b->F_A, b);
	mu_assert(count(b) == 2, count_msg);
	mu_assert(count(b->F_A) == 2, count_msg);

	Release(b);
	mu_assert(count(b) == 1, count_msg);

	return NULL;
}

char *test_copy_1()
{
	B *b = NULL;
	mu_assert(count(b) == -1, count_msg);

	New(B, b);
	mu_assert(count(b) == 1, count_msg);

	New(A, b->F_A);
	mu_assert(count(b) == 1, count_msg);
	mu_assert(count(b->F_A) == 1, count_msg);

	A *a = NULL;
	mu_assert(count(a) == -1, count_msg);

	Copy(a, b->F_A);
	mu_assert(count(a) == 2, count_msg);

	Copy(a, NULL);
	mu_assert(count(a) == -1, count_msg);
	mu_assert(count(b->F_A) == 1, count_msg);

	Copy(b->F_A, NULL);
	mu_assert(count(b->F_A) == -1, count_msg);

	return NULL;
}

char *test_copy_2()
{
	B *b = NULL;
	mu_assert(count(b) == -1, count_msg);

	New(B, b);
	mu_assert(count(b) == 1, count_msg);

	Copy(b, b);
	mu_assert(count(b) == 1, count_msg);

	B *b1 = NULL;
	mu_assert(count(b1) == -1, count_msg);

	Copy(b1, b);
	mu_assert(count(b) == 2, count_msg);
	mu_assert(count(b1) == 2, count_msg);

	Copy(b1, b1);
	mu_assert(count(b1) == 2, count_msg);

	return NULL;
}

char *test_arp_1()
{
	ARP()
		B *b = NULL;
		mu_assert(count(b) == -1, count_msg);

		New(B, b);
		mu_assert(count(b) == 1, count_msg);

		Autorelease(b);
		mu_assert(count(b) == 1, count_msg);

		New(A, b->F_A);
		mu_assert(count(b) == 1, count_msg);
		mu_assert(count(b->F_A) == 1, count_msg);
	}

	return NULL;
}

char *test_arp_2()
{
	A *a = NULL;
	mu_assert(count(a) == -1, count_msg);

	ARP()
		B *b = NULL;
		mu_assert(count(b) == -1, count_msg);

		New(B, b);
		mu_assert(count(b) == 1, count_msg);

		Autorelease(b);
		mu_assert(count(b) == 1, count_msg);

		New(A, b->F_A);
		mu_assert(count(b) == 1, count_msg);
		mu_assert(count(b->F_A) == 1, count_msg);

		Copy(a, b->F_A);
		mu_assert(count(b) == 1, count_msg);
		mu_assert(count(b->F_A) == 2, count_msg);
		mu_assert(count(a) == 2, count_msg);
	}

	mu_assert(count(a) == 1, count_msg);

	return NULL;
}

char *test_arp_3()
{
	ARP()
		B *b = NULL;
		mu_assert(count(b) == -1, count_msg);

		New(B, b);
		mu_assert(count(b) == 1, count_msg);

		New(A, b->F_A);
		mu_assert(count(b) == 1, count_msg);

		Autorelease(b);

		ARP()
			C *c = NULL;
			mu_assert(count(c) == -1, count_msg);

			New(C, c);
			mu_assert(count(c) == 1, count_msg);

			Copy(c->F_B, b);
			mu_assert(count(b) == 2, count_msg);

			Copy(c, NULL);
			mu_assert(count(c) == -1, count_msg);
			mu_assert(count(b) == 1, count_msg);

			Autorelease(b);
		}

		mu_assert(count(b) == -1, count_msg);
	}

	return NULL;
}

char *test_arp_4()
{
	A *a = NULL;
	mu_assert(count(a) == -1, count_msg);

	ARP()
		New(A, a);
		mu_assert(count(a) == 1, count_msg);

		Autorelease(a);
		goto trying_to_avoid_releasing;
	}

trying_to_avoid_releasing:
	mu_assert(count(a) == -1, count_msg);

	return NULL;
}

A *Create_A()
{
	A *a = NULL;
	mu_assert(count(a) == -1, count_msg);

	New(A, a);
	mu_assert(count(a) == 1, count_msg);

	Autorelease(a);

	return a;
}

char *test_arp_5()
{
	A *a = NULL;
	mu_assert(count(a) == -1, count_msg);

	ARP()
		a = Create_A();
		mu_assert(count(a) == 1, count_msg);
	}

	mu_assert(count(a) == -1, count_msg);
	
	return NULL;
}

char *all_tests() {
	mu_suite_start();

	GC_CREATE();

	test_setup();

	mu_run_test(test_gc_init_size_map);
	mu_run_test(test_gc_init_obj_map);

	mu_run_test(test_gc_sweep);

	mu_run_test(test_release_1);
	mu_run_test(test_release_2);

	mu_run_test(test_copy_1);
	mu_run_test(test_copy_2);

	mu_run_test(test_arp_1);
	mu_run_test(test_arp_2);
	mu_run_test(test_arp_3);
	mu_run_test(test_arp_4);
	mu_run_test(test_arp_5);

	GC_DESTROY();

	return NULL;
}

RUN_TESTS(all_tests);
