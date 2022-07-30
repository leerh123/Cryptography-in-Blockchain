import random
w:int
class T:
    def __init__(self,p=None,d=None):
        self.p = p
        self.d = d

class EllipticCurve:
    def __init__(self,p=None,a=None,b=None,g=None,n=None,h=None):
        self.p = p
        self.a = a
        self.b = b
        self.g = g
        self.n = n
        self.h = h

    def on_curve(self,point):
        #无穷点
        if point is None:
            return True
        x, y = point
        return (y * y - x * x * x - self.a * x - self.b) % self.p == 0

    def negative(self,point):
        """-point."""
        assert self.on_curve(point)
        if point is None:
            return None
        x, y = point
        result = (x, -y % self.p)
        assert self.on_curve(result)
        return result

    def add(self,p, q):
        """p+q"""
        assert self.on_curve(p)
        assert self.on_curve(q)
        if p is None:
            return q
        if q is None:
            return p
        #p=-q
        if p == self.negative(q):
            return None
        x_p, y_p = p
        x_q, y_q = q
        if p == q:
            m = (3 * x_p * x_p + self.a) * self.inverse(2 * y_p,self.p)
        else:
            m = (y_p - y_q) * self.inverse(x_p - x_q, self.p)
        x = m * m - x_p - x_q
        y = y_p + m * (x - x_p)
        result = (x % self.p, -y % self.p)
        assert self.on_curve(result)
        return result

    def multi(self,k, point):
        """k * point"""
        assert self.on_curve(point)
        if k % self.n == 0 or point is None:
            return None
        if k < 0:
            # k * point = -k * (-point)
            return self.multi(-k, self.negative(point))
        result = None
        while k:
            if k & 1:
                result = self.add(result, point)
            point = self.add(point, point)
            k >>= 1
        assert self.on_curve(result)
        return result

    def inverse(self,a, n):
        # 求a^-1mod n
        def gcd(a: int, b: int) -> list:
            """
            Returns [gcd(a, b), x, y] where ax + by = gcd(a, b)
            """
            s, old_s = 0, 1
            t, old_t = 1, 0
            r, old_r = b, a
            while r != 0:
                quotient = old_r // r
                old_r, r = r, old_r - quotient * r
                old_s, s = s, old_s - quotient * s
                old_t, t = t, old_t - quotient * t
            return [old_r, old_s, old_t]

        gcd_res, x, y = gcd(a, n)
        if x < 0:
            x += n
        return x

    def compute_y(self,x):
        tmp = (x * x * x + self.a * x + self.b) % self.p
        #二次剩余为y
        y1,y2 = self.quadratic_residue(tmp,self.p)
        return (x,y1),(x,y2)

    # 求二次剩余
    def quadratic_residue(self,n, p):
        if (p == 2):
            return 1
        if (Legendre(n, p) + 1 == p):  # 为二次非剩余
            return -1
        a = -1
        while (True):
            a = random.randint(0, p - 1)
            t = a * a - n
            global w
            w = mod(t, p)
            if (Legendre(w, p) + 1 == p):
                break
        tmp = T()
        tmp.p = a
        tmp.d = 1
        ans = power(tmp, (p + 1) >> 1, p)
        return ans.p, p - ans.p


def quick_mod(a, b, m):

    ans = 1
    a %= m
    while b:
        if b & 1:

            ans = ans * a % m
            b-=1

        b >>= 1
        a = a * a % m

    return ans

#二次域乘法
def multi_er(a, b, m):

    ans = T()
    ans.p = (a.p * b.p % m + a.d * b.d % m * w % m) % m
    ans.d = (a.p * b.d % m + a.d * b.p % m) % m
    return ans

#二次域上快速幂
def power(a, b, m):

    ans = T()
    ans.p = 1
    ans.d = 0
    while (b):

        if (b & 1):

            ans = multi_er(ans, a, m)
            b-=1

        b >>= 1
        a = multi_er(a, a, m)

    return ans

#求勒让德符号
def Legendre(a, p):

    return quick_mod(a, (p - 1) >> 1, p)


def mod(a, m):
    a %= m
    if (a < 0):
        a += m
    return a


Secp256k1 = EllipticCurve(
    p=0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f,
    a=0,
    b=7,
    g=(0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798, 0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8),
    n=0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141,
    h=1,
)





