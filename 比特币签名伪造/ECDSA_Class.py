from EllipticCurve_Class import *
import ecdsa
from Crypto.Hash import SHA256
import random
import math

class ECDSA:
    def __init__(self,curve):
        self.curve = curve
        #自动生成私钥
        sk = ecdsa.SigningKey.generate(curve=ecdsa.curves.SECP256k1)
        self.private_key = int(sk.to_string().hex(),base=16)
        #生成公钥
        self.public_key = self.curve.multi(self.private_key,self.curve.g)

    #签名
    def sig(self,message):
        e = int(SHA256.new(message.encode()).hexdigest(),base=16)
        k = random.randint(0,self.curve.n)
        R = self.curve.multi(k,self.curve.g)
        r = R[0]
        assert r!=0
        k_1 = self.curve.inverse(k,self.curve.n)
        s = (k_1*(e+self.private_key*r)) % self.curve.n
        return r,s


    #验证签名
    def verf(self,message_hash,r,s,choose_public_key=False,public_key=None):
        if choose_public_key ==True:
            self.public_key = public_key
        e = message_hash
        w = self.curve.inverse(s,self.curve.n)
        (x,y) = self.curve.add(self.curve.multi(e*w,self.curve.g),self.curve.multi(r*w,self.public_key))
        return x==r

def sig_forge(public_key):
    '''伪造签名过程'''
    u = 5
    v = 7
    R = Secp256k1.add(Secp256k1.multi(u,Secp256k1.g),Secp256k1.multi(v,public_key))
    r = R[0]
    e = (r*u*Secp256k1.inverse(v,Secp256k1.n))% Secp256k1.n
    s = (r*Secp256k1.inverse(v,Secp256k1.n)) % Secp256k1.n
    return e,r,s


def compute_public_key(curve:EllipticCurve,message_hash,r,s):
    '''基于签名恢复出公钥'''
    G = curve.g
    e = message_hash
    R1,R2 = curve.compute_y(r)
    assert curve.on_curve(R1)
    assert curve.on_curve(R2)
    r_1 = curve.inverse(r,curve.n)
    eG = curve.multi(-e,G)
    Rs1 = curve.multi(s,R1)
    Rs2 = curve.multi(s,R2)
    public_key1 = curve.multi(r_1,curve.add(Rs1,eG))
    public_key2 = curve.multi(r_1,curve.add(Rs2,eG))
    return public_key1,public_key2



if __name__=="__main__":
    test = ECDSA(Secp256k1)

    message_hash = "a896f798b5e7cfd2d92dbc624a4d5c41e491e4eb47e52fd441e962151a613698"
    r = "8914c774e38a2a914b99928b46d9f5096bd1cdc792edcae95346b08b1d4d03ba"
    s = "48252bd6e4342301d4b4562ab224fa95e2b406e2665c81dcf89953884b13872"
    pk1,pk2 = compute_public_key(Secp256k1,int(message_hash,base=16),int(r,base=16),int(s,base=16))
    prefix = "0x04"
    print(f"计算出的公钥1为(十进制):{prefix+str(hex(pk1[0]))[2:]+str(hex(pk1[1]))[2:]}")
    print(f"计算出的公钥2为(十进制):{prefix+str(hex(pk2[0]))[2:]+str(hex(pk2[1]))[2:]}")
    print("----使用公钥1----")
    e, r, s = sig_forge(pk1)
    print("伪造的签名为：")
    print(f"消息哈希：{hex(e)}")
    print(f"签名r:{hex(r)}")
    print(f"签名s:{hex(s)}")
    print(f"验证结果为{test.verf(e, r, s, choose_public_key=True, public_key=pk1)}")

    print("----使用公钥2----")
    e, r, s = sig_forge(pk2)
    print("伪造的签名为：")
    print(f"消息哈希：{hex(e)}")
    print(f"签名r:{hex(r)}")
    print(f"签名s:{hex(s)}")
    print(f"验证结果为{test.verf(e, r, s, choose_public_key=True, public_key=pk2)}")
