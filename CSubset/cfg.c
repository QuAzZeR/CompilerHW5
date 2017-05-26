#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "csg.h"




typedef struct BlockDesc *Block; 

typedef struct BlockDesc {
  Block fail, branch; // jump targets
  CSGNode first, last; // pointer to first and last instruction in basic block
    
  int blockId;
} BlockDesc;

// typedef struct node leader;
typedef struct leader{
  int id;
  CSGNode node;
  struct leader *next;
}leader;

int blockCount = 1;
leader *headLeader = NULL;
int leaderCount = 0;

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

void collectLeader(int line, CSGNode node)
{
  leader *ptt = headLeader; 
  if (ptt == NULL)
  {
    headLeader = malloc(sizeof(leader));
    headLeader->id = line;    
    headLeader->node = node;
    headLeader->next = NULL;
    leaderCount++;
    // printf(">>>>>>%d %d %d\n",ptt->id,line,leaderCount);
  }
  else{
    while(1){
      // printf("%d %d",ptt->id,line);
      if (ptt->id == line)
        return;
      if (ptt->next == NULL)
        break;
      ptt = ptt->next;
    }
    leader *tmp;
    tmp = malloc(sizeof(leader));
    tmp->id = line;
    tmp->node = node;
    tmp->next = NULL;
    ptt->next = tmp;
    leaderCount++;
    // printf(">>%d %d %d\n",ptt->id,line,leaderCount);
  }
  // printf("EIEI\n");
  

}

Block createBlock(CSGNode node, int BlockId ){
  Block block = malloc(sizeof(struct BlockDesc));
  block->first = node;
  block->last = NULL;
  block->fail = NULL;
  block->branch = NULL;
  block->blockId = BlockId;
  return block;
}
Block findBlock(Block *block, int line){
  int j = 0;
  for (j = 0 ; j < leaderCount ; j++){
    if (line == block[j]->first->line)
      return block[j];
  }
  return NULL;
}

void getTemporaryVariable(CSGNode x){
  // if (x == GP) {
    
  // } else if (x == FP) {
  if(1){   
  } else {
    switch (x->class) {
      case CSGVar: if ((x->type == CSGlongType) && (x->lev == 1)) printf(" %s", x->name); else printf(" %s_base", x->name); break;
      case CSGConst: printf(" %lld", x->val); break;
      case CSGFld: printf(" %s_offset", x->name); break;
      case CSGInst: case CSGAddr: printf(" (%d)", x->line); break;
      case CSGProc: printf(" [%d]", x->true->line); break;
      default: printf("unknown class");
    }
  }
}

