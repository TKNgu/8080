#ifndef Opcode_hpp
#define Opcode_hpp

#include <cstdint>
#include <fstream>
#include <string>

struct Opcode {
  uint8_t code;
  std::string instruction;
  uint8_t size = 0;
  std::string flags;
  std::string function;
};

void ShowOpcode(Opcode &);
unsigned ShowOpcode(uint8_t *, unsigned);
unsigned Run(uint8_t[], unsigned);
unsigned InitOpcode(Opcode *&, std::ifstream &);

#endif
