static unsigned int g_seed=0;
const int THRESHOLD = 2064;
#include <cmath>
#include "get_time.h"
#include "parse_command_line.h"
#include <cilk/reducer_opadd.h>
#include <iostream>
using namespace std;
template <typename T>
void quicksort(T* A, size_t n);
template <typename T>
void quicksort(T* A, size_t n, int low, int high);

template <typename T>
void prefixSum(T in[], int low, int high) {
	int lg=log2(high-low+1);
	for (int i=0; i<lg; ++i) {
		cilk_for (int j=low; j<=high-(1<<(i+1))+1; j+=(1<<(i+1))) {
			in[j+(1<<(i+1))-1]+=in[j+(1<<i)-1];
		}
	}
}

template <typename T>
pair<int, int> partition(T* A, int* ltIndex, int* gtIndex, int* eqIndex, T* ltVals, T* gtVals, T* eqVals, size_t n, int low, int high, T& value) {
	if (low>high) return {0,0};
	cilk::reducer_opadd<T> ltReducer;
	cilk::reducer_opadd<T> gtReducer;
	int sz=high-low+1;

	cilk_for (int i=low; i<=high; ++i) {
		if (A[i]<value) {
			ltIndex[i]=1;
			gtIndex[i]=0;
			eqIndex[i]=0;
			ltReducer+=1;
		} else if (A[i]>value) {
			gtIndex[i]=1;
			ltIndex[i]=0;
			eqIndex[i]=0;
			gtReducer+=1;
		} else {
			gtIndex[i]=0;
			ltIndex[i]=0;
			eqIndex[i]=1;
		}
	}

	int lt=ltReducer.get_value();
	int gt=gtReducer.get_value();
	int eq=sz-lt-gt;

	prefixSum(ltIndex, low, high);
	prefixSum(gtIndex, low, high);
	prefixSum(eqIndex, low, high);

	cilk_for(int i=low; i<=high; ++i) {
		if (A[i]<value) {
			int val=0, index=i+1-low;
			while (index) {
				val+=ltIndex[index-1+low];
				index-=index&(-1*index);
			}
			ltVals[val-1+low]=A[i];
		} else if (A[i]>value) {
			int val=0, index=i+1-low;
                        while (index) {
                                val+=gtIndex[index-1+low];
                                index-=index&(-1*index);
                        }
			gtVals[val-1+low+lt+eq]=A[i];
		} else {
			int val=0, index=i+1-low;
                        while (index) {
                                val+=eqIndex[index-1+low];
                                index-=index&(-1*index);
                        }
			eqVals[val-1+low+lt]=A[i];
		} 
	}
	
	cilk_for (int i=low; i<low+lt; ++i) {
		A[i]=ltVals[i];
	}
	cilk_for(int i=low+lt; i<low+lt+eq; ++i) {
		A[i]=eqVals[i];
	}
	cilk_for (int i=low+lt+eq; i<=high; ++i) {
		A[i]=gtVals[i];
	}
	return pair<int, int>{lt, lt+eq};
}

//https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
inline int fastrand() { 
  g_seed = (214013*g_seed+2531011); 
  return (g_seed>>16)&0x7FFF; 
} 

template <typename T>
void quicksort(T* A, int* ltIndex, int* gtIndex, int* eqIndex, T* ltVals, T* gtVals, T* eqVals, size_t n, int low, int high) {
	if (low>=high) return;
	
	int range = high-low+1;
	if (range<n/192) {
		sort(A+low, A+high+1);
		return;
	}
	T a=A[low+fastrand()%range]; T b=A[low+fastrand()%range]; T c=A[low+fastrand()%range];
	T median = max(min(a,b), min(max(a,b),c));
	pair<int, int> p;
	if (range<1000000000) {
		p.first = std::partition(A+low, A+high+1, [median](const auto& em){return em<median;})-A-low;
		p.second = std::partition(A+low+p.first, A+high+1, [median](const auto& em){return em==median;})-A-low;
	} else p = partition(A, ltIndex, gtIndex, eqIndex, ltVals, gtVals, eqVals, n, low, high, median);

	cilk_spawn quicksort(A, ltIndex, gtIndex, eqIndex, ltVals, gtVals, eqVals, n, low, low+p.first-1);
	cilk_spawn quicksort(A, ltIndex, gtIndex, eqIndex, ltVals, gtVals, eqVals, n, low+p.second, high);
	cilk_sync;
}

template <typename T>
void quicksort(T* A, size_t n) {
	int* ltIndex=new int[n];
	int* gtIndex=new int[n];
	int* eqIndex=new int[n];
	T* ltVals=new T[n];
	T* gtVals=new T[n];
	T* eqVals=new T[n];
	quicksort(A, ltIndex, gtIndex, eqIndex, ltVals, gtVals, eqVals, n, 0, n-1);

	delete[] ltIndex;
	delete[] gtIndex;
	delete[] eqIndex;
	delete[] ltVals;
	delete[] gtVals;
	delete[] eqVals;
}
