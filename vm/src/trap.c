#include "architecture.h"
#include "trap.h"

/* the PUTS trap routine outputs a null-terminated string
 * the starting address of the string is stored in R0 and the string continues
 * until a null character is encountered (0x0000).
 * Note: each memory address stores a single character.
 */
void trapPuts()
{
  // take the offset from the memory specified in R0 to get a pointer to the
  // first character.
  uint16_t* c = mem + regs[R_0];

  // Loop while we haven't encountered a null character.
  while (*c)
  {
    // We convert to a C char and place it in stdout, then move to the next
    // character.
    putc((char)*c, stdout);
    c++;
  }

  // Force a write of the string we just put into the stdout stream.
  fflush(stdout);
}

/*
 * The trap instruction has the following form:
 *
 *  15  12 11   8 7         0
 * | 1111 | 0000 | trapvect8 |
 *
 * The fields are as follows:
 * - bits 15-12 are the opcode identifying the trap instruction
 * - bits 11-8 are unused
 * - bits 7-0 identify the trap code specifying what trap routine should
 *   be executed.
 */
void handleTrap(uint16_t trapInstruction)
{
  // Store the current value of the program counter in R_7 before jumping to
  // the trap routine so we can load this value again when we execute the
  // trap routine.
  regs[R_7] = regs[R_PC];

  // We take bitwise-and with 0xFF which is 11111111 to get the 8 least
  // significant bits corresponding to the trap routine.
  switch (trapInstruction & 0xFF)
  {
    case TRAP_GETC:
      // trap getc routine
      break;
    case TRAP_OUT:
      // trap out routine
      break;
    case TRAP_PUTS:
      trapPuts();
      break;
    case TRAP_IN:
      // trap in routine
      break;
    case TRAP_PUTSP:
      // trap putsp routine
      break;
    case TRAP_HALT:
      // trap halt routine
      break;
  }
}
