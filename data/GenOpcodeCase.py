import json

print("Hello Python GenOpcode")

for item in [["0x06", "B"], ["0x0e", "C"], ["0x16", "D"],
             ["0x1e", "E"], ["0x26", "H"], ["0x2e", "L"]]:
    print("instructions[{}] = LD_NN_N({});".format(item[0], item[1]))

print()
for item in [[0x78, "A"], [0x40, "B"], [0x48, "C"], [0x50, "D"], [0x58, "E"],
             [0x60, "H"], [0x68, "L"]]:
    code = item[0];
    for register in ["B", "C", "D", "E", "H", "L"]:
        print("instructions[{}] = LD_R1_R2({}, {});".format(hex(code), item[1], register))
        code += 1
    print("instructions[{}] = LD_R1_R2M({}, HL);".format(hex(code), item[1]))
    code += 1
    print("instructions[{}] = LD_R1_R2({}, A);".format(hex(code), item[1]))
    print()

code = 0x70
for register in ["B", "C", "D", "E", "H", "L"]:
    print("instructions[{}] = LD_R1M_R2(HL, {});".format(hex(code), register))
    code += 1
print("instructions[0x36] = LD_R1M_N(HL);")

print()
print("instructions[0x0a] = LD_R1_R2M(A, BC);")
print("instructions[0x1a] = LD_R1_R2M(A, DE);")
print("instructions[0x7e] = LD_R1_R2M(A, HL);")
print("instructions[0xfa] = LD_R1_NM(A);")

print("instructions[0x02] = LD_R1M_R2(BC, A);")
print("instructions[0x12] = LD_R1M_R2(DE, A);")
print("instructions[0x77] = LD_R1M_R2(HL, A);")
print("instructions[0xea] = LD_NM_R2(A);")
