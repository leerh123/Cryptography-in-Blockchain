import pandas as pd
if __name__ =="__main__":
    #打开txt文件
    sample = pd.read_csv("C:\\Users\\86139\\Documents\\test123.txt",sep=":",names = ["原像","哈希值"])
    #对于原像相等的进行去重
    data1 = sample.drop_duplicates(subset=['原像'], keep=False)
    print(data1.shape)
    #找到哈希值相等的
    duplicate_bool = data1.duplicated(subset=['哈希值'], keep='first')
    repeat = data1.loc[duplicate_bool == True]
    #哈希值相等的构造一个列表
    hash_ls = repeat.哈希值.values
    #在大表格中对这些相等的哈希值进行查找并打印
    for i in hash_ls:
        print(data1[data1["哈希值"] == i])

