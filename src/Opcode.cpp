#include "Opcode.hpp"

#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;

#define BUFFER_SIZE 1024

unsigned SplitNext(char *buffer, unsigned start, char key = ',') {
  for (; buffer[start] != '\0' && buffer[start] != key; start++) {
  }
  return start;
}

void ShowOpcode(Opcode &opcode) {
  cout << "0x" << hex << (int)(opcode.code) << "\t" << opcode.instruction
       << "\t\t" << (int)(opcode.size) << '\t' << opcode.flags << "\t\t\t"
       << opcode.function << endl;
}

#define CASE_SHOW_ONE(value, instruction)                          \
  case value:                                                      \
    cout << "0x" << hex << setw(2) << value << " " << instruction; \
    break;

#define CASE_SHOW_TWO(value, instruction)                                  \
  case value:                                                              \
    cout << "0x" << hex << setw(2) << value << " " << instruction << " 0x" \
         << hex << setw(2) << setfill('0') << (int)(buffer[pc++]);         \
    break;

#define CASE_SHOW_THREE(value, instruction)                                \
  case value:                                                              \
    cout << "0x" << hex << setw(2) << value << " " << instruction << " 0x" \
         << hex << setw(2) << setfill('0') << (int)(buffer[pc++]) << " 0x" \
         << hex << setw(2) << setfill('0') << (int)(buffer[pc++]);         \
    break;

