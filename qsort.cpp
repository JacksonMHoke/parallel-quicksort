#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "get_time.h"
#include "parse_command_line.h"
#include "qsort.h"
#include <algorithm>
using namespace std;

using data_type = size_t;

#define NUM_ROUNDS 5

inline uint64_t hash64(uint64_t u )
{
	uint64_t v = u * 3935559000370003845ul + 2691343689449507681ul;
	v ^= v >> 21;
	v ^= v << 37;
	v ^= v >>  4;
	v *= 4768777513237032717ul;
	v ^= v << 20;
	v ^= v >> 41;
	v ^= v <<  5;
	return v;
}

int main(int argc, char** argv) {
	commandLine P(argc, argv,
		"[-n num_elements]");
	int n = P.getOptionLongValue("-n", 100000000);
	
	data_type* A = new data_type[n];
	data_type* A_check = new data_type[n];
	cilk_for (int i = 0; i < n; i++) {
		A[i] = hash64(i);
		A_check[i] = A[i];
	}
	
	//correctness check, you can comment this out when you test performance
	std::sort(A_check, A_check+n);
	quicksort(A, n);
	cilk_for (int i = 0; i < n; i++) {
		if (A[i] != A_check[i]) {
			cout << "wrong answer at index: " << i << endl;
			exit(1);
		}
	}
	delete[] A_check;
	
	//first round, not counted in total time
	cilk_for (int i = 0; i < n; i++) {
		A[i] = hash64(i);
	}
	quicksort(A, n);
	double tot_time = 0.0;
	for (int round = 0; round < NUM_ROUNDS; round++) {
		cilk_for (int i = 0; i < n; i++) {
			A[i] = hash64(i);
		}
		timer t; t.start();
		quicksort(A, n);
		t.stop();
		double tm = t.get_total();
		cout << "Round " << round << ", time: " << tm << endl;
		tot_time += tm;
	}
	cout << "Average time: " << tot_time/NUM_ROUNDS << endl;
	
	return 0;
}
