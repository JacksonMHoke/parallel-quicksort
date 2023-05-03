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
#include <time.h>
using namespace std;

template <typename T>
void partitionSeq(T* A, size_t n, T& value) {
	int curr=0;
	for (int i=0; i<n; ++i) {
		if (A[i]<=value) {
			T temp=A[curr];
			A[curr]=A[i];
			A[i]=temp;
			curr++;
		}
	}
} 

int main () {
	srand(time(NULL));
	int SIZE=10000;
	int* A = new int[SIZE];
	int* B = new int[SIZE];
	for (int i=0; i<SIZE; ++i) {
		A[i]=rand()%51;
		B[i]=A[i];
	}
	
	//cout << "TESTING PREFIX SUM" << endl;

	//int in[]={1,2,3,4,5,6,7,8,9,10,11};
	//prefixSum(in, 11);
	//cout << "AFTER PREFIX SUM" << endl;
	//for (int i=0; i<11; ++i) {
	//	cout << in[i] << " ";
	//}
	//cout << endl;
	//cout.flush();
	//int numBelow=0, temp=A[SIZE/2];
	//for (int i=0; i<SIZE; ++i) {
	//	if (A[i]<=temp) numBelow++;
	//}

	//double partime = 0.0;
	//timer t; t.start();
	//partition(A, SIZE, temp);
	//t.stop();
	//partime=t.get_total();
	
	//double seqtime = 0.0;
	//timer t2; t2.start();
	//partitionSeq(B, SIZE, temp);
	//t2.stop();
	//seqtime=t2.get_total();

	//cout << "SEQUENTIAL TIME: " << seqtime << "\nPARALLEL TIME: " << partime << endl;

	//for (int i=0; i<numBelow; ++i) {
	//	if (A[i]>temp) {
	//		cout << i << " " << A[i] << " " << endl;
	//	}
	//}
	//cout << "FINISHED CHECKING FOR CORRECTNESS" << endl;
	//quicksort(A, SIZE);
	//for (int i=0; i<SIZE; ++i) cout << A[i] << " ";
	//cout << endl;
	quicksort(A, SIZE);
	for (int i=0; i<SIZE; ++i)
		cout << A[i] << " ";
	cout << endl;
	delete[] A;
	delete[] B;
}
