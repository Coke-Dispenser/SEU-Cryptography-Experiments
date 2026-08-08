#pragma once
#include <iostream>
#include <bitset>
#include<iomanip>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;
struct Word {
    bitset<8> b[4];
    bitset<8>& operator[](int index) { return b[index]; }
    const bitset<8>& operator[](int index) const { return b[index]; }
};
using StateMatrix = vector<vector<bitset<8>>>;
static const int SBOX_AFFINE_MAT[8][8] = {
{1, 0, 0, 0, 1, 1, 1, 1},
{1, 1, 0, 0, 0, 1, 1, 1},
{1, 1, 1, 0, 0, 0, 1, 1},
{1, 1, 1, 1, 0, 0, 0, 1},
{1, 1, 1, 1, 1, 0, 0, 0},
{0, 1, 1, 1, 1, 1, 0, 0},
{0, 0, 1, 1, 1, 1, 1, 0},
{0, 0, 0, 1, 1, 1, 1, 1}
};
static const int INV_SBOX_AFFINE_MAT[8][8] = {
{0, 0, 1, 0, 0, 1, 0, 1},
{1, 0, 0, 1, 0, 0, 1, 0},
{0, 1, 0, 0, 1, 0, 0, 1},
{1, 0, 1, 0, 0, 1, 0, 0},
{0, 1, 0, 1, 0, 0, 1, 0},
{0, 0, 1, 0, 1, 0, 0, 1},
{1, 0, 0, 1, 0, 1, 0, 0},
{0, 1, 0, 0, 1, 0, 1, 0}
};
static const bitset<8> SBOX_AFFINE_VECTOR = 0x63;
static const bitset<8> INV_SBOX_AFFINE_VECTOR = 0x05;
static const int MIX_COLUMNS_MAT[4][4] = {
{2, 3, 1, 1},
{1, 2, 3, 1},
{1, 1, 2, 3},
{3, 1, 1, 2}
};
static const int INV_MIX_COLUMNS_MAT[4][4] = {
{0x0e, 0x0b, 0x0d, 0x09},
{0x09, 0x0e, 0x0b, 0x0d},
{0x0d, 0x09, 0x0e, 0x0b},
{0x0b, 0x0d, 0x09, 0x0e}
};
static const int SHIFT_TABLE[9][3] = {
{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
{1, 2, 3},
{0, 0, 0},
{1, 2, 3},
{0, 0, 0},
{1, 3, 4}
};
static const int Rcon[11] = {
0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};
static const bitset<8> mx = 0x1B;
bitset<8> multiply(bitset<8> input_8, int switch_on);
bitset<8> multiply(bitset<8> a, bitset<8> b);
bitset<8> inverse(bitset<8> input_8);
bitset<8> SubBytePure(bitset<8> input_8);
bitset<8> InvSubBytePure(bitset<8> input_8);
bitset<8> multiply02(bitset<8> input_8);
void ByteSub(StateMatrix& state);
void InvByteSub(StateMatrix& state);
void ShiftRows(StateMatrix& state);
void InvShiftRows(StateMatrix& state);
void MixColumn(StateMatrix& state);
void InvMixColumn(StateMatrix& state);
void AddRoundKey(StateMatrix& state, const vector<Word>& W, int round);
vector<bitset<8>> xorWords(const vector<bitset<8>>& a, const vector<bitset<8>>& b);
void KeyExpansion(const vector<bitset<8>>& Key, vector<vector<bitset<8>>>& W, int Nk, int Nb, int Nr);
vector<bitset<8>> hexToBytes(string hex);
void Encrypt(StateMatrix& state, const vector<Word>& W, int Nk, int Nb, int Nr);
void Decrypt(StateMatrix& state, const vector<Word>& W, int Nk, int Nb, int Nr);