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

// 0x1FF is 111111111 in binary. So we are extracting the pc offset in the
// 9 least significant bits and sign extending it to 16 bits.
uint16_t signExtendPcOffset(uint16_t &currInstruction)
{
  return signExtend(currInstruction & 0x1FF, 9);
}

// the index of a register is 3 bits (registers 0 to 7). So we specify the
// end index of the register in the instruction and trim off all bits after
// that, so now the 3 least significant bits of the result indexes the register
// then we take the bitwise-and with 7 (111), which extracts these last 3 bits.
uint16_t extractRegister(uint16_t &currInstruction, short endIdx)
{
  return (currInstruction >> endIdx) & 0x7;
}

// To extract the bit at `bitIdx` we first remove all bits less significant
// than `bitIdx` and then bitwise-and with 1 to get just that bit.
uint16_t extractBit(uint16_t &currInstruction, short bitIdx)
{
  return (currInstruction >> bitIdx) & 1;
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
 * In this case we add SR1 and SR2 and store in DR.
 *
 * Bit 5 being 1 indicates we are in the second case
 * - then bits 4-0 store a 5 bit value that is added to the first operand, this
 *   value must be sign extended to 16 bits
 */
void executeAdd(uint16_t addInstruction)
{
  uint16_t dr = extractRegister(addInstruction, 9);
  unit16_t sr1 = extractRegister(addInstruction, 6);
  uint16_t modeFlag = extractBit(addInstruction, 5);

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
    // We are in case 1, so the second register is in bits 2-0
    uint16_t sr2 = extractRegister(addInstruction, 0);
    regs[dr] = regs[sr1] + regs[sr2];
  }
  updateConditionFlags(dr);
}

/*
 * There are 2 forms of the and instruction
 *
 *  15  12 11 9 8   6  5  4  3 2    0
 * | 0101 | DR | SR1 | 0 | 00 | SR2 |
 *  15  12 11 9 8   6  5  4         0
 * | 0001 | DR | SR1 | 1 | imm5     |
 *
 * The first 3 components of both are the same:
 * - the opcode 0101 is stored in bits 15-12
 * - the destination register (DR) is in bits 11-9
 * - the register storing the first operand (SR1) in bits 8-6
 *
 * Bit 5 being 0 indicates we are in the first case
 * - bits 4 and 3 are unused
 * - bits 2-0 store the register with the second operand (SR2)
 * In this case we take the bitwise and of SR1 and SR2 and store in DR.
 *
 * Bit 5 being 1 indicates we are in the second case
 * - then bits 4-0 store a 5 bit value that is anded with the first operand,
 *   this value must be sign extended to 16 bits
 */
void executeAnd(uint16_t andInstruction)
{
  uint16_t dr = extractRegister(andInstruction, 9);
  unit16_t sr1 = extractRegister(andInstruction, 6);
  uint16_t modeFlag = extractBit(andInstruction, 5);

  if (modeFlag)
  {
    // we are in the second case, so sign extend the 5 bit immediate value
    // this is in bits 4-0 so we can bitwise-and with 31 (0x1F) which is 11111
    // in binary to only get the 5 most insignificant bits.
    uint16_t imm5 = signExtend(andInstruction & 0x1F, 5);
    regs[dr] = regs[sr1] & imm5;
  }
  else
  {
    // We are in case 1, so the second register is in bits 2-0
    uint16_t sr2 = extractRegister(andInstruction, 0);
    regs[dr] = regs[sr1] & regs[sr2];
  }
  updateConditionFlags(dr);
}

/*
 * The branch instruction has the following form:
 *
 *  15  12  11  10  9  8         0
 * | 0000 | n | z | p | PCoffset9 |
 *
 * Bits 15-12 hold the branch opcode (0000)
 * Bits 11, 10 and 9 hold the branch condition flags, we branch if any of the
 * following are true:
 * - the n bit (index 11) is set and the COND register is FL_NEG
 * - the z bit (index 10) is set and the COND register is FL_ZRO
 * - the p bit (index 9) is set and the COND register is FL_POS
 *
 * If any of the branch conditions are true we add the 9-bit PCoffset9 to the
 * incremented program counter after sign extending to 16 bits
 */
