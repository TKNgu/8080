#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

#define FILE_ROOM "data/Tetris.gb"

#define ROM_LOGO_START 0x0104
#define ROM_LOGO_END 0x0133

#define ROM_TITLE_START 0x0134
#define ROM_TITLE_END 0x0143

#define ROM_MMCODE_START 0x013F
#define ROM_MMCODE_END 0x0142

struct CPURegister {
  union {
    struct {
      uint8_t A;
      uint8_t F;
    };
    uint16_t AF;
  };
  union {
    struct {
      uint8_t B;
      uint8_t C;
    };
    uint16_t BC;
  };
  union {
    struct {
      uint8_t D;
      uint8_t E;
    };
    uint16_t DE;
  };
  union {
    struct {
      uint8_t H;
      uint8_t L;
    };
    uint16_t HL;
  };
  union {
    struct {
      uint8_t S;
      uint8_t P;
    };
    uint16_t SP;
  };
  uint16_t PC;
} CPURegister;

#define ZFLAG CPURegister.F |= 0x10000000
#define ZCLEAR CPURegister.F &= 0x01111111

#define NFLAG CPURegister.F |= 0x01000000
#define NCLEAR CPURegister.F &= 0x10111111

#define HFLAG CPURegister.F |= 0x00100000
#define HCLEAR CPURegister.F &= 0x11011111

#define CFLAG CPURegister.F |= 0x00010000
#define CCLEAR CPURegister.F &= 0x11101111

unsigned char memory[0xffff];

#define LD_R_N16(RH, RL) \
  []() {                 \
    RL = memory[PC++];   \
    RH = memory[PC++];   \
    return 12u;          \
  }

#define LD_R1_VR2(R1, R2) \
  []() {                  \
    R1 = memory[R2];      \
    return 8u;            \
  }

#define INC_R_FLAG(R)            \
  []() {                         \
    R++;                         \
    R ? ZFLAG : ZCLEAR;          \
    NCLEAR;                      \
    (R & 0x0f) ? HFLAG : HCLEAR; \
    return 4u;                   \
  };

#define DEC_R_FLAG(R)            \
  []() {                         \
    R--;                         \
    R ? ZFLAG : ZCLEAR;          \
    NFLAG;                       \
    (R & 0x0f) ? HFLAG : HCLEAR; \
    return 4u;                   \
  };

#define LD_VR1_R2(R1, R2) \
  []() {                  \
    memory[R1] = R2;      \
    return 8u;            \
  };

#define INC_R(R) \
  []() {         \
    R++;         \
    return 8u;   \
  };

#define LD_R_N8(R)    \
  []() {              \
    R = memory[PC++]; \
    return 8u;        \
  };

#define ADD_R1_R2(R1, R2)                                \
  []() {                                                 \
    uint32_t result = R1 + R2;                           \
    (result & 0xff00) ? CFLAG : CCLEAR;                  \
    NCLEAR;                                              \
    ((R1 & 0x0f) + (R2 & 0x0f)) > 0x0f ? HFLAG : HCLEAR; \
    R1 = result;                                         \
    return 8u;                                           \
  };

#define ADC_R1_R2(R1, R2)                                     \
  []() {                                                      \
    uint8_t cy = CFLAG ? 1u : 0u;                             \
    uint32_t result = R1 + R2 + cy;                           \
    (result & 0xff00) ? CFLAG : CCLEAR;                       \
    NCLEAR;                                                   \
    ((R1 & 0x0f) + (R2 & 0x0f)) + cy > 0x0f ? HFLAG : HCLEAR; \
    R1 = result;                                              \
    return 8u;                                                \
  };

#define SUB_R1_R2(R1, R2)                                \
  []() {                                                 \
    int32_t result = R1 - R2;                            \
    (result & 0xff00) ? CFLAG : CCLEAR;                  \
    NFLAG;                                               \
    ((R1 & 0x0f) - (R2 & 0x0f)) & 0xf0 ? HFLAG : HCLEAR; \
    R1 = result;                                         \
    return 8u;                                           \
  };

