/*
 * The main VM procedure is the following:
 * 1) load an instruction from memory at the address of the PC register
 * 2) Increment the PC register
 * 3) Look at the opcode to determine which type of instruction it should do
 * 4) Perform the instruction with the instructions parameters
 * 5) Loop back to step 1.
 */

#include "architecture.h"

int main(int argc, const char* argv[])
{
  // Load arguments
  // Setup

  // initially load the zero flag into the condition register
  regs[R_COND] = FL_ZRO;

  // Set the program counter to the starting position
  regs[R_PC] = PC_START;

  int running = 1;
  while (running == 1)
  {
    // Steps 1 and 2: fetch instruction pointed to by program counter
    // and then increment the program counter
    uint16_t instruction = mem_read(regs[R_PC]++);

    // Step 3: extract the opcode
    uint16_t opcode = instruction >> 12;

    // Step 4: implement action based on opcode
    switch (opcode)
    {
      case OP_ADD:
        // add instruction
        break;
      case OP_AND:
        // and instruction
        break;
      case OP_NOT:
        // not instruction
        break;
      case OP_BR:
        // branch instruction
        break;
      case OP_JMP:
        // jump instruction
        break;
      case OP_JSR:
        // jump register instruction
        break;
      case OP_LD:
        // load instruction
        break;
      case OP_LDI:
        // load indirect instruction
        break;
      case OP_LDR:
        // load register instruction
        break;
      case OP_LEA:
        // load effective address instruction
        break;
      case OP_ST:
        // store instruction
        break;
      case OP_STI:
        // store indirect instruction
        break;
      case OP_STR:
        // store register instruction
        break;
      case OP_TRAP:
        // trap instruction
        break:
      case OP_RES:
      case OP_RTI:
      default:
        // bad opcode
        break;
    }
  }

  // Shutdown VM
}
