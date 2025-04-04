/*
 * The main VM procedure is the following:
 * 1) load an instruction from memory at the address of the PC register
 * 2) Increment the PC register
 * 3) Look at the opcode to determine which type of instruction it should do
 * 4) Perform the instruction with the instructions parameters
 * 5) Loop back to step 1.
 */

#include "architecture.h"
#include "instruction.h"
#include "trap.h"

int main(int argc, const char* argv[])
{
  if (argc < 2)
  {
    // Show usage string
    printf("lc3 [image-file1] ...\n");
    exit(1);
  }

  // Check that the images can be read
  for (int idx = 1; idx < argc; idx++)
  {
    if (!read_image(argv[idx]))
    {
      printf("Failed to load image: %s\n", argv[idx]);
      exit(1);
    }
  }
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
    uint16_t currInstruction = mem_read(regs[R_PC]++);

    // Step 3: extract the opcode
    uint16_t opcode = currInstruction >> 12;

    // Step 4: implement action based on opcode
    switch (opcode)
    {
      case OP_ADD:
        executeAdd(currInstruction);
        break;
      case OP_AND:
        executeAnd(currInstruction);
        break;
      case OP_NOT:
        executeNot(currInstruction);
        break;
      case OP_BR:
        executeBranch(currInstruction);
        break;
      case OP_JMP:
        executeJump(currInstruction);
        break;
      case OP_JSR:
        executeJumpToSubroutine(currInstruction);
        break;
      case OP_LD:
        executeLoad(currInstruction);
        break;
      case OP_LDI:
        executeLoadIndirect(currInstruction);
        break;
      case OP_LDR:
        executeLoadRegister(currInstruction);
        break;
      case OP_LEA:
        executeLoadEffectiveAddress(currInstruction);
        break;
      case OP_ST:
        executeStore(currInstruction);
        break;
      case OP_STI:
        executeStoreIndirect(currInstruction);
        break;
      case OP_STR:
        executeStoreRegister(currInstruction);
        break;
      case OP_TRAP:
        handleTrap(currInstruction);
        break:
      case OP_RES:
      case OP_RTI:
      default:
        printf("Invalid opcode received: %d", opcode);
        exit(1);
    }
  }

  // Shutdown VM
}
