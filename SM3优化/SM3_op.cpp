#include<iostream>
#include<sstream>
#include<string>
#include<random>
#include<ctime>
#include<vector>
#include"SM3.h"
#include <intrin.h>
#include <thread>
using namespace std;

void test_thread(SM3_basic* test, string* str, int start, int end) {
	for (int i = start; i < end; i++) {
		test[i].update(str[i]);
	}
}

void test_() {
	SM3_basic test[1000];
	string str[1000] ;
	for (int i = 0; i < 1000; i++)
		str[i] = makeRandStr(56);
	thread first(test_thread, test,str,0,250);
	thread second(test_thread, test, str, 250, 500);
	thread third(test_thread, test, str, 500, 750);
	thread fourth(test_thread, test, str, 750, 1000);
	clock_t start, end;
	start = clock();
	first.join();
	second.join();
	third.join();
	fourth.join();
	end = clock();
	double endtime = (double)(end - start) / CLOCKS_PER_SEC;
	
	cout << "ÍÌÍÂÁ¿:" <<  1.0/endtime*1000*56/1000000  << "MB/s" << endl;



	
}



int main() {
	SM3_basic test1;
	test1.update("hello");
	cout << test1.final() << endl;

}
