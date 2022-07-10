#pragma once
#include<string>
#include<iostream>
#include <sstream>
#include "TypeConvert.h"
using namespace std;

class SM3_basic
{
private:
	long long IV[8];
	long long result[8];

	TypeConvert type;


	//令param循环移n位
	long long rol(long long param, int n, char choice);

	//布尔函数FF
	long long FF(long long x, long long y, long long z, int j);

	//布尔函数GG
	long long GG(long long x, long long y, long long z, int j);

	//置换函数P0
	long long P0(long long x);

	//置换函数P1
	long long P1(long long x);

	//获取常数Tj
	long long get_Tj(int j);





public:
	SM3_basic() {
		IV[0] = 0x7380166f;
		IV[1] = 0x4914b2b9;
		IV[2] = 0x172442d7;
		IV[3] = 0xda8a0600;
		IV[4] = 0xa96f30bc;
		IV[5] = 0x163138aa;
		IV[6] = 0xe38dee4d;
		IV[7] = 0xb0fb0e4e;

		for (int i = 0; i < 8; i++) {
			result[i] = IV[i];
		}
	}

	SM3_basic(long long iv[8]) {
		for (int i = 0; i < 8; i++) {
			IV[i] = iv[i];
			result[i] = IV[i];
		}
	}



	//一次最多56个字节
	void update(string message) {
		long long m = pow(2, 32);


	//给IV赋值
		for (int i = 0; i < 8; i++) {
			IV[i] = result[i];
		}
	

	//消息填充（一个message__ascii是两个字）
		if (message.size() > 56)
			message = message.substr(0, 56);

		long long message_ascii[8] = { 0 };
		int message_size = message.size();
		message_ascii[7] = message_size;
		int num = message_size / 8;
		int size_current = num * 8;
		//将前几个填满
		for (int i = 0; i < num ; i++) {
			message_ascii[i] = type.stringtoascii(message.substr(8 * i, 8));
		}
		//对最后一个没满的进行消息填充
		if (num < 7) {
			string message_last = message.substr(size_current, message_size - size_current);
			long long ascii_last = type.stringtoascii(message_last);
			int x = 8 * (8 - message_last.size()) - 1;
			message_ascii[num] = ((ascii_last << 1) + 1) << x;
		}


	//消息扩展
		//将两个字大小的message_ascii分割为一个字大小的W
		long long W[67];
		long long W_another[63];
		long long tmp = pow(2, 33) - 1;
		for (int i = 0; i < 8; i++) {
			W[2 * i] = message_ascii[i] >> 32;//高32位
			W[2 * i + 1] = message_ascii[i] & tmp; //低32位
		}

		for (int i = 16; i < 67; i++) {
			W[i] = P1(W[i - 16] ^ W[i - 9] ^ rol(W[i - 3], 15, 'l')) ^ rol(W[i - 13], 7, 'l') ^ W[i - 6];
		}

		for (int i = 0; i < 63; i++) {
			W_another[i] = (W[i] ^ W[i + 4]);
		}


	//迭代压缩
		

		long long A = IV[0];
		long long B = IV[1];
		long long C = IV[2];
		long long D = IV[3];
		long long E = IV[4];
		long long F = IV[5];
		long long G = IV[6];
		long long H = IV[7];


		for (int i = 0; i < 63; i++) {
			long long SS1 = rol((rol(A, 12, 'l') + E + rol(get_Tj(i), i % 32, 'l')) % m, 7, 'l') ;
			long long SS2 = SS1 ^ rol(A, 12, 'l') ;
			long long TT1 = (FF(A, B, C, i) + D + SS2 + W_another[i]) % m;
			long long TT2 = (GG(E, F, G, i) + H + SS1 + W[i]) % m;
			D = C;
			C = rol(B, 9, 'l');
			B = A;
			A = TT1;
			H = G;
			G = rol(F, 19, 'l');
			F = E;
			E = P0(TT2);

		}
		result[0] = (A ^ IV[0]) ;
		result[1] = (B ^ IV[1]) ;
		result[2] = (C ^ IV[2]) ;
		result[3] = (D ^ IV[3]) ;
		result[4] = (E ^ IV[4]) ;
		result[5] = (F ^ IV[5]) ;
		result[6] = (G ^ IV[6]) ;
		result[7] = (H ^ IV[7]) ;

	}



	string final() {
		stringstream ss;
		string tmp;
		string res = "0x";
		for (int i = 0; i < 8; i++) {
			ss << hex << result[i] << endl;
			ss >> tmp;
			res += tmp;
		}
		return res;
	}



	long long* get_result_int() {
		long long* a = new long long[8];
		for (int i = 0; i < 8; i++) {
			a[i] = result[i];
		}
		return a;
	}
		
};

long long SM3_basic::rol(long long param, int n,char choice) {
	int size = 32;
	n = n % size;
	long long tmp = pow(2, size) - 1;
	if (choice == 'l')
		return (param >> (size - n) | (param << n)) & tmp;
	else if (choice == 'r')
		return (param << (size - n) | (param >> n)) & tmp;
	else
		return NULL;
}

long long SM3_basic::FF(long long x, long long y, long long z, int j) {
	if (j <= 15) {
		return x ^ y ^ z;
	}
	else {
		return (x & y) | (x & z) | (y & z);
	}
}

long long SM3_basic::GG(long long x, long long y, long long z, int j) {
	if (j <= 15) {
		return x ^ y ^ z;
	}

	else {
		return (x & y) | (~x & z);

	}

}

long long SM3_basic::P0(long long x) {
	return x ^ rol(x, 9, 'l') ^ rol(x, 17, 'l');
}

long long SM3_basic::P1(long long x) {
	return x ^ rol(x, 15, 'l') ^ rol(x, 23, 'l');

}

long long SM3_basic::get_Tj(int j) {
	if (j <= 15) 
		return 0x79cc4519;
	else
		return 0x7a879d8a;
}
