#ifndef INSTRUCTION_H
#define INSTRUCTION_H

void executeAdd(uint16_t addInstruction);

void executeAnd(uint16_t andInstruction);

void executeBranch(uint16_t branchInstruction);

void executeJump(uint16_t jumpInstruction);

void executeJumpToSubroutine(uint16_t jumpInstruction);

void executeLoad(uint16_t loadInstruction);

void executeLoadIndirect(uint16_t ldiInstruction);

void executeLoadRegister(uint16_t ldrInstruction);

void executeLoadEffectiveAddress(uint16_t leaInstruction);

#endif
