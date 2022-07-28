#pragma once
#include<string>
#include<iostream>
#include <sstream>
#include "TypeConvert.h"
#include <intrin.h>
#define choice123 1
using namespace std;

//循环左移
__m256i _mm256_rl_epi32(__m256i a, int b) {
	return _mm256_xor_si256(_mm256_slli_epi32(a, b), _mm256_srli_epi32(a, 32 - b));
}

//P1函数
__m256i _mm256_P1_epi32(__m256i a) {
	__m256i tmp = _mm256_xor_si256(a, _mm256_rl_epi32(a, 15));
	return  _mm256_xor_si256(tmp, _mm256_rl_epi32(a, 23));
}


class SM3_basic
{
private:
	int IV[8];
	int result[8];

	TypeConvert type;


	//令param循环移n位
	int rol(int param, int n, char choice);

	//布尔函数FF
	int FF(int x, int y, int z, int j);

	//布尔函数GG
	int GG(int x, int y, int z, int j);

	//置换函数P0
	int P0(int x);

	//置换函数P1
	int P1(int x);

	//获取常数Tj
	int get_Tj(int j);





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

		for (int i = 0; i < 8; i++)
			result[i] = IV[i];
	}

	SM3_basic(long long iv[8]) {
		for (int i = 0; i < 8; i++) {
			IV[i] = iv[i];
			result[i] = IV[i];
		}

	}


