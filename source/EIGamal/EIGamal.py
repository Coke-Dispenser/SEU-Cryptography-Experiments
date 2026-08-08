import sys

def is_prime(n):
    if n < 2: return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0: return False
    return True

def get_prime_factors(n):
    factors = set()
    d, temp = 2, n
    while d * d <= temp:
        if temp % d == 0:
            factors.add(d)
            while temp % d == 0: temp //= d
        d += 1
    if temp > 1: factors.add(temp)
    return factors

def find_generator(p):
    if p == 2: return 1
    phi = p - 1
    factors = get_prime_factors(phi)
    for g in range(2, p):
        if all(pow(g, phi // q, p) != 1 for q in factors):
            return g
    return None

def mod_inverse(a, m):
    def egcd(a, b):
        if a == 0: return b, 0, 1
        d, x1, y1 = egcd(b % a, a)
        return d, y1 - (b // a) * x1, x1
    d, x, y = egcd(a, m)
    if d != 1: return None
    return x % m

def main():
    args = sys.argv[1:]
    if not args: return
    mode = args[0]

    try:
        if mode == "gen":
            p, a = map(int, args[1:3])
            if not is_prime(p): raise ValueError("p must be prime")
            alpha = find_generator(p)
            print(f"{p},{alpha},{pow(alpha, a, p)}")

        elif mode == "enc":
            m, p, alpha, h, k = map(int, args[1:6])
            if not (1 <= k < p - 1): raise ValueError("k must be in [1, p-2]")
            print(f"{pow(alpha, k, p)},{(m * pow(h, k, p)) % p}")

        elif mode == "dec":
            p, gamma, delta, a = map(int, args[1:5])
            s = pow(gamma, a, p)
            inv = mod_inverse(s, p)
            if inv is None: raise ValueError("Decryption failed")
            print((delta * inv) % p)
            
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()