#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static jmp_buf env;

static void f1(){
		switch(setjmp(env)){
				case 0: 
						printf("setjmp case 0\n");
						break;
				case 1: 
						printf("Return from longjmp case 1\n");
						break;
				default: 
						printf("Return from case > 1\n");
						break;
		}
}

static void eating8Stack(){
		int big_buff[8192];
		for(int i = 0; i < sizeof(big_buff); i++){
				big_buff[i] = 0xCC;
		}
}

int main(void){
	
		f1();

		//Overwrite 8KB of stack memory to ensure that there is no more data relating to f1.
		eating8Stack();
	
		longjmp(env, 2);

		return 0;
}
