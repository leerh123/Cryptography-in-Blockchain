#pragma once
#include<iostream>
#include<sstream>
#include<string>
#include<random>
#include<ctime>
using namespace std;

class TypeConvert
{
public:
	//str小于等于8字节
	long long stringtoascii(const string& str)
	{
		int size = str.size();
		string res_tmp = "0x";
		string tmp;
		stringstream ss;
		int i;
		for (i = 0; i < size; i++){
			ss << hex << int(str[i]) << endl;
			ss >> tmp;

			res_tmp = res_tmp + tmp  ;
		}
		long long result = strtoll(res_tmp.c_str(), NULL, 16);
		return result;
	}

	string asciitostring(long long a) {
		stringstream tmp;
		tmp << hex << a;
		string s;
		tmp >> s;
		int size = s.size();
		if (size % 2 != 0) {
			string s = "asciitostring:param is not a ascii code";
			cout << s << endl;
			throw s;
		}
		int num = size / 2;
		int* store = new int[num];
		for (int i = 0; i < num; i++) {
			store[i] = strtoll(s.substr(2 * i, 2).c_str(), NULL, 16);
		}
		string result;
		for (int i = 0; i < num; i++) {
			char ch = store[i];
			result.insert(result.size(), 1, ch);
		}
		delete[] store;

		return result;
	};
};

static constexpr char CCH[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

//生成sz长的随机字符串
string makeRandStr(int sz)
{

	string ret;
	ret.resize(sz);
	std::mt19937 rng(std::random_device{}());
	int size = sizeof(CCH) - 1;
	int limit = sz - 1;
	int i;
	for (i = 0; i < sz; i += 2) {
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