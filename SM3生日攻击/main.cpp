#include"SM3_basic.h"
#include<iostream>
#include<string>
#include<random>
#include<ctime>
#include <windows.h>
#include<vector>
#include <thread>
#include<fstream>

using namespace std;

static constexpr char CCH[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

class Attack {
private:
	SM3_basic test;
	string str;
	string result;
	int size;
public:
	//对str进行hash，取前size个半字节
	void init(string str_,int size) {
		str = str_;
		test.update(str);
		size = size;
		result = test.final().substr(0, size);
	}

	string get_result() {
		return result;
	}

	string get_str() {
		return str;
	}

	bool is_empty() {
		return str.empty();
	}

	bool empty() {
		bool result = str.empty();
		return result;
	}

};


//对Attack类型的等号重载
bool operator==(Attack& st1,Attack& st2)
{
	string s1 = st1.get_result();
	string s2 = st2.get_result();
	return (s1==s2);
}



//生成sz长的随机字符串
string makeRandStr(int sz)
{
	
	string ret;
	ret.resize(sz);
	std::mt19937 rng(std::random_device{}());
	int size = sizeof(CCH) - 1;
	int limit = sz - 1;
	int i;
	for (i = 0; i < sz; i+=2){
		uint32_t x = rng() % (size);
		uint32_t x1 = rng() % (size);
		ret[i] = CCH[x];
		ret[i + 1] = CCH[x1];
	}

	for (; i < sz; i++) {
		uint32_t x = rng() % (size);
		ret[i] = CCH[x];
	}

	return ret;
}


//对字符串进行排序
void sort_str(Attack* input, string* output, int n) {
	vector<string>strarray;
	int i, j = 0;
	for (int i = 0; i < n; i++) {
		strarray.push_back(input[i].get_result());
	}
	sort(strarray.begin(), strarray.end());
	vector<string> ::iterator st;
	for (st = strarray.begin(); st != strarray.end(); st++) {
		output[j++] = *st;
	}
}

bool find_equal = false;
//找到两个哈希值相同的元素
Attack* find_equal_string(Attack* test, long long start,long long end) {
	
	Attack* result = new Attack[2];
	for (int i = start; i < end; i++) {
		for (int j = i + 1; j < end; j++) {
			if (test[i] == test[j]) {
				result[0] = test[i];
				result[1] = test[j];
				find_equal = true;
				return result;
			}
		}
	}
	return nullptr;

}


void attack_init(Attack* test,int n,long long start,long long end) {
	int i;
	int limit = end - 1;
	for (i = start; i < limit; i += 2) {
		test[i].init(makeRandStr(56), n);
		test[i + 1].init(makeRandStr(56), n);
	}
	for (i; i < end; i++) {
		test[i].init(makeRandStr(56), n);
	}
}

//对前n个半字节进行生日攻击
void birthday_attack(int n,long long size) {
	Attack* test = new Attack[size];
	//两轮
	for (int k = 0; k < 2; k++) {
		//生成随机字符串
		int i;
		int limit = size - 1;
		for (i = 0; i < limit; i += 2) {
			test[i].init(makeRandStr(56), n);
			test[i + 1].init(makeRandStr(56), n);
		}
		for (i; i < size; i++) {
			test[i].init(makeRandStr(56), n);
		}
		
		Attack* result =  find_equal_string(test, 0, size);
		if (result!=nullptr) {
			cout << "*****碰撞结果*****" << endl;
			cout << "字符串1：" << result[0].get_str() << endl;
			cout << "字符串2：" << result[1].get_str() << endl;
			cout << "哈希结果1：" << result[0].get_result() << endl;
			cout << "哈希结果2：" << result[1].get_result() << endl;
			cout << "找到了" << n * 4 << "位的碰撞" << endl;
			return;
		}
		
		
	}
	cout << "失败" << endl;
	delete[] test;
}


int main() {
	clock_t start, end;
	int n = 12;
	long long size = long long(ceil(1.17 * pow(2, (n -1) * 2)));
	fstream myfile;
	myfile.open("C:\\Users\\86139\\Documents\\test123.txt", ios::out);
	for (int k = 0; k < 8; k++) {
		Attack* test = new Attack[size];
		start = clock();
		//birthday_attack(n, size);
		
		//int i;
		//int limit = size - 1;
		//for (i = 0; i < limit; i += 2) {
			//test[i].init(makeRandStr(56), n);
			//test[i + 1].init(makeRandStr(56), n);
		//}
		//for (i; i < size; i++) {
			//test[i].init(makeRandStr(56), n);
		//}
		thread t1(attack_init, test, n, long long(0), long long(size / 6));
		thread t2(attack_init, test, n, long long(size / 6), long long(size / 3));
		thread t3(attack_init, test, n, long long(size / 3), long long(size / 2));
		thread t4(attack_init, test, n, long long(size / 2), long long(size / 3 * 2));
		thread t5(attack_init, test, n, long long(size / 3 * 2), long long(size / 6 * 5));
		thread t6(attack_init, test, n, long long(size / 6 * 5), long long(size));
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		t6.join();

	

		if (myfile.is_open())
		{
			for (int j = 0; j < size; j++) {
				string s = test[j].get_result();

				string s1 = test[j].get_str();
				myfile << s1 << "\t" << ":" << s << "\n";
			}
		
		}
		delete[] test;
	}
	myfile.close();
	end = clock();
	double endtime = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "用时" << endtime << "秒" << endl;
	
	
}
	





	