void executeBranch(uint16_t branchInstruction)
{
  uint16_t nBit = extractBit(branchInstruction, 11);
  uint16_t zBit = extractBit(branchInstruction, 10);
  uint16_t pBit = extractBit(branchInstruction, 9);

  uint16_t branchBit = nBit & (regs[R_COND] == FL_NEG);
  branchBit |= zBit & (regs[R_COND] == FL_ZRO);
  branchBit |= pBit & (regs[R_COND] == FL_POS);

  if (branchBit)
  {
    uint16_t pcOffset = signExtendPcOffset(branchInstruction);
    regs[R_PC] += pcOffset;
  }
}

/*
 * The jump instruction has the form:
 *
 *  15  12 11  9 8     6 5      0
 * | 1100 | 000 | BaseR | 000000 |
 *
 * The opcode 1100 is stored in bits 1100
 * Bits 11 to 9 and 5 to 0 are unused.
 * The register holding the address to jump to is BaseR in bits 8 to 6.
 *
 * This is an unconditional jump.
 *
 * NOTE: function return is just a jump command with BaseR set to 111.
 */
void executeJump(uint16_t jumpInstruction)
{
  uint16_t baseR = extractRegister(jumpInstruction, 6);
  regs[R_PC] = regs[baseR];
}

/*
 * This method passes control to a subroutine.
 *
 * We first save the program counter to R7 so we can link back to the calling
 * subroutine.
 *
 * Then we need to load the program counter with the first address of the first
 * instruction in the called subroutine. There are 2 forms of this command:
 *
 *  15  12  11 10         0
 * | 0100 | 1 | PCoffset11 |
 *
 *  15  12  11 10 9 8     6 5      0
 * | 0100 | 0 | 00 | BaseR | 000000 |
 *
 * In both cases bits 15-12 store the opcode
 *
 * If bit 11 is 1 then we are in the first case and we sign extend the offset
 * in bits 10-0 to 16 bits and add it to the incremented program counter.
 *
 * If instead bit 11 is 0 then we extract the base register from bits 8-6 and
 * set the program counter to the address in this register. All other bits are
 * unused.
 */
void executeJumpToSubroutine(uint16_t jumpInstruction)
{
  // Save current program counter in R7 (this PC points to current instruction
  // in the calling subroutine so we can come back here after).
  regs[R_7] = regs[PC];

  uint16_t modeBit = extractBit(jumpInstruction, 11);

  if (modeBit)
  {
    uint16_t pcOffset = signExtend(jumpInstruction & 0x7FF, 11);
    regs[PC] += pcOffset;
  }
  else
  {
    uint16_t baseR = extractRegister(jumpInstruction, 6);
    regs[PC] = regs[baseR];
  }
}

/*
 * Load Indirect Instruction
 *
 * Used to load a value from a location in memory into a register.
 * Load Indirect Instructions are off the form:
 *
 *  15  12 11 9 8          0
 * | 1010 | DR | PCoffset9 |
 *
 * Bits 15-12 store the opcode 1010
 * Bits 11 to 9 store the destination register
 * The remaining 9 bits store the program counter offset
 * - this number needs to be extended to 16 bits
 * - then added to the increment program counter (PC) to get a memory
 *   address from which to read from
 */
void executeLoadIndirect(uint16_t ldiInstruction)
{
  uint16_t dr = extractRegister(ldiInstruction, 9);
  uint16_t pcOffset = signExtendPcOffset(ldiInstruction);

  // The program counter is incremented when we fetch the instruction so we
  // can just add the offset to the current program counter and read the data
  // stored at that memory location (the first mem_read). That data is an
  // address pointing to where the actual data is stored, so we read that
  // address (the second mem_read) and load the value into the destination
  // register (dr).
  regs[dr] = mem_read(mem_read(regs[R_PC] + pcOffset));

  updateConditionFlags(dr);
}

/*
 * The Load instruction has the following form:
 *
 *  15  12 11 9 8          0
 * | 0010 | DR | PCoffset9 |
 *
 * Bits 15-12 store the opcode.
 * Bits 11-9 store the destination register
 *
 * The PC offset stored in bits 8-0 is sign extended to 16 bits and added to
 * the program counter (PC) to get an address that is read from memory and
 * stored in the destination register.
 */
void executeLoad(uint16_t loadInstruction)
{
  uint16_t dr = extractRegister(loadInstruction, 9);
  uint16_t pcOffset = signExtend(loadInstruction & 0x1FF, 9);

  // We read from the memory at the address specified by the program counter
  // plus the offset and store that in the destination register.
  regs[dr] = mem_read(regs[R_PC] + pcOffset);

  updateConditionFlags(dr);
}
