#include <iostream>
#include<math.h>
#include<chrono>
#include<thread>
using namespace std;
#define num 16384//2的14次方

const int thread_num = 128;
uint8_t sbox[256] = { 
	0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
	0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
	0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
	0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
	0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
	0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
	0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
	0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
	0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
	0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
	0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
	0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
	0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
	0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
	0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
	0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48

};

uint32_t ck[32] = {
					0x00070e15,0x1c232a31,0x383f464d,0x545b6269,
	0x70777e85,0x8c939aa1,0xa8afb6bd,0xc4cbd2d9,
	0xe0e7eef5,0xfc030a11,0x181f262d,0x343b4249,
	0x50575e65,0x6c737a81,0x888f969d,0xa4abb2b9,
	0xc0c7ced5,0xdce3eaf1,0xf8ff060d,0x141b2229,
	0x30373e45,0x4c535a61,0x686f767d,0x848b9299,
	0xa0a7aeb5,0xbcc3cad1,0xd8dfe6ed,0xf4fb0209,
	0x10171e25,0x2c333a41,0x484f565d,0x646b7279

};

uint32_t fk[4] = {
	0xa3b1bac6,
	0x56aa3350,
	0x677d9197,
	0xb27022dc
};

unsigned int input[num];
unsigned int output[num];
void ui_to_bytes(unsigned int i, uint8_t n[4])
{
	n[0] = ((i >> 24) & 0xFF);
	n[1] = ((i >> 16) & 0xFF);
	n[2] = ((i >> 8) & 0xFF);
	n[3] = (i & 0xFF);
}

void byte_to_bits(uint8_t data, uint8_t* n)
{
	for (int i = 0; i < 8; i++)
	{
		n[7-i] = (data & (0x01 << i)) == (0x01 << i) ? 1 : 0;
	}
}

unsigned int bits_to_ui(uint8_t* n)
{
	unsigned int tmp0=0,tmp1=0,tmp2=0,tmp3=0, tmp4 = 0, tmp5 = 0,tmp6 = 0,tmp7 = 0;
	for (int i = 0; i < 32; i=i+8)
	{
		tmp0 += *(n + i) * pow(2, 31 - i);
		tmp1 += *(n + i+1) * pow(2, 31 - i-1);
		tmp2 += *(n + i + 2) * pow(2, 31 - i - 2);
		tmp3 += *(n + i + 3) * pow(2, 31 - i - 3);
		tmp4 += *(n + i + 4) * pow(2, 31 - i - 4);
		tmp5 += *(n + i + 5) * pow(2, 31 - i - 5);
		tmp6 += *(n + i + 6) * pow(2, 31 - i - 6);
		tmp7 += *(n + i + 7) * pow(2, 31 - i - 7);
	}
	return tmp0+tmp1+tmp2+tmp3+tmp4+tmp5+tmp6+tmp7;
	//unsigned int tmp0 = 0;
	//for (int i = 0; i < 32; i ++)
	//{
	//	tmp0 += *(n + i) * pow(2, 31 - i);
	//
	//}
	//return tmp0 ;
}

void left_move(uint8_t* n, int length, uint8_t tmp[32])     //循环左移，n是一个字即四个字节
{
	
	for (int i = 0; i < 32 - length; i++)
	{
		tmp[i] =n[i + length];
	}
	for (int i = 0; i < length; i++)
	{
		tmp[32-length+i] = n[i];
	}
	
}

unsigned int get_key(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int ck )
{
	unsigned int rk;
	unsigned int tmp = b ^ c ^ d ^ ck;
	uint8_t mid0[32] = { 0 }, mid1[32] = {0};
	uint8_t bytes[4];
	ui_to_bytes(tmp, bytes);
	uint8_t _bytes[4];
	_bytes[0] = sbox[bytes[0]];
	_bytes[1] = sbox[bytes[1]];
	_bytes[2] = sbox[bytes[2]];
	_bytes[3] = sbox[bytes[3]];
	uint8_t n[32] = { 0 }, _n[8] = { 0 }, __n[8] = { 0 }, ___n[8]{ 0 }, ____n[8] = { 0 };
	byte_to_bits(_bytes[0], _n);
	byte_to_bits(_bytes[1], __n);
	byte_to_bits(_bytes[2], ___n);
	byte_to_bits(_bytes[3], ____n);
	for (int i = 0; i < 8; i++) {
		n[i] = _n[i];
		n[8 + i] = __n[i];
		n[16 + i] = ___n[i];
		n[24 + i] = ____n[i];
	}

	left_move(n, 13,mid0);
	left_move(n, 23,mid1);
	unsigned int lre = bits_to_ui(n) ^ bits_to_ui(mid0) ^ bits_to_ui(mid1);
	rk = a ^ lre;
	return rk;
}

