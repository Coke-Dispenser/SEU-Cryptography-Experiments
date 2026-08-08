import sys
import hashlib

def main():
    if len(sys.argv) < 2:
        print("Usage: DH.exe [pub|secret] [args...]")
        return

    mode = sys.argv[1]

    try:
        if mode == 'pub':
            # 计算公钥: DH.exe pub [p] [g] [private_key]
            p = int(sys.argv[2])
            g = int(sys.argv[3])
            priv = int(sys.argv[4])
            pub = pow(g, priv, p)
            print(pub)

        elif mode == 'secret':
            # 计算共享密钥: DH.exe secret [p] [other_pub] [my_priv]
            p = int(sys.argv[2])
            other_pub = int(sys.argv[3])
            my_priv = int(sys.argv[4])
            
            # 1. 计算出原始的 DH 共享大整数秘密 S
            raw_secret = pow(other_pub, my_priv, p)
            
            # 2. 密钥派生 (KDF)：这里用 SHA-256 将大整数转换为 256-bit (64位十六进制) 字符串
            # 这样不仅解决了"任意长度/规定长度"的问题，还增强了安全性（抹平了代数结构）
            key_hex = hashlib.sha256(str(raw_secret).encode('utf-8')).hexdigest()
            
            # 返回：原始整数秘密, 派生出的16进制密钥
            print(f"{raw_secret},{key_hex}")

    except Exception as e:
        print(f"Error: {str(e)}")

if __name__ == '__main__':
    main()