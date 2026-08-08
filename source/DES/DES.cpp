#include"DES.h"
bitset<48> K[16];
template <size_t InSize, size_t OutSize>
bitset<OutSize> permute(const bitset<InSize>& input, const int* table)
{
	bitset<OutSize> output;
	for (size_t i = 0; i < OutSize; i++)
	{
		output[OutSize - 1 - i] = input[InSize - table[i]];
	}
	return output;
}
bitset<32> S_BOX_permute(const bitset<48>& input_48)
{
	bitset<32> output;
	for (int i = 0; i < 8; i++) {
		int start_bit = 47 - i * 6;
		int b1 = input_48[start_bit];
		int b2 = input_48[start_bit - 1];
		int b3 = input_48[start_bit - 2];
		int b4 = input_48[start_bit - 3];
		int b5 = input_48[start_bit - 4];
		int b6 = input_48[start_bit - 5];
		int row = (b1 << 1) | b6;
		int col = (b2 << 3) | (b3 << 2) | (b4 << 1) | b5;
		int val = S_BOX[i][row][col];
		for (int j = 0; j < 4; ++j) {
			output[31 - (i * 4 + (3 - j))] = (val >> j) & 1;
		}
	}
	return output;
}
bitset<56> shifts(const bitset<56>& input_56, int round)
{
	bitset<56> output;
	for (int i = 0; i < 28; i++)
	{
		output[i] = input_56[(i - SHIFTS[round - 1] + 28) % 28];
		output[i + 28] = input_56[(i - SHIFTS[round - 1] + 28) % 28 + 28];
	}
	return output;
}
void genKeys(bitset<64> input) {
	bitset<56> Ki = permute<64, 56>(input, PC1);
	for (int r = 1; r <= 16; ++r) {
		Ki = shifts(Ki, r);
		K[r - 1] = permute<56, 48>(Ki, PC2);
	}
}
bitset<32> f(bitset<32> R, bitset<48> key)
{
	bitset<48> Expand = permute<32, 48>(R, E);
	Expand ^= key;
	bitset<32> SBOX = S_BOX_permute(Expand);
	return permute<32, 32>(SBOX, P);
}
void separate(bitset<32>& L, bitset<32>& R, bitset<64>& ip)
{
	for (int i = 0; i < 32; i++)
	{
		R[i] = ip[i];
		L[i] = ip[i + 32];
	}
}
bitset<64> combine(bitset<32>& L, bitset<32>& R)
{
	bitset<64>temp;
	for (int i = 0; i < 32; i++)
	{
		temp[i] = L[i];
		temp[i + 32] = R[i];
	}
	return temp;
}
bitset<64> DES_main(bitset<64> text, bitset<64> rawkey, bool j)
{
	genKeys(rawkey);
	bitset<64> ip = permute<64, 64>(text, IP);
	bitset<32> L, R;
	separate(L, R, ip);
	for (int i = 0; i < 16; i++)
	{
		int keyIndex = j ? i : (15 - i);
		bitset<32> temp1 = R;
		bitset<32> temp2 = f(R, K[keyIndex]);
		R = temp2 ^ L;
		L = temp1;
	}
	bitset<64> c = permute<64, 64>(combine(L, R), IP_INV);
	return c;
}
int main(int argc, char* argv[]) {
	if (argc < 4) return 1;
	bitset<64> text(stoull(argv[1], nullptr, 16));
	bitset<64> key(stoull(argv[2], nullptr, 16));
	bool judge = (string(argv[3]) == "1");
	bitset<64> result = DES_main(text, key, judge);
	cout << hex << setfill('0') << setw(16) << result.to_ullong() << endl;
	return 0;
}