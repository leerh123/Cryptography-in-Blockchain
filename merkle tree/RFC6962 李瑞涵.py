from Crypto.Hash import SHA256
import time
import math

class MerkleTree:
    def __init__(self,leaf_ls):
        '''
        function: 初始化函数。按leaf_ls生成merkle树
        :param leaf_ls: int类型-叶子取值列表
        '''
        self.leaf_ls = leaf_ls              #元素取值列表
        self.leaf_ls.sort()                 #对列表排序
        self.leaf_num = len(leaf_ls)        #叶节点的数量
        self.root = self.MTH(self.leaf_ls)  #根节点取值

    def append(self,n):
        '''
        function:增加一个元素,且元素的值为leaf_num+1
        :param n:n为int类型
        '''
        self.leaf_ls.append(n)
        self.leaf_ls.sort()
        self.leaf_num = len(leaf_ls)
        self.root = self.MTH(self.leaf_ls)

    def inclusion_proof(self,n):
        '''
        funtion: 对n进行存在性证明
        :param n: int型，n为元素的值
        :return: bool值，true代表n存在于merkle树中，false代表算法失败
        '''
        try:
            index = self.leaf_ls.index(n)
        except:
            return False
        audit_path = self.Path(index,self.leaf_ls)
        hash_leaf = int(SHA256.new((str(0x0|n)).encode()).hexdigest(),base=16)
        result = hash_leaf
        if audit_path!= None:
            for i in audit_path:
                result = int(SHA256.new((str(0x1|result|i)).encode()).hexdigest(),base=16)


        return (result==self.root)

    def exclusion_proof(self,n):
        '''
        funtion:对n进行不存在证明
        :param n: float类型，n为元素的值
        :return: bool值，true代表n不存在于merkle树中，false代表算法失败
        '''
        #找到两个相邻元素
        try:
            below = [i for i in self.leaf_ls if i<n]
            above = [j for j in self.leaf_ls if j>n]
            b = below[-1]
            a = above[0]
        except:
            return not self.inclusion_proof(n)


        #对这两个相邻元素进行存在性证明
        if(self.inclusion_proof(b)==True and self.inclusion_proof(a)==True):
        #证明这两个相邻元素相邻
            if(abs(self.leaf_ls.index(b)-self.leaf_ls.index(a))==1):
                return True

        return False


    def MTH(self,leaf_ls):
        n = len(leaf_ls)
        if n==0:
            return
        elif n==1:
            return int(SHA256.new((str(0x0|leaf_ls[0])).encode()).hexdigest(),base=16)
        else:
            k = math.ceil(n/2)
            return int(SHA256.new((str(0x1|self.MTH(leaf_ls[0:k])|self.MTH(leaf_ls[k:n]))).encode()).hexdigest(),
                       base=16)

    def Path(self,m,leaf_ls):
        '''
        function: 对leaf_ls[m]节点求审计路径
        :param m: int型
        :leaf_ls: 列表型
        :return: list型，返回leaf_ls[m]的审计路径(有序)
        '''
        if m>=self.leaf_num:
            return
        n = len(leaf_ls)
        if n==0 or n==1:
            return
        else:
            k = math.ceil(n/2)
            if m<k:
                result = []
                tmp = self.Path(m, leaf_ls[0:k])
                if tmp!= None and len(tmp)!=0:
                    for i in tmp:
                        result.append(i)
                result.append(self.MTH(leaf_ls[k:n]))
                return result
            else:
                result = []
                tmp = self.Path(m-k, leaf_ls[k:n])
                if tmp != None and len(tmp) != 0:
                    for i in tmp:
                        result.append(i)
                result.append(self.MTH(leaf_ls[0:k]))
                return result





# def leaf(n):
#     return int(SHA256.new((str(0x0|n)).encode()).hexdigest(),base=16)
# def not_leaf(a,b):
#     return int(SHA256.new((str(0x1 | a | b)).encode()).hexdigest(), base=16)

if __name__ =="__main__":
    start1 = time.perf_counter()

    size = 100000
    leaf_ls = []
    for i in range(size):
        leaf_ls.append(i)
    test = MerkleTree(leaf_ls)
    print("merkle树构建完成")
    end1 = time.perf_counter()
    print(f"merkle构建用时{end1 - start1}秒")

    print("-----存在性证明-----")
    example1 = int(input("请输入一个整数："))
    start2 = time.perf_counter()
    if(test.inclusion_proof(example1)==True):
        print("这个数存在")
    else:
        print("这个数不存在")
    end2 = time.perf_counter()
    print(f"存在性证明用时{end2 - start2}秒")

    print("-----不存在证明-----")
    example2 = float(input("请输入一个数(整数小数均可)："))
    start3 = time.perf_counter()
    if(test.exclusion_proof(example2)==True):
        print("这个数不存在")
    else:
        print("这个数存在")
    end3 = time.perf_counter()
    print(f"不存在性证明用时{end3-start3}秒")