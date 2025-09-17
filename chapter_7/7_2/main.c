// todo (study)
// - alloc in free struct if fund and if size io ok
// - add new POOL_SIZE if all poll is filled
// - align data before allocation (check info)

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define POOL_SIZE 1024

struct ek_chunk{
		short free;
		size_t size;
		void * my_p;
		struct ek_chunk * next;
};

static void * prog_brk = NULL;
static void * initial_brk = NULL;
struct ek_chunk * start_chunk = NULL;
static void * current_space_filled = NULL;


void * ek_alloc(size_t size){

		if(prog_brk == NULL){
				prog_brk = sbrk(POOL_SIZE);
				initial_brk = prog_brk;
				start_chunk = (struct ek_chunk *) prog_brk;
				start_chunk->free = 0;
				start_chunk->size = size;
				start_chunk->my_p = (void *) (start_chunk + 1);
				start_chunk->next = NULL;

				return start_chunk->my_p;
		}

		
		struct ek_chunk * current = start_chunk;
 

		while(current->next != NULL){
				current = current->next;
		}

		char* current_end = (char *)current + sizeof(struct ek_chunk) + current->size;
		char* end_of_chunk = (char *)initial_brk + POOL_SIZE;

		size_t space_remaining = end_of_chunk - current_end;
		size_t space_needed = sizeof(struct ek_chunk) + current->size;

		printf("space remaining %zu\nspace nedeed %zu\n", space_remaining, space_needed);


		if(space_remaining >= space_needed){
				struct ek_chunk * new = (struct ek_chunk *) current_end;
				new->free = 0;
				new->size = size;
				new->my_p = (void *)(new + 1);
				new->next = NULL;
				current->next = new;

				return new->my_p;
		} else {
				printf("POOL_SIZE full %d\n", space_remaining);
				return NULL;
		}


}

int ek_free(void * p){
		struct ek_chunk * temp = (struct ek_chunk *) p;
		temp->free = 1;
		return 0;
}





int main(void){
		
		int * p = ek_alloc(25);

		int * p2 = ek_alloc(1000);


		ek_free(p);
		return 0;
}
