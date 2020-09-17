/*
 * CS 5007, Assignment 6
 * Summer 2019
 * Northeastern University, Seattle
 *
 * Author: Adrienne Slaughter
 * 8 Mar 2019
 * 
 */
#include "hello_world.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
	long a = -889262067;
	long b = 330223330;

    say_hello(a, b); 
    return 0;
}

void say_hello(long foo, long bar){
	printf("The magic code is: %X\n", (unsigned int)(foo + bar));
}
