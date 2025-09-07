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

static void f2(int val){
	longjmp(env, val);
}

int main(void){
	
	f1();
	f2(1);
	
	//longjmp(env, 1);
	
	return 0;
}
