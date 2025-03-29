#include "architecture.h"
#include "instruction.h"

// Sign extending an integer x of length `bitCount` to 16 bits.
uint16_t signExtend(uint16_t x, int bitCount)
{
  // Basically we cut off all the bits except the most significant and then
  // and that with 1. So this returns true if the most significant bit is 1
  // meaning the number is negative using 2's complement
  if ((x >> (bitCount - 1)) & 1)
  {
    // 0xFFFF is 16 1's because F is 15 in hexadecimal which is 1111 in binary.
    // Then we shift it by `bitCount` so the least significant `bitCount` bits
    // will be 0 and we or that with x, so essentially padding x with 1s to
    // 16 bits.
    x |= (0xFFFF << bitCount);
  }
  return x;
}

void updateConditionFlags(uint16_t registerIdx)
{
  if (regs[registerIdx] == 0)
  {
    regs[R_COND] = FL_ZRO;
  }
  // Removes the 15 most significant bits, leaving only the most significant
  // bit. If this is 1 then it is a negative number using 2's complement.
  else if (regs[registerIdx] >> 15)
  {
    regs[R_COND] = FL_NEG;
  }
  else
  {
    regs[R_COND] = FL_POS;
  }
}

/*
 * There are 2 forms of the add instruction:
 *
 * 15   12 11 9 8   6  5  4  3 2    0
 * | 0001 | DR | SR1 | 0 | 00 | SR2 |
 *
 * 15   12 11 9 8   6  5  4         0
 * | 0001 | DR | SR1 | 1 | imm5     |
 *
 * The first 3 components of both are the same:
 * - the opcode 0001 is stored in bits 15-12
 * - the destination register (DR) is in bits 11-9
 * - the register storing the first operand (SR1) in bits 8-6
 *
 * Bit 5 being 0 indicates we are in the first case
 * - bits 4 and 3 are unused
 * - bits 2-0 store the register with the second operand (SR2)
 *
 * Bit 5 being 1 indicates we are in the second case
 * - then bits 4-0 store a 5 bit value that is added to the first operand, this
 *   value must be sign extended to 15 bits
 */
void executeAdd(uint16_t addInstruction)
{
  // the destination register (DR) is in in bits 11-9, so we remove all bits
  // less significant than the 9th bit, then bitwise-and with 7 which is 111
  // in binary to only get bits 11-9.
  uint16_t dr = (addInstruction >> 9) & 0x7;
  // the first register (SR1) is in in bits 8-6, so we remove all bits
  // less significant than the 6th bit, then bitwise-and with 7 which is 111
  // in binary to only get bits 8-6.
  unit16_t sr1 = (addInstruction >> 6) & 0x7;
  // the mode flag is in in bit 5, so we remove all bits less significant than
  // the 5th bit, then bitwise-and with 1 which is 1 in binary to only get bit
  // 5.
  uint16_t modeFlag = (addInstruction >> 5) & 1;

  if (modeFlag)
  {
    // we are in the second case, so sign extend the 5 bit immediate value
    // this is in bits 4-0 so we can bitwise-and with 31 (0x1F) which is 11111
    // in binary to only get the 5 most insignificant bits.
    uint16_t imm5 = signExtend(addInstruction & 0x1F, 5);
    regs[dr] = regs[sr1] + imm5;
  }
  else
  {
    // We are in case 1, so the second register is in bits 2-0 which we can
    // extract via bitwise and with 7, which is 111 in binary
    uint16_t sr2 = addInstruction & 0x7;
    regs[dr] = regs[sr1] + regs[sr2];
  }
  updateConditionFlags(dr);
}
