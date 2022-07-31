# -*- coding=utf-8 -*-
# @Time:2022/7/30 17:24
# @Auther:程泽锋
# @File :SM2_2P_sig.py
# @software:PyCharm

'''
在GMSSL库的基础上，对CryptSM2类进行改进，从而实现了SM2 2P签名

项目代码说明:通过函数名与print()函数打印内容可知晓

运行指导:
1.密钥均为代码自行生成
2.可以通过修改M，Z参数对不同的ID（Z），消息（M）进行修改
3.其他地方无需修改，运行代码即可测试
'''


from gmssl import sm3, func

default_ecc_table = {
    'n': 'FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123',
    'p': 'FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF',
    'g': '32c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7'\
         'bc3736a2f4f6779c59bdcee36b692153d0a9877cc62a474002df32e52139f0a0',
    'a': 'FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC',
    'b': '28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93',
    'g_re':'32c4ae2c1f1981195f9904466a39c9948fe30bbff2660be1715a4589334c74c7'\
           '43c8c95c0b098863a642311c9496deac2f56788239d5b8c0fd20cd1adec60f5f'
}
class CryptSM2(object):

    def __init__(self, private_key, public_key, ecc_table=default_ecc_table):
        self.private_key = private_key
        self.public_key = public_key
        self.para_len = len(ecc_table['n'])
        self.ecc_a3 = (
            int(ecc_table['a'], base=16) + 3) % int(ecc_table['p'], base=16)
        self.ecc_table = ecc_table

    def _kg(self, k, Point):  # kP运算
        Point = '%s%s' % (Point, '1')
        mask_str = '8'
        for i in range(self.para_len - 1):
            mask_str += '0'
        mask = int(mask_str, 16)
        Temp = Point
        flag = False
        for n in range(self.para_len * 4):
            if (flag):
                Temp = self._double_point(Temp)
            if (k & mask) != 0:
                if (flag):
                    Temp = self._add_point(Temp, Point)
                else:
                    flag = True
                    Temp = Point
            k = k << 1
        return self._convert_jacb_to_nor(Temp)

    def _double_point(self, Point):  # 倍点
        l = len(Point)
        len_2 = 2 * self.para_len
        if l< self.para_len * 2:
            return None
        else:
            x1 = int(Point[0:self.para_len], 16)
            y1 = int(Point[self.para_len:len_2], 16)
            if l == len_2:
                z1 = 1
            else:
                z1 = int(Point[len_2:], 16)

            T6 = (z1 * z1) % int(self.ecc_table['p'], base=16)
            T2 = (y1 * y1) % int(self.ecc_table['p'], base=16)
            T3 = (x1 + T6) % int(self.ecc_table['p'], base=16)
            T4 = (x1 - T6) % int(self.ecc_table['p'], base=16)
            T1 = (T3 * T4) % int(self.ecc_table['p'], base=16)
            T3 = (y1 * z1) % int(self.ecc_table['p'], base=16)
            T4 = (T2 * 8) % int(self.ecc_table['p'], base=16)
            T5 = (x1 * T4) % int(self.ecc_table['p'], base=16)
            T1 = (T1 * 3) % int(self.ecc_table['p'], base=16)
            T6 = (T6 * T6) % int(self.ecc_table['p'], base=16)
            T6 = (self.ecc_a3 * T6) % int(self.ecc_table['p'], base=16)
            T1 = (T1 + T6) % int(self.ecc_table['p'], base=16)
            z3 = (T3 + T3) % int(self.ecc_table['p'], base=16)
            T3 = (T1 * T1) % int(self.ecc_table['p'], base=16)
            T2 = (T2 * T4) % int(self.ecc_table['p'], base=16)
            x3 = (T3 - T5) % int(self.ecc_table['p'], base=16)

            if (T5 % 2) == 1:
                T4 = (T5 + ((T5 + int(self.ecc_table['p'], base=16)) >> 1) - T3) % int(self.ecc_table['p'], base=16)
            else:
                T4 = (T5 + (T5 >> 1) - T3) % int(self.ecc_table['p'], base=16)

            T1 = (T1 * T4) % int(self.ecc_table['p'], base=16)
            y3 = (T1 - T2) % int(self.ecc_table['p'], base=16)

            form = '%%0%dx' % self.para_len
            form = form * 3
            return form % (x3, y3, z3)

    def _add_point(self, P1, P2):  # 点加函数，P2点为仿射坐标即z=1，P1为Jacobian加重射影坐标
        len_2 = 2 * self.para_len
        l1 = len(P1)
        l2 = len(P2)
        if (l1 < len_2) or (l2 < len_2):
            return None
        else:
            X1 = int(P1[0:self.para_len], 16)
            Y1 = int(P1[self.para_len:len_2], 16)
            if (l1 == len_2):
                Z1 = 1
            else:
                Z1 = int(P1[len_2:], 16)
            x2 = int(P2[0:self.para_len], 16)
            y2 = int(P2[self.para_len:len_2], 16)

            T1 = (Z1 * Z1) % int(self.ecc_table['p'], base=16)
            T2 = (y2 * Z1) % int(self.ecc_table['p'], base=16)
            T3 = (x2 * T1) % int(self.ecc_table['p'], base=16)
            T1 = (T1 * T2) % int(self.ecc_table['p'], base=16)
            T2 = (T3 - X1) % int(self.ecc_table['p'], base=16)
            T3 = (T3 + X1) % int(self.ecc_table['p'], base=16)
            T4 = (T2 * T2) % int(self.ecc_table['p'], base=16)
            T1 = (T1 - Y1) % int(self.ecc_table['p'], base=16)
            Z3 = (Z1 * T2) % int(self.ecc_table['p'], base=16)
            T2 = (T2 * T4) % int(self.ecc_table['p'], base=16)
            T3 = (T3 * T4) % int(self.ecc_table['p'], base=16)
            T5 = (T1 * T1) % int(self.ecc_table['p'], base=16)
            T4 = (X1 * T4) % int(self.ecc_table['p'], base=16)
            X3 = (T5 - T3) % int(self.ecc_table['p'], base=16)
            T2 = (Y1 * T2) % int(self.ecc_table['p'], base=16)
            T3 = (T4 - X3) % int(self.ecc_table['p'], base=16)
            T1 = (T1 * T3) % int(self.ecc_table['p'], base=16)
            Y3 = (T1 - T2) % int(self.ecc_table['p'], base=16)

            form = '%%0%dx' % self.para_len
            form = form * 3
            return form % (X3, Y3, Z3)

    def _convert_jacb_to_nor(self, Point): # Jacobian加重射影坐标转换成仿射坐标
        len_2 = 2 * self.para_len
        x = int(Point[0:self.para_len], 16)
        y = int(Point[self.para_len:len_2], 16)
        z = int(Point[len_2:], 16)
        z_inv = pow(z, int(self.ecc_table['p'], base=16) - 2, int(self.ecc_table['p'], base=16))
        z_invSquar = (z_inv * z_inv) % int(self.ecc_table['p'], base=16)
        z_invQube = (z_invSquar * z_inv) % int(self.ecc_table['p'], base=16)
        x_new = (x * z_invSquar) % int(self.ecc_table['p'], base=16)
        y_new = (y * z_invQube) % int(self.ecc_table['p'], base=16)
        z_new = (z * z_inv) % int(self.ecc_table['p'], base=16)
        if z_new == 1:
            form = '%%0%dx' % self.para_len
            form = form * 2
            return form % (x_new, y_new)
        else:
            return None

    def verify(self, Sign, data):
        # 验签函数，sign签名r||s，E消息hash，public_key公钥
        r = int(Sign[0:self.para_len], 16)
        s = int(Sign[self.para_len:2*self.para_len], 16)
        print(len(data))
        e = int(data.hex(), 16)
        t = (r + s) % int(self.ecc_table['n'], base=16)
        if t == 0:
            return 0
        P1 = self._kg(s, self.ecc_table['g'])
        P2 = self._kg(t, self.public_key)
        # print(P1)
        # print(P2)
        if P1 == P2:
            P1 = '%s%s' % (P1, 1)
            P1 = self._double_point(P1)
        else:
            P1 = '%s%s' % (P1, 1)
            P1 = self._add_point(P1, P2)
            P1 = self._convert_jacb_to_nor(P1)
        x = int(P1[0:self.para_len], 16)
        return (r == ((e + x) % int(self.ecc_table['n'], base=16)))


    def sign(self, data, K):  # 签名函数, data消息的hash，private_key私钥，K随机数，均为16进制字符串
        E = data.hex() # 消息转化为16进制字符串
        e = int(E, 16)

        d = int(self.private_key, 16)
        k = int(K, 16)

        P1 = self._kg(k, self.ecc_table['g'])

        x = int(P1[0:self.para_len], 16)
        R = ((e + x) % int(self.ecc_table['n'], base=16))
        if R == 0 or R + k == int(self.ecc_table['n'], base=16):
            return None
        d_1 = pow(d+1, int(self.ecc_table['n'], base=16) - 2, int(self.ecc_table['n'], base=16))
        S = (d_1*(k + R) - R) % int(self.ecc_table['n'], base=16)
        if S == 0:
            return None
        else:
            return '%064x%064x' % (R,S)

