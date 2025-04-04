// A set of methods to execute 16-bit machine instructions.
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

void executeAdd(uint16_t addInstruction);

void executeAnd(uint16_t andInstruction);

void executeNot(uint16_t notInstruction);

void executeBranch(uint16_t branchInstruction);

void executeJump(uint16_t jumpInstruction);

void executeJumpToSubroutine(uint16_t jumpInstruction);

void executeLoad(uint16_t loadInstruction);

void executeLoadIndirect(uint16_t ldiInstruction);

void executeLoadRegister(uint16_t ldrInstruction);

void executeLoadEffectiveAddress(uint16_t leaInstruction);

void executeStore(uint16_t storeInstruction);

void executeStoreIndirect(uint16_t stiInstruction);

void executeStoreRegister(uint16_t strInstruction);

#endif
