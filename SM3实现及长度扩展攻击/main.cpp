#include"SM3_basic.h"
void test() {
	SM3_basic test;
	test.update("hellohash");
	string result = test.final();
	cout << result << endl;
}

int main() {
	SM3_basic a;
	a.update("hello");
	cout << a.final() << endl;
	long long* res = a.get_result_int();
	for (int i = 0; i < 8; i++) {
		cout << hex << res[i] << endl;
	}
	SM3_basic b(res);
	b.update("world");
	cout << b.final() << endl;
	SM3_basic c;
	c.update("hello");
	c.update("world");
	cout << c.final() << endl;
	
}