unsigned ShowOpcode(uint8_t *buffer, unsigned pc) {
  switch (buffer[pc++]) {
    CASE_SHOW_ONE(0x00, "NOP")
    CASE_SHOW_THREE(0x01, "LXI B")
    CASE_SHOW_ONE(0x02, "STAX B")
    CASE_SHOW_ONE(0x03, "INX B")
    CASE_SHOW_ONE(0x04, "INR B")
    CASE_SHOW_ONE(0x05, "DCR B")
    CASE_SHOW_TWO(0x06, "MVI B")
    CASE_SHOW_ONE(0x07, "RLC")

    CASE_SHOW_ONE(0x08, "-")
    CASE_SHOW_ONE(0x09, "DAD B")
    CASE_SHOW_ONE(0x0a, "LDAX B")
    CASE_SHOW_ONE(0x0b, "DCX B")
    CASE_SHOW_ONE(0x0c, "INR C")
    CASE_SHOW_ONE(0x0d, "DCR C")
    CASE_SHOW_TWO(0x0e, "MVI C")
    CASE_SHOW_ONE(0x0f, "RRC")

    CASE_SHOW_ONE(0x10, "-")
    CASE_SHOW_THREE(0x11, "LXI D")
    CASE_SHOW_ONE(0x12, "STAX D")
    CASE_SHOW_ONE(0x13, "INX D")
    CASE_SHOW_ONE(0x14, "INR D")
    CASE_SHOW_ONE(0x15, "DCR D")
    CASE_SHOW_TWO(0x16, "MVI D")
    CASE_SHOW_ONE(0x17, "RAL")

    CASE_SHOW_ONE(0x18, "-")
    CASE_SHOW_ONE(0x19, "DAD D")
    CASE_SHOW_ONE(0x1a, "LDAX D")
    CASE_SHOW_ONE(0x1b, "DCX D")
    CASE_SHOW_ONE(0x1c, "INR E")
    CASE_SHOW_ONE(0x1d, "DCR E")
    CASE_SHOW_TWO(0x1e, "MVI E")
    CASE_SHOW_ONE(0x1f, "RAR")

    CASE_SHOW_ONE(0x20, "RIM")
    CASE_SHOW_THREE(0x21, "LXI H")
    CASE_SHOW_THREE(0x22, "SHLD")
    CASE_SHOW_ONE(0x23, "INX H")
    CASE_SHOW_ONE(0x24, "INR H")
    CASE_SHOW_ONE(0x25, "DCR H")
    CASE_SHOW_TWO(0x26, "MVI H")
    CASE_SHOW_ONE(0x27, "DAA")

    CASE_SHOW_ONE(0x28, "-")
    CASE_SHOW_ONE(0x29, "DAD H")
    CASE_SHOW_THREE(0x2a, "LHLD")
    CASE_SHOW_ONE(0x2b, "DCX H")
    CASE_SHOW_ONE(0x2c, "INR L")
    CASE_SHOW_ONE(0x2d, "DCR L")
    CASE_SHOW_TWO(0x2e, "MVI L")
    CASE_SHOW_ONE(0x2f, "CMA")

    CASE_SHOW_ONE(0x30, "SIM")
    CASE_SHOW_THREE(0x31, "LXI SP")
    CASE_SHOW_THREE(0x32, "STA")
    CASE_SHOW_ONE(0x33, "INX SP")
    CASE_SHOW_ONE(0x34, "INR M")
    CASE_SHOW_ONE(0x35, "DCR M")
    CASE_SHOW_TWO(0x36, "MVI M")
    CASE_SHOW_ONE(0x37, "STC")

    CASE_SHOW_ONE(0x38, "-")
    CASE_SHOW_ONE(0x39, "DAD SP")
    CASE_SHOW_THREE(0x3a, "LDA")
    CASE_SHOW_ONE(0x3b, "DCX SP")
    CASE_SHOW_ONE(0x3c, "INR A")
    CASE_SHOW_ONE(0x3d, "DCR A")
    CASE_SHOW_TWO(0x3e, "MVI A")
    CASE_SHOW_ONE(0x3f, "CMC")

    CASE_SHOW_ONE(0x40, "MOV B,B")
    CASE_SHOW_ONE(0x41, "MOV B,C")
    CASE_SHOW_ONE(0x42, "MOV B,D")
    CASE_SHOW_ONE(0x43, "MOV B,E")
    CASE_SHOW_ONE(0x44, "MOV B,H")
    CASE_SHOW_ONE(0x45, "MOV B,L")
    CASE_SHOW_ONE(0x46, "MOV B,M")
    CASE_SHOW_ONE(0x47, "MOV B,A")

    CASE_SHOW_ONE(0x48, "MOV C,B")
    CASE_SHOW_ONE(0x49, "MOV C,C")
    CASE_SHOW_ONE(0x4a, "MOV C,D")
    CASE_SHOW_ONE(0x4b, "MOV C,E")
    CASE_SHOW_ONE(0x4c, "MOV C,H")
    CASE_SHOW_ONE(0x4d, "MOV C,L")
    CASE_SHOW_ONE(0x4e, "MOV C,M")
    CASE_SHOW_ONE(0x4f, "MOV C,A")

    CASE_SHOW_ONE(0x50, "MOV D,B")
    CASE_SHOW_ONE(0x51, "MOV D,C")
    CASE_SHOW_ONE(0x52, "MOV D,D")
    CASE_SHOW_ONE(0x53, "MOV D,E")
    CASE_SHOW_ONE(0x54, "MOV D,H")
    CASE_SHOW_ONE(0x55, "MOV D,L")
    CASE_SHOW_ONE(0x56, "MOV D,M")
    CASE_SHOW_ONE(0x57, "MOV D,A")

    CASE_SHOW_ONE(0x58, "MOV E,B")
    CASE_SHOW_ONE(0x59, "MOV E,C")
    CASE_SHOW_ONE(0x5a, "MOV E,D")
    CASE_SHOW_ONE(0x5b, "MOV E,E")
    CASE_SHOW_ONE(0x5c, "MOV E,H")
    CASE_SHOW_ONE(0x5d, "MOV E,L")
    CASE_SHOW_ONE(0x5e, "MOV E,M")
    CASE_SHOW_ONE(0x5f, "MOV E,A")

    CASE_SHOW_ONE(0x60, "MOV H,B")
    CASE_SHOW_ONE(0x61, "MOV H,C")
    CASE_SHOW_ONE(0x62, "MOV H,D")
    CASE_SHOW_ONE(0x63, "MOV H,E")
    CASE_SHOW_ONE(0x64, "MOV H,H")
    CASE_SHOW_ONE(0x65, "MOV H,L")
    CASE_SHOW_ONE(0x66, "MOV H,M")
    CASE_SHOW_ONE(0x67, "MOV H,A")

    CASE_SHOW_ONE(0x68, "MOV L,B")
    CASE_SHOW_ONE(0x69, "MOV L,C")
    CASE_SHOW_ONE(0x6a, "MOV L,D")
    CASE_SHOW_ONE(0x6b, "MOV L,E")
    CASE_SHOW_ONE(0x6c, "MOV L,H")
    CASE_SHOW_ONE(0x6d, "MOV L,L")
    CASE_SHOW_ONE(0x6e, "MOV L,M")
    CASE_SHOW_ONE(0x6f, "MOV L,A")

    CASE_SHOW_ONE(0x70, "MOV M,B")
    CASE_SHOW_ONE(0x71, "MOV M,C")
    CASE_SHOW_ONE(0x72, "MOV M,D")
    CASE_SHOW_ONE(0x73, "MOV M,E")
    CASE_SHOW_ONE(0x74, "MOV M,H")
    CASE_SHOW_ONE(0x75, "MOV M,L")
    CASE_SHOW_ONE(0x76, "HLT")
    CASE_SHOW_ONE(0x77, "MOV M,A")

    CASE_SHOW_ONE(0x78, "MOV A,B")
    CASE_SHOW_ONE(0x79, "MOV A,C")
    CASE_SHOW_ONE(0x7a, "MOV A,D")
    CASE_SHOW_ONE(0x7b, "MOV A,E")
    CASE_SHOW_ONE(0x7c, "MOV A,H")
    CASE_SHOW_ONE(0x7d, "MOV A,L")
    CASE_SHOW_ONE(0x7e, "MOV A,M")
    CASE_SHOW_ONE(0x7f, "MOV A,A")

    CASE_SHOW_ONE(0x80, "ADD B")
    CASE_SHOW_ONE(0x81, "ADD C")
    CASE_SHOW_ONE(0x82, "ADD D")
    CASE_SHOW_ONE(0x83, "ADD E")
    CASE_SHOW_ONE(0x84, "ADD H")
    CASE_SHOW_ONE(0x85, "ADD L")
    CASE_SHOW_ONE(0x86, "ADD M")
    CASE_SHOW_ONE(0x87, "ADD A")

    CASE_SHOW_ONE(0x88, "ADC B")
    CASE_SHOW_ONE(0x89, "ADC C")
    CASE_SHOW_ONE(0x8a, "ADC D")
    CASE_SHOW_ONE(0x8b, "ADC E")
    CASE_SHOW_ONE(0x8c, "ADC H")
    CASE_SHOW_ONE(0x8d, "ADC L")
    CASE_SHOW_ONE(0x8e, "ADC M")
    CASE_SHOW_ONE(0x8f, "ADC A")

    CASE_SHOW_ONE(0x90, "SUB B")
    CASE_SHOW_ONE(0x91, "SUB C")
    CASE_SHOW_ONE(0x92, "SUB D")
    CASE_SHOW_ONE(0x93, "SUB E")
    CASE_SHOW_ONE(0x94, "SUB H")
    CASE_SHOW_ONE(0x95, "SUB L")
    CASE_SHOW_ONE(0x96, "SUB M")
    CASE_SHOW_ONE(0x97, "SUB A")

    CASE_SHOW_ONE(0x98, "SBB B")
    CASE_SHOW_ONE(0x99, "SBB C")
    CASE_SHOW_ONE(0x9a, "SBB D")
    CASE_SHOW_ONE(0x9b, "SBB E")
    CASE_SHOW_ONE(0x9c, "SBB H")
    CASE_SHOW_ONE(0x9d, "SBB L")
    CASE_SHOW_ONE(0x9e, "SBB M")
    CASE_SHOW_ONE(0x9f, "SBB A")

    CASE_SHOW_ONE(0xa0, "ANA B")
    CASE_SHOW_ONE(0xa1, "ANA C")
    CASE_SHOW_ONE(0xa2, "ANA D")
    CASE_SHOW_ONE(0xa3, "ANA E")
    CASE_SHOW_ONE(0xa4, "ANA H")
    CASE_SHOW_ONE(0xa5, "ANA L")
    CASE_SHOW_ONE(0xa6, "ANA M")
    CASE_SHOW_ONE(0xa7, "ANA A")

    CASE_SHOW_ONE(0xa8, "XRA B")
    CASE_SHOW_ONE(0xa9, "XRA C")
    CASE_SHOW_ONE(0xaa, "XRA D")
    CASE_SHOW_ONE(0xab, "XRA E")
    CASE_SHOW_ONE(0xac, "XRA H")
    CASE_SHOW_ONE(0xad, "XRA L")
    CASE_SHOW_ONE(0xae, "XRA M")
    CASE_SHOW_ONE(0xaf, "XRA A")

    CASE_SHOW_ONE(0xb0, "ORA B")
    CASE_SHOW_ONE(0xb1, "ORA C")
    CASE_SHOW_ONE(0xb2, "ORA D")
    CASE_SHOW_ONE(0xb3, "ORA E")
    CASE_SHOW_ONE(0xb4, "ORA H")
    CASE_SHOW_ONE(0xb5, "ORA L")
    CASE_SHOW_ONE(0xb6, "ORA M")
    CASE_SHOW_ONE(0xb7, "ORA A")

    CASE_SHOW_ONE(0xb8, "CMP B")
    CASE_SHOW_ONE(0xb9, "CMP C")
    CASE_SHOW_ONE(0xba, "CMP D")
    CASE_SHOW_ONE(0xbb, "CMP E")
    CASE_SHOW_ONE(0xbc, "CMP H")
    CASE_SHOW_ONE(0xbd, "CMP L")
    CASE_SHOW_ONE(0xbe, "CMP M")
    CASE_SHOW_ONE(0xbf, "CMP A")

    CASE_SHOW_ONE(0xc0, "RNZ")
    CASE_SHOW_ONE(0xc1, "POP B")
    CASE_SHOW_THREE(0xc2, "JNZ")
    CASE_SHOW_THREE(0xc3, "JMP")
    CASE_SHOW_THREE(0xc4, "CNZ")
    CASE_SHOW_ONE(0xc5, "PUSH B")
    CASE_SHOW_TWO(0xc6, "ADI")
    CASE_SHOW_ONE(0xc7, "RST 0")
    CASE_SHOW_ONE(0xc8, "RZ")
    CASE_SHOW_ONE(0xc9, "RET")
    CASE_SHOW_THREE(0xca, "JZ")
    CASE_SHOW_ONE(0xcb, "-")
    CASE_SHOW_THREE(0xcc, "CZ")
    CASE_SHOW_THREE(0xcd, "CALL")
    CASE_SHOW_TWO(0xce, "ACI")
    CASE_SHOW_ONE(0xcf, "RST")
    CASE_SHOW_ONE(0xd0, "RNC")
    CASE_SHOW_ONE(0xd1, "POP D")
    CASE_SHOW_THREE(0xd2, "JNC")
    CASE_SHOW_TWO(0xd3, "OUT")
    CASE_SHOW_THREE(0xd4, "CNC")
    CASE_SHOW_ONE(0xd5, "PUSH D")
    CASE_SHOW_TWO(0xd6, "SUI")
    CASE_SHOW_ONE(0xd7, "RST")
    CASE_SHOW_ONE(0xd8, "RC")
    CASE_SHOW_ONE(0xd9, "-")
    CASE_SHOW_THREE(0xda, "JC")
    CASE_SHOW_TWO(0xdb, "IN")
    CASE_SHOW_THREE(0xdc, "RST")
    CASE_SHOW_ONE(0xdd, "-")
    CASE_SHOW_TWO(0xde, "SBI")
    CASE_SHOW_THREE(0xdf, "RST")
    CASE_SHOW_ONE(0xe0, "RPO")
    CASE_SHOW_ONE(0xe1, "POP H")
    CASE_SHOW_THREE(0xe2, "JPO")
    CASE_SHOW_ONE(0xe3, "XTHL")
    CASE_SHOW_THREE(0xe4, "CPO")
    CASE_SHOW_ONE(0xe5, "PUSH H")
    CASE_SHOW_TWO(0xe6, "ANI")
    CASE_SHOW_ONE(0xe7, "RST 4")
    CASE_SHOW_ONE(0xe8, "RPE")
    CASE_SHOW_ONE(0xe9, "PCHL")
    CASE_SHOW_THREE(0xea, "JPE")
    CASE_SHOW_ONE(0xeb, "XCHG")
    CASE_SHOW_ONE(0xec, "CPE")
    CASE_SHOW_ONE(0xed, "-")
    CASE_SHOW_TWO(0xee, "XRI")
    CASE_SHOW_ONE(0xef, "RST 5")
    CASE_SHOW_ONE(0xf0, "RP")
    CASE_SHOW_ONE(0xf1, "POP PSW")
    CASE_SHOW_THREE(0xf2, "JP")
    CASE_SHOW_ONE(0xf3, "DI")
    CASE_SHOW_THREE(0xf4, "CP")
    CASE_SHOW_ONE(0xf5, "PUSH PSW")
    CASE_SHOW_TWO(0xf6, "ORI")
    CASE_SHOW_ONE(0xf7, "RST 6")
    CASE_SHOW_ONE(0xf8, "RM")
    CASE_SHOW_ONE(0xf9, "SPHL")
    CASE_SHOW_THREE(0xfa, "JM")
    CASE_SHOW_ONE(0xfb, "EI")
    CASE_SHOW_THREE(0xfc, "CM")
    CASE_SHOW_ONE(0xfd, "-")
    CASE_SHOW_TWO(0xfe, "CPI")
    CASE_SHOW_ONE(0xff, "RST")
  }
  cout << endl;
  return pc;
}

