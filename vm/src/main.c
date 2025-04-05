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

/*
 * Used to swap between big endian and little endian.
 *
 * LC3 programs are big endian but most computers are little endian.
 */
uint16_t swap16(uint16_t x)
{
  return (x << 8) | (x >> 8);
}

/*
 * Used to read a file representing the image to be run by the VM.
 */
void readImageFile(FILE* file)
{
  // Origin specifies where in memory to place the image
  uint16_t origin;
  // Read the first 16 bits from the file and place in origin
  fread(&origin, sizeof(origin), 1, file);
  // swap to little endian
  origin = swap16(origin);

  // Now we know to place the file starting at origin in our memory.
  // As memory has a finite size, we know the maximum size we can read from
  // the file.
  uint16_t maxRead = MEMORY_MAX - origin;
  // The starting address to write the program to.
  uint16_t *instructionPointer = memory + origin;
  // Now we read the entire program into memory (up to maxRead size) starting
  // from the instructionPointer.
  size_t instructionCount = fread(
    instructionPointer, sizeof(uint16_t), maxRead, file);

  // Lastly, for each instruction we need to swap from big endian to little
  // endian, so we loop through starting from the origin.
  while (instructionCount-- > 0)
  {
    *instructionPointer = swap16(*instructionPointer);
    instructionPointer++;
  }
}

/*
 * A convenient wrapper around readImageFile that accepts a file path.
 */
int readImage(const char* imagePath)
{
  FILE* file = fopen(imagePath, "rb");
  if (!file)
  {
    return 0;
  }
  readImageFile(file);
  fclose(file);
  return 1;
}

int main(int argc, const char* argv[])
{
  if (argc < 2)
  {
    // Show usage string
    printf("lc3 [image-file1] ...\n");
    exit(1);
  }

  // Check that the images can be read.
  for (int idx = 1; idx < argc; idx++)
  {
    // We read each image into memory, throwing an error if it can't be read.
    if (!readImage(argv[idx]))
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

  RUNNING = 1;
  while (RUNNING)
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
