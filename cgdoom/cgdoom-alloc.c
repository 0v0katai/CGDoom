#include "cgdoom-alloc.h"
#include "z_zone.h"

/* We use -fstrict-volatile-bitfields to enforce the 32-bit access size. */
struct node;
typedef volatile struct node node_t;

struct node {
	/* Neighbors, or NULL at the head and tail of the list */
	node_t *prev, *next;
	/* Size of the block, in bytes */
	uint32_t size :24;
	/* Whether the block is free */
	uint32_t free :8;
};

/* First node of the list. */
static node_t *arena = NULL;
/* Bounds of the arena, used to find whether data has been allocated here. */
static void *arena_start, *arena_end;

/* Split a free node into two (if there's enough space for a second one). */
static void split(node_t *node, int size)
{
	int remainder = node->size - size;
	if(remainder < 32) return;

	node_t *right = (void *)node + sizeof(node_t) + size;
	right->prev = node;
	right->next = node->next;
	right->size = remainder - sizeof(node_t);
	right->free = 1;

	node->size = size;
	node->next = right;
	if(right->next) right->next->prev = right;
}

/* Merge this free node with the next one (also needs to be free). */
static void merge_with_next(node_t *node)
{
	if(!node->next) return;
	node->size += sizeof(node_t) + node->next->size;
	node->next = node->next->next;
	if(node->next) node->next->prev = node;
}

void CGD_PRAM_Init(void *start, void *end)
{
	arena = NULL;
	if(end - start < 256) return;

	arena = start;
	arena->prev = NULL;
	arena->next = NULL;
	arena->size = (end - start) - sizeof(node_t);
	arena->free = 1;

	arena_start = start;
	arena_end = end;
}

void *CGD_PRAM_Malloc(size_t size)
{
	node_t *candidate;
	size = (size + 3) & -4;

	/* Find a free block in the list */
	for(candidate = arena; candidate; candidate = candidate->next) {
		if(candidate->free && candidate->size >= size) break;
	}
	if(!candidate) return Z_Malloc(size, PU_STATIC, 0);

	/* Prepare and return that block */
	split(candidate, size);
	candidate->free = 0;
	return (void *)candidate + sizeof(node_t);
}

void CGD_PRAM_Free(void *ptr)
{
	if(!ptr) return;
	if(ptr < arena_start || ptr >= arena_end) return Z_Free(ptr);

	node_t *node = (void *)ptr - sizeof(node_t);
	node->free = 1;
	if(node->next && node->next->free) merge_with_next(node);
	if(node->prev && node->prev->free) merge_with_next(node->prev);
}

void *CGD_PRAM_Zalloc(size_t size)
{
	uint32_t *ptr = CGD_PRAM_Malloc(size);
	if(!ptr) return NULL;
	for(int i = 0; i < size / 4; i++) ptr[i] = 0;
	return ptr;
}
