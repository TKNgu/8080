#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "Opcode.hpp"

using namespace std;

#define FILE_ROOM "data/SpaceInvaders.gb"
#define FILE_OPCODE "data/opcode.csv"
#define BUFFER_SIZE 1024

int main(int, char *[]) {
  cout << "Hello 8080" << endl;

  Opcode *opcodes;
  ifstream opcodeFile(FILE_OPCODE, ios::in);
  auto opcodeSize = InitOpcode(opcodes, opcodeFile);

  ifstream romFile(FILE_ROOM, ios::in | ios::binary);
  char buffer[BUFFER_SIZE];
  while (!romFile.eof()) {
    romFile.read(buffer, BUFFER_SIZE);
    unsigned bufferSize = romFile.gcount();
    for (auto index = 0u; index < bufferSize; index++) {
      cout << hex << (int)(buffer[index]) << " ";
      if (index == 10) {
        goto EXIT;
      }
    }
  }

EXIT:
  delete[] opcodes;

  return 0;
}