void calculateLiveness(Block *block){
  register CSGNode i;
  i = code;
  while (i != NULL) {
    printf("    instr %d: ", i->line);
    
    switch (i->op) {
      case iadd: printf("add"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case isub: printf("sub"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case imul: printf("mul"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case idiv: printf("div"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case imod: printf("mod"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case ineg: printf("neg"); getTemporaryVariable(i->x); break;

      case iparam: printf("param"); getTemporaryVariable(i->x); break;
      case ienter: printf("enter"); break;
      case ileave: printf("leave"); break;
      case iret: printf("ret"); break;
      case iend: printf("end"); break;

      case icall: printf("call"); getTemporaryVariable(i->x); break;
      case ibr: printf("br"); PrintBrakNode(i->x); break;

      case iblbc: printf("blbc"); getTemporaryVariable(i->x); PrintBrakNode(i->y); break;
      case iblbs: printf("blbs"); getTemporaryVariable(i->x); PrintBrakNode(i->y); break;

      case icmpeq: printf("cmpeq"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case icmple: printf("cmple"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case icmplt: printf("cmplt"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;

      case iread: printf("read"); break;
      case iwrite: printf("write"); getTemporaryVariable(i->x); break;
      case iwrl: printf("wrl"); break;

      case iload: printf("load"); PrintNode(i->x); break;
      case istore: printf("store"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;
      case imove: printf("move"); getTemporaryVariable(i->x); getTemporaryVariable(i->y); break;

      case inop: printf("nop"); break;
      default: printf("unknown instruction");
    }
    printf("\n");
      
    i = i->nxt;
      
  }
}

void printCFG(Block *block){
  CSGNode i;
  i = code;
  int j = 0;
  for (j = 0 ; j < leaderCount ; j++)
  {
    // printf("--------------\n");
    printf("*** block %d ", block[j]->blockId);
    printf("fail "); if(block[j]->fail) printf("%d", block[j]->fail->blockId); else printf("-");
    printf(" branch "); if(block[j]->branch) printf("%d", block[j]->branch->blockId); else printf("-");
    printf(" first "); if(block[j]->first) printf("%d", block[j]->first->line); else printf("-");
    printf(" last "); if(block[j]->last) printf("%d", block[j]->last->line); else printf("-");
    printf("\n");
    i = block[j]->first;
    // continue;
    // printf("%d\n %d %d\n",j,block[j]->first->line,block[j]->last->line);
    while (1) {
      printf("    instr %d: ", i->line);
      switch (i->op) {
        case iadd: printf("add"); PrintNode(i->x); PrintNode(i->y); break;
        case isub: printf("sub"); PrintNode(i->x); PrintNode(i->y); break;
        case imul: printf("mul"); PrintNode(i->x); PrintNode(i->y); break;
        case idiv: printf("div"); PrintNode(i->x); PrintNode(i->y); break;
        case imod: printf("mod"); PrintNode(i->x); PrintNode(i->y); break;
        case ineg: printf("neg"); PrintNode(i->x); break;

        case iparam: printf("param"); PrintNode(i->x); break;
        case ienter: printf("enter"); break;
        case ileave: printf("leave"); break;
        case iret: printf("ret"); break;
        case iend: printf("end"); break;

        case icall: printf("call"); PrintNode(i->x); break;
        case ibr: printf("br"); PrintBrakNode(i->x); break;

        case iblbc: printf("blbc"); PrintNode(i->x); PrintBrakNode(i->y); break;
        case iblbs: printf("blbs"); PrintNode(i->x); PrintBrakNode(i->y); break;

        case icmpeq: printf("cmpeq"); PrintNode(i->x); PrintNode(i->y); break;
        case icmple: printf("cmple"); PrintNode(i->x); PrintNode(i->y); break;
        case icmplt: printf("cmplt"); PrintNode(i->x); PrintNode(i->y); break;

        case iread: printf("read"); break;
        case iwrite: printf("write"); PrintNode(i->x); break;
        case iwrl: printf("wrl"); break;

        case iload: printf("load"); PrintNode(i->x); break;
        case istore: printf("store"); PrintNode(i->x); PrintNode(i->y); break;
        case imove: printf("move"); PrintNode(i->x); PrintNode(i->y); break;

        case inop: printf("nop"); break;
        default: printf("unknown instruction");
      }
      printf("\n");
      if (i == block[j]->last)
        break;
      i = i->nxt;
      
    }
  }
}

Block* generateCFG(){
  register CSGNode i;
  register int cnt;
  Block *block;
  Block tmp;
  cnt = 1;
  i = code;
  while (i != NULL){
    i->line = cnt;
    cnt++;
    i = i->nxt;
  }
  i= code;
  while (i != NULL){
    switch(i->op){
      case ienter: collectLeader(i->line,i); break;
      case iret: break;
      case iend: break;
      case iblbc: collectLeader(i->y->line,i->y); collectLeader(i->nxt->line,i->nxt); break;
      case iblbs: collectLeader(i->y->line,i->y); collectLeader(i->nxt->line,i->nxt); break;
      case ibr: collectLeader(i->x->line,i->x); collectLeader(i->nxt->line,i->nxt); break;
      default: break;
    }
    i = i->nxt;  
    
  }
  leader *ptt = headLeader;

  
  int *arrayLeader = malloc(sizeof(int) * leaderCount);
  int j = 0;
  for (leader *it = headLeader; j < leaderCount  ;j++, it = it->next){
    arrayLeader[j] = it->id;
  
  }
  
  qsort(arrayLeader, leaderCount, sizeof(int),cmpfunc);
  
  block = malloc(sizeof(*block) * leaderCount);
  for (j = 0 ; j < leaderCount ; j++){
    ptt = headLeader;
    while(1){
      if (ptt->id == arrayLeader[j]){
        break;
      }
      if(ptt->next == NULL)
        break;
      ptt = ptt->next;
    }
    block[j] = createBlock(ptt->node,j+1);
    
  }
  
  i = code;
  j = 1;
  while (i != NULL){
    switch(i->op){
      case iblbc:
      case iblbs:
        if (i->line == arrayLeader[j]-1){
          tmp = findBlock(block,i->y->line);
          assert(tmp != NULL);
          block[j-1]->branch = tmp;
          assert(tmp != NULL); 
          tmp = findBlock(block,i->nxt->line);
          block[j-1]->fail = tmp;
          block[j-1]->last = i;
          j++;

        }
        break;
      case ibr:
        if (i->line == arrayLeader[j]-1){
          tmp = findBlock(block,i->x->line);
          // printf("%d %d\n",tmp->blockId,tmp->first->line);
          assert(tmp != NULL);
          block[j-1]->branch = tmp;
          block[j-1]->last = i;
          j++;
        }
        break;
      case iend:
      case iret:
        block[j-1]->last = i; 
        block[j-1]->branch = NULL;   
        j++;     
        break;
      default:
        if(i->nxt && i->line == arrayLeader[j]-1){
          tmp = findBlock(block,i->nxt->line);
          assert(tmp != NULL);
          block[j-1]->fail = tmp;
          block[j-1]->last = i;
          j++;
        }
        break;
    }
    // printf("%d %d %d-------->>>\n",i->line,arrayLeader[j]-1,i->op);
    
    i = i->nxt;
    // printCFG(block);  
  }
  

  return block;
}