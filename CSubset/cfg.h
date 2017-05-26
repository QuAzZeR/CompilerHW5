#ifndef _CSubCFG_H_
#define _CSubCFG_H_

#include <stdio.h>
#include "csg.h"


typedef struct BlockDesc *Block; 

typedef struct BlockDesc {
  Block fail, branch; // jump targets
  CSGNode first, last; // pointer to first and last instruction in basic block
} BlockDesc;

// extern Block block;
extern Block* generateCFG(void);
extern void printCFG(Block*);
#endif