###################后续代码为添加的代码###############################
    def inverse_mod_prime(self,a:int,primeMod:int):
        assert 0<a<primeMod
        return pow(a,primeMod-2,primeMod)

    def genrateP1(self):
        _n=int(self.ecc_table['n'], 16)
        self.d1=int(func.random_hex(self.para_len),16)
        d1_re=self.inverse_mod_prime(self.d1,_n)
        P1=self._kg(d1_re,self.ecc_table['g'])
        return P1

    def genrateP(self,P1):
        _n = int(self.ecc_table['n'], 16)
        g_re = self.ecc_table['g_re']
        self.d2 =int(func.random_hex(self.para_len), 16)
        d2_re = self.inverse_mod_prime(self.d2, _n)
        self.P=self._kg(d2_re,P1)
        self.P = '%s%s' % (self.P, 1)
        self.P=self._add_point(self.P, g_re)
        self.P = self._convert_jacb_to_nor(self.P)
        return self.P


    def get_e(self,Z,M):
        M_=Z+M
        e = sm3.sm3_hash(func.bytes_to_list(M_)).encode()
        return int(e.hex(), 16)


    def get_Q1(self):
        self.k1 = int(func.random_hex(self.para_len), 16)
        g = self.ecc_table['g']
        Q1=self._kg(self.k1,g)
        return Q1


    def get_r_s2_s3(self,Q1,e):
        n=int(self.ecc_table['n'], base=16)
        g = self.ecc_table['g']
        k2=int(func.random_hex(self.para_len),16)
        k3=int(func.random_hex(self.para_len),16)
        Q2=self._kg(k2,g)
        tmp=self._kg(k3,Q1)
        tmp = '%s%s' % (tmp, 1)
        tmp=self._add_point(tmp, Q2)
        tmp = self._convert_jacb_to_nor(tmp)
        x1 = int(tmp[0:self.para_len], 16)
        r=(x1+e)%n
        s2=(self.d2*k3)%n
        s3=(self.d2*((r+k2)%n))%n
        return r,s2,s3

    def sign_2P_last(self,r,s2,s3):
        n = int(self.ecc_table['n'],base=16)
        s=(((self.d1*self.k1)%n)*s2+(self.d1*s3)%n-r)%n
        if s==0 and s+r==n:
            return None
        return '%064x%064x' % (r, s)





