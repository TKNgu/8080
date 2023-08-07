#include "Opcode.hpp"

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

