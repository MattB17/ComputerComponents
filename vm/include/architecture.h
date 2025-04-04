#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdlib.h>
#include <stdio.h>

// The default starting position for the program counter (PC)
#define PC_START = 0x3000

// 16-bit machine, each memory location stores a 16-bit value
// 2^16 = 65536 memory locations
#define MEMORY_MAX (1 << 16)
uint16_t mem[MEMORY_MAX];

// Our LC-3 architecture will have 10 registers:
// - 8 general purpose registers (R_0 - R_7)
// - a program counter (R_PC)
// - and a register for condition flags (R_COND)
enum Register
{
  R_0 = 0,
  R_1,
  R_2,
  R_3,
  R_4,
  R_5,
  R_6,
  R_7,
  R_PC,
  R_COND,
  R_MAX
};
uint16_t regs[R_MAX];

// The R_COND register stores condition flags providing info about
// the most recently executed calculation.
// LC-3 only 3 condition flags, indicating whether the previous calculation
// was positive, zero, or negative.
enum Condition
{
  FL_POS = 1 << 0,  // Positive
  FL_ZRO = 1 << 1,  // Zero
  FL_NEG = 1 << 2   // Negative
};

// Our instructions will be 16-bits consiting of an opcode followed by
// parameters.
// There are 16 opcodes so these will represent the first 4 bits of the
// instruction.
enum OpCode
{
  OP_BR = 0,  // Branch
  OP_ADD,     // Addition
  OP_LD,      // load
  OP_ST,      // store
  OP_JSR,     // jump register
  OP_AND,     // bitwise AND
  OP_LDR,     // load register
  OP_STR,     // store register
  OP_RTI,     // unused
  OP_NOT,     // bitwise not
  OP_LDI,     // load indirect
  OP_STI,     // store indirect
  OP_JMP,     // jump
  OP_RES,     // reserved (unused)
  OP_LEA,     // load effective address
  OP_TRAP     // execute trap
};

// The trap routines. Note they range from 0x20 (00100000) to 0x25 (00100101)
enum TrapCode
{
  TRAP_GETC = 0x20,   // Get character from keyboard, not echoed onto the terminal
  TRAP_OUT = 0x21,    // Output a character
  TRAP_PUTS = 0x22,   // Output a word string
  TRAP_IN = 0x23,     // Get character from keyboard, echoed onto the terminal
  TRAP_PUTSP = 0x24,  // Output a byte string
  TRAP_HALT = 0x25    // Halt the program
};

#endif
