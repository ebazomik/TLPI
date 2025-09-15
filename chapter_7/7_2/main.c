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

		struct ek_chunk * new = (struct ek_chunk *)((char *) (current + sizeof(struct ek_chunk) + current->size + 1));
		new->free = 0;
		new->size = size;
		new->my_p = (void *)(new + 1);
		new->next = NULL;


}

int ek_free(void * p){
		// for now ek_free simply reset the brk to initial process break
		brk((void *) initial_brk);
		return 0;
}





int main(void){
		
		int * p = (int *) ek_alloc(sizeof(int));
		*p = 25;

		char * p2 = (char *) ek_alloc(6);
		strcpy(p2, "Ciao!\0");


		ek_free(p);
		return 0;
}