#define CASE_ONE(vale, instruction) \
  case vale: {                      \
    cout << instruction;            \
    pc++;                           \
    break;                          \
  }
#define CASE_TWO(vale, instruction)               \
  case vale: {                                    \
    cout << instruction;                          \
    pc++;                                         \
    cout << hex << "\t0x" << (int)(buffer[pc++]); \
    break;                                        \
  }
#define CASE_THREE(vale, instruction)             \
  case vale: {                                    \
    cout << instruction;                          \
    pc++;                                         \
    cout << hex << "\t0x" << (int)(buffer[pc++]); \
    cout << hex << "\t0x" << (int)(buffer[pc++]); \
    break;                                        \
  }

unsigned InitOpcode(Opcode *&opcodes, ifstream &opcodeFile) {
  char buffer[BUFFER_SIZE];
  opcodeFile.getline(buffer, BUFFER_SIZE);
  int opcodeSize = stoi(buffer);
  opcodes = new Opcode[opcodeSize];

  for (auto index = 0u; index < opcodeSize || opcodeFile.eof(); index++) {
    opcodeFile.getline(buffer, BUFFER_SIZE);
    int startIndex = 0, endIndex;

    endIndex = SplitNext(buffer, startIndex);
    auto &opcode = opcodes[index];
    opcode.code =
        stoi(string(buffer, startIndex, endIndex - startIndex), 0, 16);

    if (buffer[endIndex + 1] == '"') {
      startIndex = endIndex + 2;
      endIndex = SplitNext(buffer, startIndex, '"');
    } else {
      startIndex = endIndex + 1;
      endIndex = SplitNext(buffer, startIndex);
    }
    opcode.instruction = string(buffer, startIndex, endIndex - startIndex);

    startIndex = endIndex + (buffer[endIndex] == '"' ? 2 : 1);
    if ((endIndex = SplitNext(buffer, startIndex)) > startIndex) {
      opcode.size = stoi(string(buffer, startIndex, endIndex - startIndex));
    }

    if (buffer[endIndex + 1] == '"') {
      startIndex = endIndex + 2;
      endIndex = SplitNext(buffer, startIndex, '"');
      opcode.flags = string(buffer, startIndex, endIndex - startIndex);
      startIndex = endIndex + 2;
    } else {
      startIndex = endIndex + 1;
      endIndex = SplitNext(buffer, startIndex);
      opcode.flags = string(buffer, startIndex, endIndex - startIndex);
      startIndex = endIndex + 1;
    }

    endIndex = SplitNext(buffer, startIndex, '"');
    opcode.function = string(buffer, startIndex, endIndex - startIndex);
  }
  return opcodeSize;
}

