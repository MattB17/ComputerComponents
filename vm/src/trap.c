#include "architecture.h"
#include "instruction.h"
#include "trap.h"

/* the PUTS trap routine outputs a null-terminated string
 * the starting address of the string is stored in R0 and the string continues
 * until a null character is encountered (0x0000).
 * Note: each memory address stores a single character.
 */
void trapPuts()
{
  // take the offset from the memory array mem specified in R0 to get a pointer
  // to the first character.
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
 * The trap OUT routine writes a character in R0[7:0] to the console.
 */
void trapOut()
{
  // Take the character in R0 and we convert it to a char (which is 8 bits).
  // Then we place it on the stdout buffer and flush the buffer.
  putc((char)(regs[R_0] & 0xFF), stdout);
  fflush(stdout);
}

/*
 * The trap GETC routine fetches a single character from the keyboard and
 * places that character into R0.
 */
void trapGetc()
{
  // We retrieve a char from the keyboard with `getchar` and convert it into
  // 16 bits to store in R_0.
  regs[R_0] = (uint16_t)getchar();
  updateConditionFlags(R_0);
}

/*
 * The trap IN routine prompts on the screen for a character. It reads in that
 * character, echoes it to the screen and stores it into R0.
 */
void trapIn() {
  printf("Enter a single character: ");
  char c = getchar();
  putc(c, stdout);
  fflush(stdout);
  regs[R_0] = (uint16_t)c;
  updateConditionFlags(R_0);
}

/*
 * The trap PUTSP routine reads a string from memory starting at R0 until a
 * null character is encountered and writes that string to the console.
 *
 * Each location in memory contains 2 characters. The first is in bits [7:0]
 * and the second is in bits [15:8]
 */
void trapPutsp()
{
  // take the offset from the memory array mem specified in R0 to get a pointer
  // to the first character.
  uint16_t* c = mem + regs[R_0];

  // Loop while we haven't encountered a null character.
  while (*c) {
    uint16_t currChar = *c;
    // We first write the lower  bits.
    putc((char)(currChar & 0xFF), stdout);
    // Now take of the lower 8 bits and write those, unless we have encountered
    // the null character.
    currChar = currChar >> 8;
    if (currChar) {
      putc((char) currChar, stdout);
      c++
    } else {
      break;
    }
  }

  fflush(stdout);
}

/*
 * the trap routine HALT stops the program and prints a message to the console.
 */
void halt()
{
  puts("Execution halted");
  fflush(stdout);
  RUNNING = 0;
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
      trapGetc();
      break;
    case TRAP_OUT:
      trapOut();
      break;
    case TRAP_PUTS:
      trapPuts();
      break;
    case TRAP_IN:
      trapIn();
      break;
    case TRAP_PUTSP:
      trapPutsp();
      break;
    case TRAP_HALT:
      halt();
      break;
  }
}
