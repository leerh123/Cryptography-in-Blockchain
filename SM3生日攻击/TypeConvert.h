#pragma once
#include<iostream>
#include<sstream>
using namespace std;
class TypeConvert
{
public:
	//str小于等于8字节
	long long stringtoascii(const string& str)
	{
		if (str.size() > 8) {
			string s = "stringtoascii:param's length exceeds 8B";
			cout << s << endl;
			throw s;
		}
		string res_tmp = "0x";
		string tmp;
		stringstream ss;
		for (int i = 0; i < str.size(); i++)
		{
			ss << hex << int(str[i]) << endl;
			ss >> tmp;
			res_tmp += tmp;
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