// void ArithFlags(uint8_t &Flag, uint8_t res) {
//   Flag &= (res |= 0x01111111);
//   Flag &= res ? 0x10111111 : 0xff;
//
//   state->cc.z = ((res & 0xff) == 0);
//   state->cc.s = (0x80 == (res & 0x80));  // res & 1000 0000
//   state->cc.p = parity(res & 0xff, 8);
//   state->cc.ac = (res > 0x09);  // checks if result is greater than 9
// }

unsigned Run(uint8_t buffer[], unsigned pc) {
  // static uint8_t CPU8080[12];
  // memset(CPU8080, 0x00, 12);
  // uint8_t &A = CPU8080[0];
  // uint8_t &Flag = CPU8080[1];
  // uint16_t &BC = ((uint16_t *)(CPU8080))[1];
  // uint8_t &B = CPU8080[2];
  // uint8_t &C = CPU8080[3];
  // uint16_t &DE = ((uint16_t *)(CPU8080))[2];
  // uint8_t &D = CPU8080[4];
  // uint8_t &E = CPU8080[5];
  // uint16_t &HL = ((uint16_t *)(CPU8080))[3];
  // uint8_t &H = CPU8080[6];
  // uint8_t &L = CPU8080[7];
  // uint16_t &SP = ((uint16_t *)(CPU8080))[4];
  // uint16_t &PC = ((uint16_t *)(CPU8080))[5];
  //
  // switch (buffer[pc++]) {
  //   case 0x01:
  //     C = buffer[pc++];
  //     B = buffer[pc++];
  //     break;
  //   case 0x02:
  //     BC = A;
  //     break;
  //   case 0x03:
  //     BC++;
  //     break;
  //   case 0x04:
  //     B++;
  //     break;
  //   case 0x05:
  //     B--;
  //     break;
  //   case 0x06:
  //     B = buffer[pc++];
  //     break;
  //   case 0x07:
  //     uint8_t temp = A;
  //     A = temp << 1 | temp >> 7;
  //   case 0x00:
  //   default:;
  // }
  return pc;
}

