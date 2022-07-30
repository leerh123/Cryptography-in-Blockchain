import random


print('请输入你的id')
id1 = int(input())
print('请输入你的年级')
grade1 =int(input())
print('请输入你的年份')
year1 = int(input())

'''id，grade，year为本人所拥有'''
id = 402
grade = 2
year = 2022

'''sig是通过考试的机构的签名值，r是验证机构选取的随机数'''
sig_by_moe = 30222
r = random.randint(0, 100)

'''a、b分别为p、h*t的哈希值'''
a = 1
b = 1

'''这个是本人自己使用，计算好后传给验证机构'''
def t(id1,grade1,year1,x):
    return (x-id1)*(x-grade1)*(x-year1)


'''这个是验证机构所知道的'''
def p(id,grade,year,sig_by_moe,x):
    return (x - id) * (x - grade) * (x - year)*(x-sig_by_moe)

'''验证机构收到t之后,计算h'''
def h(sig_by_moe,x):
    return (x-sig_by_moe)


'''验证函数,判断p是否等于t*h'''
def verify(id1,grade1,year1,id,grade,year,sig_by_moe,r):
    a = hash(p(id,grade,year,sig_by_moe,r))
    b = hash(t(id1,grade1,year1,r)*h(sig_by_moe,r))
    if(a==b):
        print('确实通过了考试')
        return True
    else :
        print('没有通过考试')
        return False


verify(id1,grade1,year1,id,grade,year,sig_by_moe,r)