print('--------------SM2 2P crypt------------------')
sm2_crypt_2P_A = CryptSM2(public_key=None, private_key=None)
sm2_crypt_2P_B = CryptSM2(public_key=None, private_key=None)


print('--------------A生成P1并发送--------------------')
P1=sm2_crypt_2P_A.genrateP1()
print('P1:')
print(P1)

print('\n--------------B接受P1后进行相关运算生产P---------------')
P=sm2_crypt_2P_B.genrateP(P1)
print('P:')
print(P)

####################           此处修改Z，M        ########################################################
print('\n--------------A生产Q1，e后发送给B----------------')
Z=b'A'
M=b'11111'
Q1=sm2_crypt_2P_A.get_Q1()
e=sm2_crypt_2P_A.get_e(Z,M)
print('Q1:')
print(Q1)
print('e:')
print(e)


print('\n--------------B接受Q1，e后进行后续运算生成r,s2,s3-------------')
r,s2,s3=sm2_crypt_2P_B.get_r_s2_s3(Q1,e)
print('r:')
print(r)
print('s2:')
print(s2)
print('s3')
print(s3)
print('\n--------------A接受了r,s2,s3后生成s----------------')
sig=sm2_crypt_2P_A.sign_2P_last(r,s2,s3)
print('SM2 2P签名:')
print(sig)



print('\n---------------通过GMSSL的验证函数对签名结果进行了验证---------------------------')
a=(sm2_crypt_2P_A.d1*sm2_crypt_2P_B.d2)%int(sm2_crypt_2P_A.ecc_table['n'], base=16)
private_key=str(hex(sm2_crypt_2P_A.inverse_mod_prime(a,int(sm2_crypt_2P_A.ecc_table['n'], base=16))-1))
sm2_crypt_2P_C = CryptSM2(public_key=P, private_key=private_key)
M_=Z+M
e = sm3.sm3_hash(func.bytes_to_list(M_)).encode()
verify = sm2_crypt_2P_C.verify(sig,e)
print('验证结果:')
print(verify)



