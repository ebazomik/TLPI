// todo (study)
// - add new POOL_SIZE if all poll is filled
// - align data before allocation (check info)

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define POOL_SIZE 1024

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

		if(prog_brk == NULL){
				prog_brk = sbrk(POOL_SIZE);
				initial_brk = prog_brk;
				start_chunk = (struct ek_chunk *) prog_brk;
				build_chunk(start_chunk, size, (void *) (start_chunk + 1), NULL);
				return start_chunk->my_p;
		}

		struct ek_chunk * current = start_chunk;

		while(current->next != NULL){
				current = current->next;
				if(current->free) break;
		}
	

		char* current_end = 
				(char *)current + sizeof(struct ek_chunk) + current->size;

		char* end_of_chunk = 
				(char *)initial_brk + POOL_SIZE;

		size_t space_remaining = 0;

		if(!current->free)
				space_remaining = end_of_chunk - current_end;

		size_t space_needed = sizeof(struct ek_chunk) + size;

		int write_to_current = 
				(space_needed <= sizeof(struct ek_chunk) + current->size) &&
				current->free;

		if(write_to_current){
				build_chunk(current, size, (void *) (current + 1), NO_NEXT);
				return current->my_p;
		}

		if(space_remaining >= space_needed){
				struct ek_chunk * new = (struct ek_chunk *) current_end;
				build_chunk(new, size,(void*) (new + 1), NULL);
				current->next = new;
				return new->my_p;
		} else {
				// increment brk
				printf("POOL_SIZE full, space remaining: %d\n", space_remaining);
				return NULL;
		}


}

int ek_free(void * p){
		struct ek_chunk * temp =
				(struct ek_chunk *) ((char *) p - sizeof(struct ek_chunk));
		temp->free = 1;
		temp->my_p = NULL;
		return 0;
}




int main(void){
		
		int * p = ek_alloc(sizeof(int));
		*p = 50;
		printf("p:\t%p -> %d\n", p, *p);

		int * p2 = ek_alloc(sizeof(int));
		*p2 = 100;
		printf("p2:\t%p -> %d\n", p2, *p2);

		int * p3 = ek_alloc(sizeof(int));
		*p3 = 150;
		printf("p3:\t%p -> %d\n", p3, *p3);

		ek_free(p2);

		int * p4 = ek_alloc(sizeof(int));
		*p4 = 200;
		printf("p4:\t%p -> %d\n", p4, *p4);


		return 0;
}
