#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <random>
#include <ctime>
#include <stdexcept>

using namespace std;

class BigInt {
public:
    vector<int> digits;
    bool neg = false;
    static const int BASE = 1e9;

    BigInt(long long v = 0) {
        if (v < 0) { neg = true; v = -v; }
        if (v == 0) digits.push_back(0);
        while (v > 0) {
            digits.push_back(v % BASE);
            v /= BASE;
        }
    }

    BigInt(string s) {
        if (s.empty()) { digits.push_back(0); return; }
        if (s[0] == '-') { neg = true; s = s.substr(1); }
        for (int i = s.size(); i > 0; i -= 9) {
            if (i < 9) digits.push_back(stoi(s.substr(0, i)));
            else digits.push_back(stoi(s.substr(i - 9, 9)));
        }
        trim();
    }

    void trim() {
        while (digits.size() > 1 && digits.back() == 0) digits.pop_back();
        if (digits.size() == 1 && digits[0] == 0) neg = false;
    }

    bool operator<(const BigInt& other) const {
        if (neg != other.neg) return neg;
        if (digits.size() != other.digits.size())
            return neg ? digits.size() > other.digits.size() : digits.size() < other.digits.size();
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i])
                return neg ? digits[i] > other.digits[i] : digits[i] < other.digits[i];
        }
        return false;
    }

    bool operator==(const BigInt& other) const {
        return neg == other.neg && digits == other.digits;
    }

    bool abs_less(const BigInt& other) const {
        if (digits.size() != other.digits.size()) return digits.size() < other.digits.size();
        for (int i = digits.size() - 1; i >= 0; i--)
            if (digits[i] != other.digits[i]) return digits[i] < other.digits[i];
        return false;
    }

    BigInt operator+(const BigInt& other) const {
        if (neg == other.neg) {
            BigInt res = *this;
            int carry = 0;
            for (size_t i = 0; i < max(res.digits.size(), other.digits.size()) || carry; ++i) {
                if (i == res.digits.size()) res.digits.push_back(0);
                long long cur = carry + res.digits[i] + (i < other.digits.size() ? other.digits[i] : 0);
                res.digits[i] = cur % BASE;
                carry = cur >= BASE;
            }
            return res;
        }
        return *this - (other.neg ? -other : other);
    }

    BigInt operator-() const {
        BigInt res = *this;
        if (!(res == BigInt(0))) res.neg = !neg;
        return res;
    }

    BigInt operator-(const BigInt& other) const {
        if (neg != other.neg) return *this + (-other);
        if (abs_less(other)) return -(other - *this);
        BigInt res = *this;
        int carry = 0;
        for (size_t i = 0; i < other.digits.size() || carry; ++i) {
            long long cur = res.digits[i] - carry - (i < other.digits.size() ? other.digits[i] : 0);
            carry = cur < 0;
            if (carry) cur += BASE;
            res.digits[i] = cur;
        }
        res.trim();
        return res;
    }

    BigInt operator*(const BigInt& other) const {
        BigInt res;
        res.neg = neg != other.neg;
        res.digits.resize(digits.size() + other.digits.size(), 0);
        for (size_t i = 0; i < digits.size(); ++i) {
            int carry = 0;
            for (size_t j = 0; j < other.digits.size() || carry; ++j) {
                long long cur = res.digits[i + j] + digits[i] * 1LL * (j < other.digits.size() ? other.digits[j] : 0) + carry;
                res.digits[i + j] = cur % BASE;
                carry = cur / BASE;
            }
        }
        res.trim();
        return res;
    }

    pair<BigInt, BigInt> divmod(const BigInt& b1, const BigInt& b2) const {
        if (b2 == BigInt(0)) throw std::invalid_argument("Division by zero");
        BigInt a = b1; a.neg = false;
        BigInt b = b2; b.neg = false;
        if (a < b) return { BigInt(0), b1 };
        BigInt q, r;
        for (int i = a.digits.size() - 1; i >= 0; i--) {
            r = r * BASE + a.digits[i];
            int l = 0, h = BASE - 1, m, qt = 0;
            while (l <= h) {
                m = l + (h - l) / 2;
                if (!(r < BigInt(m) * b)) { qt = m; l = m + 1; }
                else h = m - 1;
            }
            q.digits.push_back(qt);
            r = r - BigInt(qt) * b;
        }
        reverse(q.digits.begin(), q.digits.end());
        q.neg = b1.neg != b2.neg;
        r.neg = b1.neg;
        q.trim(); r.trim();
        return { q, r };
    }

    BigInt operator/(const BigInt& other) const { return divmod(*this, other).first; }
    BigInt operator%(const BigInt& other) const { return divmod(*this, other).second; }

    string to_string() const {
        if (digits.empty()) return "0";
        string s = (neg ? "-" : "") + std::to_string(digits.back());
        for (int i = (int)digits.size() - 2; i >= 0; i--) {
            string temp = std::to_string(digits[i]);
            s += string(9 - temp.size(), '0') + temp;
        }
        return s;
    }
};

BigInt gcd(BigInt a, BigInt b) {
    a.neg = false;
    b.neg = false;
    while (!(b == BigInt(0))) {
        a = a % b;
        swap(a, b);
    }
    return a;
}

BigInt modPow(BigInt base, BigInt exp, BigInt mod) {
    BigInt res(1);
    base = base % mod;
    if (base < BigInt(0)) base = base + mod;
    while (!(exp == BigInt(0))) {
        if (exp.digits[0] % 2 == 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp = exp / BigInt(2);
    }
    return res;
}

BigInt modInverse(BigInt a, BigInt m) {
    BigInt m0 = m, t, q;
    BigInt x0(0), x1(1);
    if (m == BigInt(1)) return BigInt(0);
    while (BigInt(1) < a) {
        if (m == BigInt(0)) return BigInt(0);
        q = a / m;
        t = m;
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    if (x1 < BigInt(0)) x1 = x1 + m0;
    return x1;
}

void manualGen(string ps, string qs) {
    BigInt p(ps), q(qs);
    BigInt n = p * q;
    BigInt phi = (p - BigInt(1)) * (q - BigInt(1));
    BigInt e("65537");
    if (!(gcd(e, phi) == BigInt(1))) e = BigInt("3");
    BigInt d = modInverse(e, phi);

    cout << "N:" << n.to_string() << endl;
    cout << "E:" << e.to_string() << endl;
    cout << "D:" << d.to_string() << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) return 1;
    string cmd = argv[1];
    if (cmd == "manual") {
        manualGen(argv[2], argv[3]);
    }
    else {
        BigInt m(argv[1]), key(argv[2]), n(argv[3]);
        cout << modPow(m, key, n).to_string() << endl;
    }
    return 0;
}