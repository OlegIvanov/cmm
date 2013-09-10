#include "minunit.h"
#include <cmm/arp.h>

GC *gc;

#define ref(obj) RetainCount(obj)
#define ref_msg "Invalid reference count."

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
	C *c = NULL;
	mu_assert(ref(c) == -1, ref_msg);

	int i = 0;
	for(i = 0; i < 256; i++) {
		New(C, c);
	}

	return NULL;
}

char *test_gc_create_block_header()
{
	BlockHeader *hdr = GC_create_block_header(gc, 1);

	mu_assert(hdr->size == 32, "Invalid size.");

	uint32_t i = 0;
	for(i = 0; i < BLOCK_SZ / hdr->size / 8; i++) {
		mu_assert(hdr->marks[i] == UINT8_MAX, "Invalid mark value.");
	}

	free(hdr->marks);
	free(hdr);

	return NULL;
}

char *test_release_1()
{
	C *c = NULL;
	mu_assert(ref(c) == -1, ref_msg);

	New(C, c);
	mu_assert(ref(c) == 1, ref_msg);
	mu_assert(ref(c->F_B) == -1, ref_msg);

	New(B, c->F_B);
	mu_assert(ref(c->F_B) == 1, ref_msg);

	New(A, c->F_B->F_A);
	mu_assert(ref(c->F_B->F_A) == 1, ref_msg);
	
	Release(c->F_B);
	mu_assert(ref(c->F_B) == -1, ref_msg);
	mu_assert(ref(c->F_B->F_A) == -1, ref_msg);

	return NULL;
}

char *test_release_2()
{
	B *b = NULL;
	mu_assert(ref(b) == -1, ref_msg);

	New(B, b);
	mu_assert(ref(b) == 1, ref_msg);

	// cyclical reference
	Copy(b->F_A, b);
	mu_assert(ref(b) == 2, ref_msg);
	mu_assert(ref(b->F_A) == 2, ref_msg);

	Release(b);
	mu_assert(ref(b) == 1, ref_msg);

	return NULL;
}

char *test_copy_1()
{
	B *b = NULL;
	mu_assert(ref(b) == -1, ref_msg);

	New(B, b);
	mu_assert(ref(b) == 1, ref_msg);

	New(A, b->F_A);
	mu_assert(ref(b) == 1, ref_msg);
	mu_assert(ref(b->F_A) == 1, ref_msg);

	A *a = NULL;
	mu_assert(ref(a) == -1, ref_msg);

	Copy(a, b->F_A);
	mu_assert(ref(a) == 2, ref_msg);

	Copy(a, NULL);
	mu_assert(ref(a) == -1, ref_msg);
	mu_assert(ref(b->F_A) == 1, ref_msg);

	Copy(b->F_A, NULL);
	mu_assert(ref(b->F_A) == -1, ref_msg);

	return NULL;
}

char *test_copy_2()
{
	B *b = NULL;
	mu_assert(ref(b) == -1, ref_msg);

	New(B, b);
	mu_assert(ref(b) == 1, ref_msg);

	Copy(b, b);
	mu_assert(ref(b) == 1, ref_msg);

	B *b1 = NULL;
	mu_assert(ref(b1) == -1, ref_msg);

	Copy(b1, b);
	mu_assert(ref(b) == 2, ref_msg);
	mu_assert(ref(b1) == 2, ref_msg);

	Copy(b1, b1);
	mu_assert(ref(b1) == 2, ref_msg);

	return NULL;
}

char *test_arp_1()
{
	ARP()
		B *b = NULL;
		mu_assert(ref(b) == -1, ref_msg);

		New(B, b);
		mu_assert(ref(b) == 1, ref_msg);

		Autorelease(b);
		mu_assert(ref(b) == 1, ref_msg);

		New(A, b->F_A);
		mu_assert(ref(b) == 1, ref_msg);
		mu_assert(ref(b->F_A) == 1, ref_msg);
	}

	return NULL;
}

char *test_arp_2()
{
	A *a = NULL;
	mu_assert(ref(a) == -1, ref_msg);

	ARP()
		B *b = NULL;
		mu_assert(ref(b) == -1, ref_msg);

		New(B, b);
		mu_assert(ref(b) == 1, ref_msg);

		Autorelease(b);
		mu_assert(ref(b) == 1, ref_msg);

		New(A, b->F_A);
		mu_assert(ref(b) == 1, ref_msg);
		mu_assert(ref(b->F_A) == 1, ref_msg);

		Copy(a, b->F_A);
		mu_assert(ref(b) == 1, ref_msg);
		mu_assert(ref(b->F_A) == 2, ref_msg);
		mu_assert(ref(a) == 2, ref_msg);
	}

	mu_assert(ref(a) == 1, ref_msg);

	return NULL;
}

char *test_arp_3()
{
	ARP()
		B *b = NULL;
		mu_assert(ref(b) == -1, ref_msg);

		New(B, b);
		mu_assert(ref(b) == 1, ref_msg);

		New(A, b->F_A);
		mu_assert(ref(b) == 1, ref_msg);

		Autorelease(b);

		ARP()
			C *c = NULL;
			mu_assert(ref(c) == -1, ref_msg);

			New(C, c);
			mu_assert(ref(c) == 1, ref_msg);

			Copy(c->F_B, b);
			mu_assert(ref(b) == 2, ref_msg);

			Copy(c, NULL);
			mu_assert(ref(c) == -1, ref_msg);
			mu_assert(ref(b) == 1, ref_msg);

			Autorelease(b);
		}

		mu_assert(ref(b) == -1, ref_msg);
	}

	return NULL;
}

char *all_tests() {
	mu_suite_start();

	test_setup();

	mu_run_test(test_gc_init_size_map);
	mu_run_test(test_gc_init_obj_map);
	mu_run_test(test_gc_sweep);
	mu_run_test(test_gc_create_block_header);

	mu_run_test(test_release_1);
	mu_run_test(test_release_2);

	mu_run_test(test_copy_1);
	mu_run_test(test_copy_2);

	mu_run_test(test_arp_1);
	mu_run_test(test_arp_2);
	mu_run_test(test_arp_3);

	return NULL;
}

RUN_TESTS(all_tests);
