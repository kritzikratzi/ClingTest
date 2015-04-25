//
//  main.c
//  ClingTest
//
//  Created by Hansi on 26.03.15.
//  Copyright (c) 2015 hansi. All rights reserved.
//

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <stdio.h>
#include <stdint.h>
#include "timeutil.h"

#include <cling/Interpreter/Interpreter.h>

//#include <clang/Sema/Sema.h>
inline void absTest( float * input, float * output, int N) {
	for( int i = 0; i < N; i++ ){
		output[i] = (input[i]<0?-input[i]:output[i]);
	}
}



int main(int argc, const char * argv[]) {
	// set up cling
	int fake_argc = 1;
	const char * fake_arg = "somethingsomething";
	const char * fake_argv[] = { fake_arg };
	const char * llvmBinary = "/Users/hansi/Downloads/cling-from-src/inst/";
	cling::Interpreter interpreter( fake_argc, fake_argv, llvmBinary );
	
	const int N = 512;
	float input[N];
	float output[N];
	
	for( int i = 0; i < N; i++ ){
		input[i] = -N/2 + i;
	}
	
	long startCompile = get_nanos();
	typedef void (*myFunc_t)(float *, float *, int);
	const char* code = "\
		extern \"C\" void absTest( float * input, float * output, int N) { \
			for( int i = 0; i < N; i++ ){\
				output[i] = (input[i]<0?-input[i]:output[i]); \
			}\
		}";
	long compileDuration = get_nanos() - startCompile;
	printf( "compile duration: %ld\n", compileDuration );
	
	myFunc_t myPrivFuncP
	= (myFunc_t) interpreter.compileFunction("absTest", code,
										 false /*ifUniq*/,
										 false /*withAccessControl*/);

	const int runs = 1000000;

	// test 1:  compute abs() of an array through cling
	unsigned long long start = get_nanos();
	for( int i = 0; i < runs; i++ ){
		(*myPrivFuncP)(input, output, N);
	}
	unsigned long long clingTime = get_nanos() - start;
	
	
	// test 2: compute abs() of an array with our precompiled method
	start = get_nanos();
	for( int i = 0; i < runs; i++ ){
		absTest(input, output, N);
	}
	unsigned long long compiledTime = get_nanos() - start;
	
	
	printf( "running abs() %d times through cling: %llu ms\n", runs, clingTime/1000/1000 );
	printf( "running abs() %d times precompiled: %llu ms\n", runs, compiledTime/1000/1000 );
    return 0;
}
