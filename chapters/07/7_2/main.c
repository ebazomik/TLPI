/*	
 *	Copyright (C) 2025 ebazomik
 *
 *	Simple implementation of linear memory allocation for study purpose.
 *
 *	This implementation creates a main pool of 8129 bytes and focuses on
 *	using virtual memory to manage allocation without calling the sbrk
 *	system call for each allocation.
 *  
 *	Licence: MIT
 */


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define POOL_SIZE 8192

struct ek_chunk{
		int free;
		size_t size;
		void * my_p;
		struct ek_chunk * next;
};

#define NO_NEXT ((struct ek_chunk *) 0xDEADBEEF)

static void * prog_brk = NULL;
static void * initial_brk = NULL;
struct ek_chunk * start_chunk = NULL;
static void * current_space_filled = NULL;

static void * build_chunk(
				struct ek_chunk * chunk,
				size_t size,
				void * m_pt,
				struct ek_chunk * nxt){

		chunk->free = 0;
		chunk->size = size;
		chunk->my_p = m_pt;
		if(nxt != NO_NEXT)
				chunk->next = nxt;
}

void * ek_alloc(size_t size){
		
		if(size == 0) return NULL;
		size_t real_size = sizeof(struct ek_chunk) + size;
		
		if(real_size > POOL_SIZE){
				
				printf("Virtual memory limit reached (%d) when attempting to allocate %d\n", POOL_SIZE, real_size);
				return NULL;
		
		}

		if(prog_brk == NULL){

				prog_brk = sbrk(POOL_SIZE);
				initial_brk = prog_brk;
				start_chunk = (struct ek_chunk *) prog_brk;
				build_chunk(start_chunk, size, (void *) (start_chunk + 1), NULL);
				
				return start_chunk->my_p;
		
		}

		struct ek_chunk * current = start_chunk;

		while(current->next != NULL){

				if(current->free) break;
				current = current->next;

		}

		size_t current_chunk_space = sizeof(struct ek_chunk) + current->size;
		char* current_end = (char *)current + current_chunk_space;
		char* end_pool = (char *)initial_brk + POOL_SIZE;
		size_t space_remaining = 0;

		if(!current->free) {

				space_remaining = end_pool - current_end;

		}

		size_t space_needed = sizeof(struct ek_chunk) + size;
		int write_to_current = (space_needed <= current_chunk_space) && current->free;

		if(write_to_current){

				build_chunk(current, size, (void *) (current + 1), NO_NEXT);

				return current->my_p;

		}

		if (space_remaining >= space_needed){

				struct ek_chunk * new = (struct ek_chunk *) current_end;
				build_chunk(new, size,(void*) (new + 1), NULL);
				current->next = new;

				return new->my_p;

		} else {

				printf("Can't allocate %d in remaing vistual space %d\n", space_needed, space_remaining);

				return NULL;
		}
}


int ek_free(void * p){
		
		if(p == NULL) return 1;

		struct ek_chunk * temp = (struct ek_chunk *) ((char *) p - sizeof(struct ek_chunk));
		temp->free = 1;
		temp->my_p = NULL;

		return 0;
}


int main(void){
		
		int * p = ek_alloc(4096);
		if(p != NULL)
				*p = 200;
				printf("p:\t%p -> %d\n", p, *p);

		char * p2 = ek_alloc(8192);
		if(p2 != NULL){
				strcpy(p2, "Hi, this is message allocated to p2");
				printf("p2:\t%p -> %s\n", p2, p2);
		}

		int * p3 = ek_alloc(sizeof(int));
		if(p3 != NULL){
				*p3 = 300;
				printf("p4:\t%p -> %d\n", p3, *p3);
		}

		return 0;
}
