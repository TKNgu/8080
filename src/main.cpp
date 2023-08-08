#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "Opcode.hpp"

using namespace std;

#define FILE_ROOM "data/SpaceInvaders.gb"
#define FILE_OPCODE "data/opcode.csv"

int main(int argc, char *argv[]) {
  cout << "Hello 8080" << endl;

  ifstream romFile;
  if (argc == 1) {
    romFile = ifstream(FILE_ROOM, ios::in | ios::binary);
  } else {
    romFile = ifstream(string(argv[1]), ios::in | ios::binary);
    if (!romFile.is_open()) {
      cout << "Error open file " << argv[1] << endl;
      return 1;
    }
  }
  romFile.seekg(0, ios::end);
  auto bufferSize = romFile.tellg();
  romFile.seekg(0, ios::beg);
  auto *buffer = new uint8_t[bufferSize];
  romFile.read((char *)(buffer), bufferSize);

  Opcode *opcodes;
  ifstream opcodeFile(FILE_OPCODE, ios::in);
  auto opcodeSize = InitOpcode(opcodes, opcodeFile);

  unsigned pc = 0;
  while (pc < bufferSize) {
    uint8_t opIndex = buffer[pc];
    Opcode &opcode = opcodes[opIndex];
    pc = Run(buffer, pc);
  }

  delete[] buffer;
  delete[] opcodes;

  return 0;
}