#define SBC_R1_R2(R1, R2)                                     \
  []() {                                                      \
    uint8_t cy = CFLAG ? 1u : 0u;                             \
    uint32_t result = R1 - R2 - cy;                           \
    (result & 0xff00) ? CFLAG : CCLEAR;                       \
    NFLAG;                                                    \
    ((R1 & 0x0f) - (R2 & 0x0f)) - cy & 0x0f ? HFLAG : HCLEAR; \
    R1 = result;                                              \
    return 8u;                                                \
  };

#define AND_R1_R2(R1, R2) \
  []() {                  \
    F = 0b00100000;       \
    R1 &= R2;             \
    R1 ? ZCLEAR : ZFLAG;  \
    return 4u;            \
  };

#define XOR_R1_R2(R1, R2) \
  []() {                  \
    F = 0b00000000;       \
    R1 ^= R2;             \
    R1 ? ZCLEAR : ZFLAG;  \
    return 4u;            \
  };

#define OR_R1_R2(R1, R2) \
  []() {                 \
    F = 0b00000000;      \
    R1 |= R2;            \
    R1 ? ZCLEAR : ZFLAG; \
    return 4u;           \
  };

#define CP_R1_R2(R1, R2)                        \
  []() {                                        \
    R1 == R2 ? ZFLAG : ZCLEAR;                  \
    R1 > R2 ? CCLEAR : CFLAG;                   \
    (R1 & 0x0f) > (R2 & 0x0f) ? HCLEAR : HFLAG; \
    NFLAG;                                      \
    return 4u;                                  \
  };

#define DEC_R(R) \
  []() {         \
    R--;         \
    return 8u;   \
  };

#define JR_OFFSET           \
  auto tmp = memory[PC];    \
  PC += (signed char)(tmp); \
  return 12u;

#define LD_R1_R2(R1, R2) \
  []() {                 \
    R1 = R2;             \
    return 4u;           \
  };

#define LD_RR []() { return 4u; };

#define HALT []() { return 4u; };

#define COUT_HEX(value) \
  cout << "0x" << hex << setw(2) << setfill('0') << (int)(value)

function<unsigned()> instructions[256];

