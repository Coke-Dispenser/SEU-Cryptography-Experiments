import sys

def gcd(a, b):
    while b:
        a, b = b, a % b
    return a

def extended_gcd(a, b):
    if a == 0:
        return b, 0, 1
    d, x1, y1 = extended_gcd(b % a, a)
    x = y1 - (b // a) * x1
    y = x1
    return d, x, y

def mod_inverse(e, phi):
    d, x, y = extended_gcd(e, phi)
    if d != 1:
        raise ValueError("e 与 phi 不互质")
    return x % phi

def main():
    # 接收参数: RSA.exe mode arg1 arg2 arg3 ...
    args = sys.argv[1:]
    mode = args[0]

    if mode == "gen":
        # 生成模式: RSA.exe gen p q
        p = int(args[1])
        q = int(args[2])
        n = p * q
        phi = (p - 1) * (q - 1)
        
        # 常用 e 值
        e = 65537
        if gcd(e, phi) != 1:
            e = 3
            while gcd(e, phi) != 1:
                e += 2
        
        d = mod_inverse(e, phi)
        # 输出格式: n,e,d
        print(f"{n},{e},{d}")

    elif mode == "calc":
        # 计算模式: RSA.exe calc data key n
        # RSA 公式: C = M^e mod n 或 M = C^d mod n
        data = int(args[1])
        key = int(args[2])
        n = int(args[3])
        
        result = pow(data, key, n)
        print(result)

if __name__ == "__main__":
    main()