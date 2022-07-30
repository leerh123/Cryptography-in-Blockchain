from gmssl import sm3
from gmssl import func
import time
a=b"11111"
maxtime=100000

#data是初始数据，length是碰撞的长度，代表十六进制数
def rho_attack(data,length):
    hash_value=sm3.sm3_hash(func.bytes_to_list(data))
    hash_list=[]
    value=hash_value[:length]
    hash_list.append(value)
    for i in range(maxtime):
        hash_value=sm3.sm3_hash(func.bytes_to_list(bytes(hash_value,encoding='utf_8')))
        value=hash_value[:length]
        if value in hash_list:
            print("rho攻击成功")
            print(i)
            print(value)
            return
        else:
            hash_list.append(value)
    print("rho攻击失败")


start=time.time()
rho_attack(a,9)
end=time.time()
print(end-start)