void LoadInstructions() {
  auto &A = CPURegister.A;
  auto &F = CPURegister.F;
  auto &B = CPURegister.B;
  auto &C = CPURegister.C;
  auto &BC = CPURegister.BC;
  auto &D = CPURegister.D;
  auto &E = CPURegister.E;
  auto &DE = CPURegister.DE;
  auto &H = CPURegister.H;
  auto &L = CPURegister.L;
  auto &HL = CPURegister.HL;
  auto &S = CPURegister.S;
  auto &P = CPURegister.P;
  auto &SP = CPURegister.SP;
  auto &PC = CPURegister.PC;

  instructions[0x00] = []() { return 4u; };
  instructions[0x01] = LD_R_N16(B, C);
  instructions[0x02] = LD_VR1_R2(BC, A);
  instructions[0x03] = INC_R(BC);
  instructions[0x04] = INC_R_FLAG(B);
  instructions[0x05] = DEC_R_FLAG(B);
  instructions[0x06] = LD_R_N8(B);
  instructions[0x07] = []() {
    auto carry = A >> 7;
    A <<= 1;
    A += carry;
    F = carry ? 0x10 : 0x00;
    return 4u;
  };
  instructions[0x08] = []() {
    auto addres = memory[PC++] + (memory[PC++] << 8);
    memory[addres] = P;
    memory[addres + 1] = S;
    return 20u;
  };
  instructions[0x09] = ADD_R1_R2(HL, BC);
  instructions[0x0a] = LD_R1_VR2(A, BC);
  instructions[0x0b] = DEC_R(BC);
  instructions[0x0c] = INC_R_FLAG(C);
  instructions[0x0d] = DEC_R_FLAG(C);
  instructions[0x0e] = LD_R_N8(C);
  instructions[0x0f] = []() {
    auto carry = A & 0x01;
    A >> 1;
    if (carry) {
      A |= 0x80;
    }
    F = carry ? 0x10 : 0x00;
    return 4u;
  };

  instructions[0x10] = []() {
    PC++;
    // Exit program.
    return 4u;
  };
  instructions[0x11] = LD_R_N16(D, E);
  instructions[0x12] = LD_VR1_R2(DE, A);
  instructions[0x13] = INC_R(DE);
  instructions[0x14] = INC_R_FLAG(D);
  instructions[0x15] = DEC_R_FLAG(D);
  instructions[0x16] = LD_R_N8(D);
  instructions[0x17] = []() {
    auto carry = A >> 7;
    A <<= 1;
    A += (CFLAG >> 4);
    F = carry ? 0x10 : 0x00;
    return 4u;
  };
  instructions[0x18] = []() { JR_OFFSET; };
  instructions[0x19] = ADD_R1_R2(HL, DE);
  instructions[0x1a] = LD_R1_VR2(A, DE);
  instructions[0x1b] = DEC_R(DE);
  instructions[0x1c] = INC_R_FLAG(E);
  instructions[0x1d] = DEC_R_FLAG(E);
  instructions[0x1e] = LD_R_N8(E);
  instructions[0x1f] = []() {
    auto carry = A & 0x01;
    A >> 1;
    if (CFLAG) {
      A |= 0x80;
    }
    F = carry ? 0x10 : 0x00;
    return 4u;
  };
  instructions[0x20] = []() {
    if (ZFLAG) {
      PC++;
      return 8u;
    }
    JR_OFFSET;
  };
  instructions[0x21] = LD_R_N16(H, L);
  instructions[0x22] = []() {
    memory[HL++] = A;
    return 8u;
  };
  instructions[0x23] = INC_R(HL);
  instructions[0x24] = INC_R_FLAG(H);
  instructions[0x25] = DEC_R_FLAG(H);
  instructions[0x26] = LD_R_N8(H);
  instructions[0x27] = []() {
    auto tmp = A;
    if (NFLAG) {
      if (HFLAG) {
        tmp = (tmp - 0x06) & 0xff;
      }
      if (CFLAG) {
        tmp -= 0x60;
      }
    } else {
      if (HFLAG || (tmp & 0xf) > 9) {
        tmp += 0x06;
      }
      if (CFLAG || tmp > 0x97) {
        tmp += 0x60;
      }
    }
    A = tmp;
    HCLEAR;
    A ? ZCLEAR : ZFLAG;
    tmp >= 0x0100 ? CFLAG : CCLEAR;
    return 4u;
  };
  instructions[0x28] = []() {
    if (ZFLAG) {
      JR_OFFSET;
    }
    PC++;
    return 8u;
  };
  instructions[0x29] = ADD_R1_R2(HL, HL);
  instructions[0x2a] = []() {
    A = memory[HL++];
    return 8u;
  };
  instructions[0x2b] = DEC_R(HL);
  instructions[0x2c] = INC_R_FLAG(L);
  instructions[0x2d] = DEC_R_FLAG(L);
  instructions[0x2e] = LD_R_N8(L);
  instructions[0x2f] = []() {
    A ^= 0xff;
    F |= 0x01100000;
    return 4u;
  };
  instructions[0x30] = []() {
    if (CFLAG) {
      PC++;
      return 8u;
    }
    JR_OFFSET;
  };
  instructions[0x31] = LD_R_N16(S, P);
  instructions[0x32] = []() {
    memory[HL--] = A;
    return 8u;
  };
  instructions[0x33] = INC_R(SP);
  instructions[0x34] = []() {
    uint8_t value = memory[HL] + 1;
    value ? ZCLEAR : ZFLAG;
    NCLEAR;
    (value & 0x0f) ? HCLEAR : HFLAG;
    memory[HL] = value;
    return 12u;
  };
  instructions[0x35] = []() {
    uint8_t value = memory[HL] - 1;
    value ? ZCLEAR : ZFLAG;
    NCLEAR;
    ((value & 0x0f) != 0xf) ? HCLEAR : HFLAG;
    memory[HL] = value;
    return 12u;
  };
  instructions[0x36] = []() {
    memory[HL] = memory[PC++];
    return 12u;
  };
  instructions[0x37] = []() {
    F = (F & 0x10000000) | 0x00010000;
    return 4u;
  };
  instructions[0x38] = []() {
    if (CFLAG) {
      JR_OFFSET;
    }
    PC++;
    return 8u;
  };
  instructions[0x39] = []() {
    uint32_t tmp = HL + SP;
    NCLEAR;
    tmp > 0xffff ? CFLAG : CCLEAR;
    (L + P) > 0xff ? HFLAG : HCLEAR;
    HL = tmp;
    return 8u;
  };
  instructions[0x3a] = []() {
    A = memory[HL--];
    return 8u;
  };
  instructions[0x3b] = DEC_R(SP);
  instructions[0x3c] = INC_R_FLAG(A);
  instructions[0x3d] = DEC_R_FLAG(A);
  instructions[0x3e] = LD_R_N8(A);
  instructions[0x3f] = []() {
    (F & 0x00010000) ? CCLEAR : CFLAG;
    F &= 0x10010000;
    return 4u;
  };

  instructions[0x40] = LD_RR;
  instructions[0x41] = LD_R1_R2(B, C);
  instructions[0x42] = LD_R1_R2(B, D);
  instructions[0x43] = LD_R1_R2(B, E);
  instructions[0x44] = LD_R1_R2(B, H);
  instructions[0x45] = LD_R1_R2(B, L);
  instructions[0x46] = LD_R1_VR2(B, HL);
  instructions[0x47] = LD_R1_R2(B, A);

  instructions[0x48] = LD_R1_R2(C, B);
  instructions[0x49] = LD_RR;
  instructions[0x4a] = LD_R1_R2(C, D);
  instructions[0x4b] = LD_R1_R2(C, E);
  instructions[0x4c] = LD_R1_R2(C, H);
  instructions[0x4d] = LD_R1_R2(C, L);
  instructions[0x4e] = LD_R1_VR2(C, HL);
  instructions[0x4f] = LD_R1_R2(C, A);

  instructions[0x50] = LD_R1_R2(D, B);
  instructions[0x51] = LD_R1_R2(D, C);
  instructions[0x52] = LD_RR;
  instructions[0x53] = LD_R1_R2(D, E);
  instructions[0x54] = LD_R1_R2(D, H);
  instructions[0x55] = LD_R1_R2(D, L);
  instructions[0x56] = LD_R1_VR2(D, HL);
  instructions[0x57] = LD_R1_R2(D, A);

  instructions[0x58] = LD_R1_R2(E, B);
  instructions[0x59] = LD_R1_R2(E, C);
  instructions[0x5a] = LD_R1_R2(E, D);
  instructions[0x5b] = LD_RR;
  instructions[0x5c] = LD_R1_R2(E, H);
  instructions[0x5d] = LD_R1_R2(E, L);
  instructions[0x5e] = LD_R1_VR2(E, HL);
  instructions[0x5f] = LD_R1_R2(E, A);

  instructions[0x60] = LD_R1_R2(H, B);
  instructions[0x61] = LD_R1_R2(H, C);
  instructions[0x62] = LD_R1_R2(H, D);
  instructions[0x63] = LD_R1_R2(H, E);
  instructions[0x64] = LD_RR;
  instructions[0x65] = LD_R1_R2(H, L);
  instructions[0x66] = LD_R1_VR2(H, HL);
  instructions[0x67] = LD_R1_R2(H, A);

  instructions[0x68] = LD_R1_R2(L, B);
  instructions[0x69] = LD_R1_R2(L, C);
  instructions[0x6a] = LD_R1_R2(L, D);
  instructions[0x6b] = LD_R1_R2(L, E);
  instructions[0x6c] = LD_R1_R2(L, H);
  instructions[0x6d] = LD_RR;
  instructions[0x6e] = LD_R1_VR2(L, HL);
  instructions[0x6f] = LD_R1_R2(L, A);

  instructions[0x70] = LD_VR1_R2(HL, B);
  instructions[0x71] = LD_VR1_R2(HL, C);
  instructions[0x72] = LD_VR1_R2(HL, D);
  instructions[0x73] = LD_VR1_R2(HL, E);
  instructions[0x74] = LD_VR1_R2(HL, H);
  instructions[0x75] = LD_VR1_R2(HL, L);
  instructions[0x76] = LD_RR;
  instructions[0x77] = LD_VR1_R2(HL, A);

  instructions[0x78] = LD_R1_R2(A, B);
  instructions[0x79] = LD_R1_R2(A, C);
  instructions[0x7a] = LD_R1_R2(A, D);
  instructions[0x7b] = LD_R1_R2(A, E);
  instructions[0x7c] = LD_R1_R2(A, H);
  instructions[0x7d] = LD_R1_R2(A, L);
  instructions[0x7e] = LD_R1_VR2(A, HL);
  instructions[0x7f] = LD_RR;

  instructions[0x80] = ADD_R1_R2(A, B);
  instructions[0x81] = ADD_R1_R2(A, C);
  instructions[0x82] = ADD_R1_R2(A, D);
  instructions[0x83] = ADD_R1_R2(A, E);
  instructions[0x84] = ADD_R1_R2(A, H);
  instructions[0x85] = ADD_R1_R2(A, L);
  instructions[0x86] = ADD_R1_R2(A, memory[HL]);
  instructions[0x87] = ADD_R1_R2(A, A);

  instructions[0x88] = ADC_R1_R2(A, B);
  instructions[0x89] = ADC_R1_R2(A, C);
  instructions[0x8a] = ADC_R1_R2(A, D);
  instructions[0x8b] = ADC_R1_R2(A, E);
  instructions[0x8c] = ADC_R1_R2(A, H);
  instructions[0x8d] = ADC_R1_R2(A, L);
  instructions[0x8e] = ADC_R1_R2(A, memory[HL]);
  instructions[0x8f] = ADC_R1_R2(A, A);

  instructions[0x90] = SUB_R1_R2(A, B);
  instructions[0x91] = SUB_R1_R2(A, C);
  instructions[0x92] = SUB_R1_R2(A, D);
  instructions[0x93] = SUB_R1_R2(A, E);
  instructions[0x94] = SUB_R1_R2(A, H);
  instructions[0x95] = SUB_R1_R2(A, L);
  instructions[0x96] = SUB_R1_R2(A, memory[HL]);
  instructions[0x97] = SUB_R1_R2(A, A);

  instructions[0x98] = SBC_R1_R2(A, B);
  instructions[0x99] = SBC_R1_R2(A, C);
  instructions[0x9a] = SBC_R1_R2(A, D);
  instructions[0x9b] = SBC_R1_R2(A, E);
  instructions[0x9c] = SBC_R1_R2(A, H);
  instructions[0x9d] = SBC_R1_R2(A, L);
  instructions[0x9e] = SBC_R1_R2(A, memory[HL]);
  instructions[0x9f] = SBC_R1_R2(A, A);

  instructions[0xa0] = AND_R1_R2(A, B);
  instructions[0xa1] = AND_R1_R2(A, C);
  instructions[0xa2] = AND_R1_R2(A, D);
  instructions[0xa3] = AND_R1_R2(A, E);
  instructions[0xa4] = AND_R1_R2(A, H);
  instructions[0xa5] = AND_R1_R2(A, L);
  instructions[0xa6] = AND_R1_R2(A, memory[HL]);
  instructions[0xa7] = AND_R1_R2(A, A);

  instructions[0xa8] = XOR_R1_R2(A, B);
  instructions[0xa9] = XOR_R1_R2(A, C);
  instructions[0xaa] = XOR_R1_R2(A, D);
  instructions[0xab] = XOR_R1_R2(A, E);
  instructions[0xac] = XOR_R1_R2(A, H);
  instructions[0xad] = XOR_R1_R2(A, L);
  instructions[0xae] = XOR_R1_R2(A, memory[HL]);
  instructions[0xaf] = XOR_R1_R2(A, A);

  instructions[0xb0] = OR_R1_R2(A, B);
  instructions[0xb1] = OR_R1_R2(A, C);
  instructions[0xb2] = OR_R1_R2(A, D);
  instructions[0xb3] = OR_R1_R2(A, E);
  instructions[0xb4] = OR_R1_R2(A, H);
  instructions[0xb5] = OR_R1_R2(A, L);
  instructions[0xb6] = OR_R1_R2(A, memory[HL]);
  instructions[0xb7] = OR_R1_R2(A, A);

  instructions[0xb8] = CP_R1_R2(A, B);
  instructions[0xb9] = CP_R1_R2(A, C);
  instructions[0xba] = CP_R1_R2(A, D);
  instructions[0xbb] = CP_R1_R2(A, E);
  instructions[0xbc] = CP_R1_R2(A, H);
  instructions[0xbd] = CP_R1_R2(A, L);
  instructions[0xbe] = CP_R1_R2(A, memory[HL]);
  instructions[0xbf] = CP_R1_R2(A, A);

  // Dang them
  instructions[0xc3] = []() {
    PC = memory[PC] + (memory[PC + 1] << 8);
    return 16u;
  };
  instructions[0xaf] = []() {
    A ^= A;
    ZFLAG;
    return 4u;
  };
}

