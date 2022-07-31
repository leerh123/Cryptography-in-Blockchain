# -*- coding=utf-8 -*-
# @Time:2022/7/31 8:47
# @Auther:程泽锋
# @File :SM2_RFC6979.py
# @software:PyCharm

import hashlib
from gmssl import sm2,sm3, func
import hmac
import math

default_ecc_table = {
    'n': 'FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123',
    'p': 'FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF',
    'g': '32c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7'\
         'bc3736a2f4f6779c59bdcee36b692153d0a9877cc62a474002df32e52139f0a0',
    'a': 'FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC',
    'b': '28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93',
}

def _hmac(key: bytes, data: bytes, h=hashlib.sha256):#创建该函数以便写代码
    return hmac.new(key, data, h).digest()


def generate_k(x, m,n, h=hashlib.sha256):#RFC6979生成随机数k的函数
    h1= h(m).digest()
    h_len= len(h1)
    v = b'\x01' * math.ceil(h_len / 8)
    _k= b'\x00' * math.ceil(h_len / 8)
    _k = _hmac(_k, v + b'\x00' + x + h1, h)
    v = _hmac(_k, v, h)
    _k = _hmac(_k, v + b'\x01' +x + h1, h)
    v = _hmac(_k, v, h)
    while True:
        t = b''
        while len(t.hex())< len(n):#确保生成的数为64bytes
            v = _hmac(_k, v, h)
            t += v
        k=int(t.hex(),16)
        if 0 < k < int(n,16):#确保生成的数小于n
            return t.hex()
        _k = _hmac(_k, v + b'\x00', h)
        v = _hmac(_k, v, h)


print('----------------在RFC6979协议下进行签名与验证----------------------')

#######设定参数
#SM2-A参数
rk_A = '0d1d31b70ef5d8d04d1d58158b2b418321a5b3dc8c68cdfe821a5e8c42d7e201'
pk_A = '36cd1616a0fdf51a57c9ac9c492d1049f8dd2579625814e1ddc9bd8d8de0b251530eced795456dc46802a5b1e1cfb7897ba39045e4619fcee3a0200e2a450ed7'

#创建SM2对象
sm2_crypt_A = sm2.CryptSM2(public_key=pk_A, private_key=rk_A)

#设置要传递的消息
msg=b'111111111111111111111'

#生成基于RFC6979的随机数K
k=generate_k(rk_A.encode(),msg,default_ecc_table['n'])
print('k:')
print(k)

#进行签名
newData_sm3 = sm3.sm3_hash(func.bytes_to_list(msg)).encode()
sign_sm3=sm2_crypt_A.sign(newData_sm3, k)
print('签名:')
print(sign_sm3)

#进行验证
verify=sm2_crypt_A.verify(sign_sm3,newData_sm3)
print('检验结果为')
print(verify)
