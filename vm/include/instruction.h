#ifndef INSTRUCTION_H
#define INSTRUCTION_H

void executeAdd(uint16_t addInstruction);

void executeLoadIndirect(uint16_t ldiInstruction);

void executeAnd(uint16_t andInstruction);

void executeBranch(uint16_t branchInstruction);

#endif