unsigned int sm4_F(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int rk)
{
	unsigned int result;
	unsigned int tmp = b ^ c ^ d ^ rk;
	uint8_t mid0[32], mid1[32],mid2[32],mid3[32];
	uint8_t bytes[4];
	ui_to_bytes(tmp, bytes);
	uint8_t _bytes[4];
	_bytes[0] = sbox[bytes[0]];
	_bytes[1] = sbox[bytes[1]];
	_bytes[2] = sbox[bytes[2]];
	_bytes[3] = sbox[bytes[3]];
	uint8_t n[32] = { 0 }, _n[8] = { 0 }, __n[8] = { 0 }, ___n[8]{ 0 }, ____n[8] = {0};
	byte_to_bits(_bytes[0], _n);
	byte_to_bits(_bytes[1], __n);
	byte_to_bits(_bytes[2], ___n);
	byte_to_bits(_bytes[3], ____n);
	for (int i = 0; i < 8; i++) {
		n[i] = _n[i];
		n[8 + i] = __n[i];
		n[16 + i] = ___n[i];
		n[24 + i] = ____n[i];
	}

	left_move(n, 2 ,mid0);
	left_move(n, 10,mid1);
	left_move(n, 18,mid2);
	left_move(n, 24,mid3);
	unsigned int lre = bits_to_ui(n) ^ bits_to_ui(mid0) ^ bits_to_ui(mid1) ^ bits_to_ui(mid2) ^ bits_to_ui(mid3);
	result = a ^ lre;
	return result;
}

void sm4_enc(unsigned int *_input,unsigned int key[4],unsigned int *_output)
{
	unsigned int k[36],rk[32],x[36];
	 k[0] = key[0] ^ fk[0];
	 k[1]= key[1] ^ fk[1];
	 k[2]= key[2] ^ fk[2];
	 k[3]= key[3] ^ fk[3];
	 for (int i = 0; i < 32; i++)
	 {
		 k[i + 4] = get_key(k[i], k[i + 1], k[i + 2], k[i + 3], ck[i]);
		 rk[i] = k[i + 4];
	 }
	 x[0] = *(_input+0);
	 x[1] = *(_input+1);
	 x[2] = *(_input+2);
	 x[3] = *(_input+3);
	 for (int i = 0; i < 32; i++)
	 {
		 x[i + 4] = sm4_F(x[i], x[i + 1], x[i + 2], x[i + 3], rk[i]);
	 }
	 *(_output+0) = x[35];
	 *(_output+1) = x[34];
	 *(_output+2) = x[33];
	 *(_output+3) = x[32];
}

void sm4_enc_edc()
{
	unsigned int key[4] = { 0x01234567,0x89abcdef, 0xfedcba98, 0x76543210 };
	for (int i = 0; i < num; i = i + 16)
	{
		sm4_enc(input + i, key, output + i);
		sm4_enc(input + i+4, key, output + i+4);
		sm4_enc(input + i + 8, key, output + i + 8);
		sm4_enc(input + i + 12, key, output + i + 12);
	}
}
void sm4_enc_edc_thread(int thread_start,int thread_end)
{
	unsigned int key[4] = { 0x01234567,0x89abcdef, 0xfedcba98, 0x76543210 };
	for (int i = thread_start; i < thread_end; i = i + 4)
	{
		sm4_enc(input + i, key, output + i);
	}
}
int main()
{
	for (int i = 0; i < num; i = i + 4)
	{
		input[i] = 0x01234567;
		input[i + 1] = 0x89abcdef;
		input[i + 2] = 0xfedcba98;
		input[i + 3] = 0x76543210;
	}
	thread th[thread_num];
	int offset = num / thread_num;
	int thread_start, thread_end;
	auto start = std::chrono::high_resolution_clock::now();
	//普通
	sm4_enc_edc();
	//多线程
	//for (int i = 0; i < thread_num; i++)
	//{
	//	thread_start = i * offset;
	//	thread_end = i * offset + offset;
	//	th[i] = thread(sm4_enc_edc_thread,thread_start, thread_end);
	//}
	//for (int i = 0; i < thread_num; i++)
	//{
	//	th[i].join();
	//}
	auto end = std::chrono::high_resolution_clock::now();
	//printf("%x\n", *(output));
	//printf("%x\n", *(output + 1));
	//printf("%x\n", *(output + 2));
	//printf("%x\n", *(output + 3));
	std::chrono::duration<double, std::ratio<1, 1000>>re = end - start;
	cout << "cost time:" << re.count() << "ms" << endl;
}