#include"AES.h"
bitset<8> multiply(bitset<8>input_8,int switch_on)
{
	switch (switch_on)
	{
	case 0x01:
		return input_8;
	case 0x02:
		return multiply02(input_8);
	case 0x03:
		return multiply02(input_8) ^ input_8;
	case 0x09:
		return multiply02(multiply02(multiply02(input_8))) ^ input_8;
	case 0x0b:
		return multiply02(multiply02(multiply02(input_8))) ^ input_8^ multiply02(input_8);
	case 0x0d:
		return multiply02(multiply02(multiply02(input_8))) ^ input_8 ^ multiply02(multiply02(input_8));
	case 0x0e:
		return multiply02(multiply02(multiply02(input_8))) ^ multiply02(input_8)^ multiply02(multiply02(input_8));
	default:
		return 0;
	}
}
bitset<8> multiply(bitset<8> a, bitset<8> b) {
	bitset<8> res = 0;
	bitset<8> temp_a = a;
	for (int i = 0; i < 8; i++) {
		if (b[i] == 1)
		{ 
			res ^= temp_a;
		}
		temp_a = multiply02(temp_a);
	}
	return res;
}
bitset<8>inverse(bitset<8>input_8)
{
	for (int i = 0; i < 256; i++)
	{
		bitset<8>temp(i);
		if(multiply(input_8, temp) == 1)
			return temp;
	}
	return 0;
}
bitset<8> InvSubBytePure(bitset<8> input_8) 
{
	bitset<8> res = 0;
	for (int i = 0; i < 8; i++) 
	{
		for (int j = 0; j < 8; j++) 
		{
			if (INV_SBOX_AFFINE_MAT[i][j])
			{
				res[i] = res[i] ^ input_8[j];
			}
		}
	}
	res ^= INV_SBOX_AFFINE_VECTOR;
	return inverse(res);
}
void InvByteSub(StateMatrix& state) {
	int Nb = state[0].size();
	for (int i = 0; i < Nb; i++) 
	{
		for (int j = 0; j < 4; j++)
		{
			state[j][i] = InvSubBytePure(state[j][i]);
		}
	}
}
bitset<8> SubBytePure(bitset<8> input_8) {
	bitset<8> inv = inverse(input_8);
	bitset<8> res = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (SBOX_AFFINE_MAT[i][j]) {
				res[i] = res[i] ^ inv[j];
			}
		}
	}
	res ^= SBOX_AFFINE_VECTOR;
	return res;
}
void ByteSub(StateMatrix& state)
{
	int Nb = state[0].size();
	for (int i = 0; i < Nb; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			state[j][i] = SubBytePure(state[j][i]);
		}
	}
}
bitset<8> multiply02(bitset<8> input_8)
{
	bitset<8>res = input_8 << 1;
	if (input_8[7] == 1)
	{
		res ^= mx;
	}
	return res;
}
void ShiftRows(StateMatrix& state) {
	int Nb = state[0].size(); 
	if (Nb != 4 && Nb != 6 && Nb != 8) return;
	for (int i = 1; i < 4; i++) {
		std::vector<std::bitset<8>> tempRow(Nb);
		int offset = SHIFT_TABLE[Nb][i - 1]; 
		for (int j = 0; j < Nb; j++) {
			tempRow[j] = state[i][(j + offset) % Nb];
		}
		state[i] = tempRow;
	}
}
void InvShiftRows(StateMatrix& state) {
	int Nb = state[0].size();
	if (Nb != 4 && Nb != 6 && Nb != 8) return;
	for (int i = 1; i < 4; i++) {
		std::vector<std::bitset<8>> tempRow(Nb);
		int offset = SHIFT_TABLE[Nb][i - 1];
		for (int j = 0; j < Nb; j++) {
			tempRow[j] = state[i][(j - offset + Nb) % Nb];
		}
		state[i] = tempRow;
	}
}
void MixColumn(StateMatrix& state)
{
	int Nb = state[0].size();
	for (int j = 0; j < Nb; j++) 
	{ 
		vector<bitset<8>> nextCol(4);
		for (int i = 0; i < 4; i++)
		{ 
			bitset<8> b = 0;
			for (int k = 0; k < 4; k++)
			{
				b ^= multiply(state[k][j], MIX_COLUMNS_MAT[i][k]);
			}
			nextCol[i] = b;
		}
		for (int i = 0; i < 4; i++) {
			state[i][j] = nextCol[i];
		}
	}
}
void InvMixColumn(StateMatrix& state) 
{
	int Nb = state[0].size();
	for (int j = 0; j < Nb; j++) 
	{
		vector<bitset<8>> nextCol(4);
		for (int i = 0; i < 4; i++)
		{
			bitset<8> b = 0;
			for (int k = 0; k < 4; k++) 
			{
				b ^= multiply(state[k][j], INV_MIX_COLUMNS_MAT[i][k]);
			}
			nextCol[i] = b;
		}
		for (int i = 0; i < 4; i++)
		{
			state[i][j] = nextCol[i];
		}
	}
}
vector<bitset<8>> xorWords(const vector<bitset<8>>& a, const vector<bitset<8>>& b) 
{
	vector<bitset<8>> res(4);
	for (int i = 0; i < 4; i++) res[i] = a[i] ^ b[i];
	return res;
}
void KeyExpansion(const vector<bitset<8>>& Key, vector<Word>& W, int Nk, int Nb, int Nr)
{
	int totalWords = Nb * (Nr + 1);
	W.resize(totalWords);
	int i = 0;
	while (i < Nk) {
		W[i].b[0] = Key[4 * i];
		W[i].b[1] = Key[4 * i + 1];
		W[i].b[2] = Key[4 * i + 2];
		W[i].b[3] = Key[4 * i + 3];
		i++;
	}
	while (i < totalWords)
	{
		Word temp = W[i - 1];

		if (i % Nk == 0)
		{
			bitset<8> first = temp.b[0];
			temp.b[0] = temp.b[1];
			temp.b[1] = temp.b[2];
			temp.b[2] = temp.b[3];
			temp.b[3] = first;
			for (int j = 0; j < 4; j++) {
				temp.b[j] = SubBytePure(temp.b[j]);
			}
			temp.b[0] ^= bitset<8>(Rcon[i / Nk]);
		}
		else if (Nk > 6 && i % Nk == 4)
		{
			for (int j = 0; j < 4; j++) {
				temp.b[j] = SubBytePure(temp.b[j]);
			}
		}
		for (int j = 0; j < 4; j++) {
			W[i].b[j] = W[i - Nk].b[j] ^ temp.b[j];
		}
		i++;
	}
}
vector<bitset<8>> hexToBytes(string hex) {
	vector<bitset<8>> bytes;
	for (size_t i = 0; i < hex.length(); i += 2) {
		string byteString = hex.substr(i, 2);
		bytes.push_back(bitset<8>(stoi(byteString, nullptr, 16)));
	}
	return bytes;
}
void AddRoundKey(StateMatrix& state, const vector<Word>& W, int round) {
	int Nb = state[0].size();
	for (int j = 0; j < Nb; j++) {
		for (int i = 0; i < 4; i++) {
			state[i][j] ^= W[round * Nb + j][i];
		}
	}
}
void Encrypt(StateMatrix& state, const vector<Word>& W, int Nk, int Nb, int Nr) {
	AddRoundKey(state, W, 0);
	for (int round = 1; round < Nr; round++) {
		ByteSub(state);      
		ShiftRows(state);
		MixColumn(state);
		AddRoundKey(state, W, round);
	}
	ByteSub(state);
	ShiftRows(state);
	AddRoundKey(state, W, Nr);
}
void Decrypt(StateMatrix& state, const vector<Word>& W, int Nk, int Nb, int Nr) {
	AddRoundKey(state, W, Nr);
	for (int round = Nr - 1; round > 0; round--) {
		InvShiftRows(state); 
		InvByteSub(state); 
		AddRoundKey(state, W, round);
		InvMixColumn(state); 
	}
	InvShiftRows(state);
	InvByteSub(state);
	AddRoundKey(state, W, 0);
}
int main(int argc, char* argv[]) {
	if (argc < 6) {
		return 1;
	}
	string textHex = argv[1];
	string keyHex = argv[2];
	bool isEncrypt = (string(argv[3]) == "1");
	int Nk = stoi(argv[4]);
	int Nb = stoi(argv[5]);
	int Nr = (Nk > Nb ? Nk : Nb) + 6;
	vector<bitset<8>> rawText = hexToBytes(textHex);
	vector<bitset<8>> rawKey = hexToBytes(keyHex);
	StateMatrix state(4, vector<bitset<8>>(Nb));
	for (int j = 0; j < Nb; j++) {
		for (int i = 0; i < 4; i++) {
			state[i][j] = rawText[j * 4 + i];
		}
	}
	vector<Word> W;
	KeyExpansion(rawKey, W, Nk, Nb, Nr);
	if (isEncrypt) {
		Encrypt(state, W, Nk, Nb, Nr);
	}
	else {
		Decrypt(state, W, Nk, Nb, Nr); 
	}
	cout << hex << setfill('0');
	for (int j = 0; j < Nb; j++) {
		for (int i = 0; i < 4; i++) {
			cout << setw(2) << state[i][j].to_ulong();
		}
	}
	cout << endl;
	return 0;
}