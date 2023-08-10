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
    uint16_t SP = 0xffff;
  };
  uint16_t PC = 0x0100;
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
    NFLAG;                                               \
    ((R1 & 0x0f) + (R2 & 0x0f)) > 0x0f ? HFLAG : HCLEAR; \
    return 8u;                                           \
  };

#define DEC_R(R) \
  []() {         \
    R--;         \
    return 8u;   \
  };

#define JR_OFFSET                  \
  PC += (signed char)(memory[PC]); \
  return 12u;

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

  // Dang them
  instructions[0x32] = []() {
    memory[HL] = A;
    HL--;
    return 8u;
  };
  instructions[0x21] = LD_R_N16(H, L);
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
  unsigned count = 0;
  for (;;) {
    cout << "0x" << hex << setw(4) << (int)(CPURegister.PC) << " 0x" << hex
         << setw(2) << (int)(memory[CPURegister.PC]) << endl;
    instructions[memory[CPURegister.PC++]]();
    if (!memory[CPURegister.PC]) {
      if (count++ > 30) {
        count = 0;
        getchar();
      }
    } else {
      count = 0;
    }
  }

  return 0;
}
