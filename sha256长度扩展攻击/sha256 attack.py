from Crypto.Hash import SHA256

test1=SHA256.new(str('hello').encode())
test1.update(str('world').encode())


test2=SHA256.new((str('hello')+str('world')).encode())
print(test1.hexdigest())
print(test2.hexdigest())