#if choice123==1
	//一次最多56个字节
	void update(string message) {
		long long m = pow(2, 32);


		//给IV赋值
		for (int i = 0; i < 8; i++)
			IV[i] = result[i];


		//消息填充（一个message__ascii是两个字）
		if (message.size() > 56)
			message = message.substr(0, 56);

		long long message_ascii[8] = { 0 };
		int message_size = message.size();
		message_ascii[7] = message_size;
		int num = message_size / 8;
		int size_current = num * 8;
		//将前几个填满
		for (int i = 0; i < num; i++) {
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
		int W[68];
		int W_another[64];
		long long tmp = pow(2, 33) - 1;
		for (int i = 0; i < 8; i++) {
			W[2 * i] = message_ascii[i] >> 32;//高32位
			W[2 * i + 1] = message_ascii[i] & tmp; //低32位
		}


		int tmp1[8][8], tmp2[8][8];
		int count = 0;


		for (int i = 16; i < 64; i += 2) {
			if (i % 8 == 0) {
				__m256i a = _mm256_setr_epi32(W[i - 16], W[i - 15], W[i - 14], W[i - 13],
					W[i - 12], W[i - 11], W[i - 10], W[i - 9]);
				__m256i b = _mm256_setr_epi32(W[i - 9], W[i - 8], W[i - 7], W[i - 6],
					W[i - 5], W[i - 4], W[i - 3], W[i - 2]);
				__m256i c = _mm256_setr_epi32(W[i - 13], W[i - 12], W[i - 11], W[i - 10],
					W[i - 9], W[i - 8], W[i - 7], W[i - 6]);

				__m256i d = _mm256_xor_si256(a, b);
				__m256i e = _mm256_rl_epi32(c, 7);

				_mm256_storeu_epi32(tmp1[count], d);
				_mm256_storeu_epi32(tmp2[count], e);
				count++;
			}
			int index = i - i / 8 * 8;
			int count1 = count - 1;
			W[i] = (int)P1(tmp1[count1][index] ^ rol(W[i - 3], 15, 'l')) ^ tmp2[count1][index] ^ W[i - 6];
			W[i + 1] = (int)P1(tmp1[count1][index + 1] ^ rol(W[i - 2], 15, 'l')) ^ tmp2[count1][index + 1] ^ W[i - 5];
		}

		//tmp1 = W[i - 16] ^ W[i - 9]
		//tmp2 = rol(W[i - 13], 7, 'l')

		for (int i = 64; i < 68; i += 2) {
			W[i] = (int)P1(W[i - 16] ^ W[i - 9] ^ rol(W[i - 3], 15, 'l')) ^ rol(W[i - 13], 7, 'l') ^ W[i - 6];
			W[i + 1] = (int)P1(W[i - 15] ^ W[i - 8] ^ rol(W[i - 2], 15, 'l')) ^ rol(W[i - 12], 7, 'l') ^ W[i - 5];

		}



		int i;


		__m256i Wi[8], Wi_4[8], W_another_tmp[8];
		for (i = 0; i < 7; i += 2) {
			int a = 8 * i;
			Wi[i] = _mm256_setr_epi32(W[a], W[a + 1], W[a + 2], W[a + 3],
				W[a + 4], W[a + 5], W[a + 6], W[a + 7]);

			Wi_4[i] = _mm256_setr_epi32(W[a + 4], W[a + 5], W[a + 6], W[a + 7],
				W[a + 8], W[a + 9], W[a + 10], W[a + 11]);
			W_another_tmp[i] = _mm256_xor_si256(Wi[i], Wi_4[i]);
			_mm256_storeu_epi32(W_another + a, W_another_tmp[i]);

			int b = 8 * i + 8;
			Wi[i + 1] = _mm256_setr_epi32(W[b], W[b + 1], W[b + 2], W[b + 3],
				W[b + 4], W[b + 5], W[b + 6], W[b + 7]);

			Wi_4[i + 1] = _mm256_setr_epi32(W[b + 4], W[b + 5], W[b + 6], W[b + 7],
				W[b + 8], W[b + 9], W[b + 10], W[b + 11]);
			W_another_tmp[i + 1] = _mm256_xor_si256(Wi[i + 1], Wi_4[i + 1]);
			_mm256_storeu_epi32(W_another + b, W_another_tmp[i + 1]);
		}

		for (i; i < 8; i++) {
			int a = 8 * i;
			Wi[i] = _mm256_setr_epi32(W[a], W[a + 1], W[a + 2], W[a + 3],
				W[a + 4], W[a + 5], W[a + 6], W[a + 7]);

			Wi_4[i] = _mm256_setr_epi32(W[a + 4], W[a + 5], W[a + 6], W[a + 7],
				W[a + 8], W[a + 9], W[a + 10], W[a + 11]);
			W_another_tmp[i] = _mm256_xor_si256(Wi[i], Wi_4[i]);
			_mm256_storeu_epi32(W_another + a, W_another_tmp[i]);
		}

		//迭代压缩
		int A = IV[0]; int B = IV[1];
		int C = IV[2]; int D = IV[3];
		int E = IV[4]; int F = IV[5];
		int G = IV[6]; int H = IV[7];

		for (int i = 0; i < 64; i++) {
			int SS1 = rol((rol(A, 12, 'l') + E + rol(get_Tj(i), i % 32, 'l')) % m, 7, 'l');
			int SS2 = SS1 ^ rol(A, 12, 'l');
			int TT1 = (FF(A, B, C, i) + D + SS2 + W_another[i]) % m;
			int TT2 = (GG(E, F, G, i) + H + SS1 + W[i]) % m;
			D = C;
			C = rol(B, 9, 'l');
			B = A;
			A = TT1;
			H = G;
			G = rol(F, 19, 'l');
			F = E;
			E = P0(TT2);

		}

		__m256i iv_tmp = _mm256_setr_epi32(IV[0], IV[1], IV[2], IV[3], IV[4], IV[5], IV[6], IV[7]);
		__m256i letter_tmp = _mm256_setr_epi32(A, B, C, D, E, F, G, H);
		__m256i result_tmp = _mm256_xor_si256(letter_tmp, iv_tmp);
		_mm256_storeu_epi32(result, result_tmp);

	}
#endif

#if choice123==0
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
		for (int i = 0; i < num; i++) {
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
		int W[68];
		int W_another[64];
		long long tmp = pow(2, 33) - 1;
		for (int i = 0; i < 8; i++) {
			W[2 * i] = message_ascii[i] >> 32;//高32位
			W[2 * i + 1] = message_ascii[i] & tmp; //低32位
		}

		for (int i = 16; i < 68; i++) {
			W[i] = (int)P1(W[i - 16] ^ W[i - 9] ^ rol(W[i - 3], 15, 'l')) ^ rol(W[i - 13], 7, 'l') ^ W[i - 6];
		}

		for (int i = 0; i < 64; i++) {
			W_another[i] = (W[i] ^ W[i + 4]);
		}


		//迭代压缩
		int A = IV[0]; int B = IV[1];
		int C = IV[2]; int D = IV[3];
		int E = IV[4]; int F = IV[5];
		int G = IV[6]; int H = IV[7];

		for (int i = 0; i < 64; i++) {
			int SS1 = rol((rol(A, 12, 'l') + E + rol(get_Tj(i), i % 32, 'l')) % m, 7, 'l');
			int SS2 = SS1 ^ rol(A, 12, 'l');
			int TT1 = (FF(A, B, C, i) + D + SS2 + W_another[i]) % m;
			int TT2 = (GG(E, F, G, i) + H + SS1 + W[i]) % m;
			D = C;
			C = rol(B, 9, 'l');
			B = A;
			A = TT1;
			H = G;
			G = rol(F, 19, 'l');
			F = E;
			E = P0(TT2);

		}
		result[0] = (A ^ IV[0]); result[1] = (B ^ IV[1]);
		result[2] = (C ^ IV[2]); result[3] = (D ^ IV[3]);
		result[4] = (E ^ IV[4]); result[5] = (F ^ IV[5]);
		result[6] = (G ^ IV[6]); result[7] = (H ^ IV[7]);

	}
#endif

	string final() {
		stringstream ss;
		string tmp;
		string res = "";
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

int SM3_basic::rol(int param, int n, char choice) {
	int size = 32;
	n = n % size;
	long long tmp = pow(2, size) - 1;
	if (choice == 'l')
		return (param >> (size - n)) ^ ((param << n));
	else if (choice == 'r')
		return (param << (size - n)) ^ ((param >> n));
	else
		return NULL;
}

int SM3_basic::FF(int x, int y, int z, int j) {
	if (j <= 15) {
		return x ^ y ^ z;
	}
	else {
		return (x & y) | (x & z) | (y & z);
	}
}

int SM3_basic::GG(int x, int y, int z, int j) {
	if (j <= 15) {
		return x ^ y ^ z;
	}

	else {
		return (x & y) | (~x & z);

	}

}

int SM3_basic::P0(int x) {
	return x ^ rol(x, 9, 'l') ^ rol(x, 17, 'l');
}

int SM3_basic::P1(int x) {
	return x ^ rol(x, 15, 'l') ^ rol(x, 23, 'l');

}

int SM3_basic::get_Tj(int j) {
	if (j <= 15)
		return 0x79cc4519;
	else
		return 0x7a879d8a;
}