void BootRom() {
  CPURegister.A = 0x01;
  CPURegister.F = 0x00;
  CPURegister.B = 0xff;
  CPURegister.C = 0x13;
  CPURegister.D = 0x00;
  CPURegister.E = 0xc1;
  CPURegister.H = 0x84;
  CPURegister.L = 0x03;
  CPURegister.PC = 0x100;
  CPURegister.SP = 0xfffe;
}

int main(int argc, char *argv[]) {
  cout << "Hello 8080" << endl;

  ifstream fileRoom(FILE_ROOM, ios::in | ios::binary | ios::ate);
  auto size = fileRoom.tellg();
  cout << "Size rom " << size << endl;
  if (size < 0x180) {
    cout << "File room error" << endl;
    return 1;
  }
  fileRoom.seekg(0, ios::beg);
  auto *rom = memory;
  fileRoom.read((char *)(rom), size);
  fileRoom.close();

  unsigned char *logo = rom + ROM_LOGO_START;
  cout << "Logo" << endl;
  for (auto indexY = 0u; indexY < 3; indexY++) {
    for (auto indexX = 0u; indexX < 16; indexX++) {
      cout << hex << setw(2) << setfill('0')
           << (int)(logo[indexY * 16 + indexX]) << " ";
    }
    cout << endl;
  }

  unsigned char *title = rom + ROM_TITLE_START;
  cout << "Title: " << string((char *)(title), ROM_TITLE_END - ROM_TITLE_START)
       << endl;

  LoadInstructions();
  BootRom();
  for (;;) {
    // // if (CPURegister.PC == 0x0216) {
    // cout << "Debug CPU **********" << endl;
    // cout << "PC " << (int)(CPURegister.PC) << endl;
    // cout << "Opcode 0x" << hex << setw(2) << setfill('0')
    //      << (int)(memory[CPURegister.PC]) << endl;
    // // cout << "ZFlag " << (int)(ZFLAG) << endl;
    // // cout << "Memory " << (int)(memory[CPURegister.PC + 1]) << endl;
    // // }
    //
    if (CPURegister.PC == 0x0217) {
      getchar();
    }
    //
    // if (!memory[0x0217]) {
    //   cout << "Memory reset to 0" << endl;
    // }
    //
    // if (CPURegister.HL == 0x0217) {
    //   cout << "HL chang to data ********** " << endl;
    //   getchar();
    // }

    cout << "****************************" << endl;
    COUT_HEX(CPURegister.A) << " ";
    COUT_HEX(CPURegister.F) << endl;
    COUT_HEX(CPURegister.B) << " ";
    COUT_HEX(CPURegister.C) << endl;
    COUT_HEX(CPURegister.D) << " ";
    COUT_HEX(CPURegister.E) << endl;
    COUT_HEX(CPURegister.H) << " ";
    COUT_HEX(CPURegister.L) << endl;

    cout << "PC ";
    COUT_HEX(CPURegister.PC) << endl;
    COUT_HEX(memory[CPURegister.PC]) << " ";
    COUT_HEX(memory[CPURegister.PC + 1]) << " ";
    COUT_HEX(memory[CPURegister.PC + 2]) << " ";
    getchar();

    instructions[memory[CPURegister.PC++]]();
  }

  return 0;
}
