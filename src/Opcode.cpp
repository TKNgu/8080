#include "Opcode.hpp"

#include <cstring>
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

unsigned ShowOpcode(Opcode &opcode, uint8_t *buffer, unsigned pc) {
  cout << "0x" << hex << (int)(opcode.code) << '\t' << opcode.instruction
       << "\t\t0x";
  if (opcode.size == 2) {
    cout << hex << (int)(buffer[pc + 1]);
  } else if (opcode.size == 3) {
    cout << hex << (int)(buffer[pc + 1]) << "\t0x" << (int)(buffer[pc + 2]);
  }
  cout << endl;
  pc += opcode.size;
  if (!opcode.size) {
    pc++;
  }
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

void ArithFlags(uint8_t &Flag, uint8_t res) {
  Flag &= (res |= 0x01111111);
  Flag &= res ? 0x10111111 : 0xff;

  state->cc.z = ((res & 0xff) == 0);
  state->cc.s = (0x80 == (res & 0x80));  // res & 1000 0000
  state->cc.p = parity(res & 0xff, 8);
  state->cc.ac = (res > 0x09);  // checks if result is greater than 9
}

unsigned Run(uint8_t buffer[], unsigned pc) {
  static uint8_t CPU8080[12];
  memset(CPU8080, 0x00, 12);
  uint8_t &A = CPU8080[0];
  uint8_t &Flag = CPU8080[1];
  uint16_t &BC = ((uint16_t *)(CPU8080))[1];
  uint8_t &B = CPU8080[2];
  uint8_t &C = CPU8080[3];
  uint16_t &DE = ((uint16_t *)(CPU8080))[2];
  uint8_t &D = CPU8080[4];
  uint8_t &E = CPU8080[5];
  uint16_t &HL = ((uint16_t *)(CPU8080))[3];
  uint8_t &H = CPU8080[6];
  uint8_t &L = CPU8080[7];
  uint16_t &SP = ((uint16_t *)(CPU8080))[4];
  uint16_t &PC = ((uint16_t *)(CPU8080))[5];

  switch (buffer[pc++]) {
    case 0x01:
      C = buffer[pc++];
      B = buffer[pc++];
      break;
    case 0x02:
      BC = A;
      break;
    case 0x03:
      BC++;
      break;
    case 0x04:
      B++;
      break;
    case 0x05:
      B--;
      break;
    case 0x06:
      B = buffer[pc++];
      break;
    case 0x07:
      uint8_t temp = A;
      A = temp << 1 | temp >> 7;
    case 0x00:
    default:;
  }
  return pc;
}

