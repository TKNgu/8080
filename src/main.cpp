#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

#define FILE_ROOM "SpaceInvaders.gb"
#define FILE_OPCODE "data/opcode.csv"
#define BUFFER_SIZE 1024

struct Opcode {
  uint8_t code;
  string instruction;
  uint8_t size;
  uint8_t flags;
  string function;
};

unsigned SplitNext(char *, unsigned);

int main(int, char *[]) {
  cout << "Hello 8080" << endl;

  ifstream opcodeFile(FILE_OPCODE, ios::in);
  char buffer[BUFFER_SIZE];
  opcodeFile.getline(buffer, BUFFER_SIZE);
  int commandSize = stoi(buffer);
  auto opcodes = new Opcode[commandSize];

  cout << "Command size " << commandSize << endl;
  for (int index = 0; index < commandSize || opcodeFile.eof(); index++) {
    opcodeFile.getline(buffer, BUFFER_SIZE);
    int startIndex = 0;
    int endIndex = SplitNext(buffer, startIndex);
    auto &opcode = opcodes[index];
    opcode.code = stoi(string(buffer, startIndex, endIndex), 0, 16);
    cout << hex << (int)(opcode.code) << endl;
  }

  delete[] opcodes;

  return 0;
}

unsigned SplitNext(char *buffer, unsigned start) {
  for (; buffer[start] != '\0' && buffer[start] != ','; start++) {
  }
  return start;
}
