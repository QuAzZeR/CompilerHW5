#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "csg.h"
#define MAX_LIVENESS 50

typedef struct BlockDesc *Block; 

typedef struct BlockDesc {
  int parentCount;
  int* parentList;
  Block fail, branch; // jump targets
  CSGNode first, last; // pointer to first and last instruction in basic block
  int blockId;
} BlockDesc;

typedef struct leader{
  int id;
  CSGNode node;
  struct leader *next;
} leader;

int blockCount = 1;
leader *headLeader = NULL;
int leaderCount = 0;

int cmpfunc (const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}

void collectLeader(int line, CSGNode node){
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

void printCFG(Block *block){
  CSGNode i;
  i = code;
  int j = 0;
  for (j = 0 ; j < leaderCount ; j++)
  {
    printf("*** block %d ", block[j]->blockId);
    printf("fail "); if(block[j]->fail) printf("%d", block[j]->fail->blockId); else printf("-");
    printf(" branch "); if(block[j]->branch) printf("%d", block[j]->branch->blockId); else printf("-");
    printf(" first "); if(block[j]->first) printf("%d", block[j]->first->line); else printf("-");
    printf(" last "); if(block[j]->last) printf("%d", block[j]->last->line); else printf("-");
    printf(", Parent Count: %d", block[j]->parentCount);
    printf("\n");
    i = block[j]->first;

    while (1) {
      printf("    Request Liveness: %d\n", i->requestLivenessSize);
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
    
    i = i->nxt;
  }

  return block;
}

int* initBlockLink(Block *block, int *enterBlockListCount)
{
  CSGNode i;
  i = code;
  int j = 0;
  // Sum parent count
  for (j = 0 ; j < leaderCount ; j++)
  {
    if(block[j]->fail)
      block[block[j]->fail->blockId-1]->parentCount++;
    if(block[j]->branch)
      block[block[j]->branch->blockId-1]->parentCount++;
  }
  (*enterBlockListCount) = 0;
  for (j = 0 ; j < leaderCount ; j++)
  {
    if(block[j]->parentCount == 0 )
    {
      (*enterBlockListCount)++;
    }
  }
  int * enterBlockList =  malloc(sizeof(int) * (*enterBlockListCount));
  int tmpCount = 0;
  for (j = 0 ; j < leaderCount ; j++)
  {
    if(block[j]->parentCount == 0 )
    {
      enterBlockList[tmpCount++] = j;
    }
  }

  // Init Request Liveness
  for (j = 0 ; j < leaderCount ; j++)
  {
    i = block[j]->first;
    while (1) {
      i->requestLivenessList = (char*)malloc(sizeof(char) * MAX_LIVENESS);
      i->requestLivenessSize = 0;
      if (i == block[j]->last)
        break;
      i = i->nxt;
    }
  }
  return enterBlockList;
}

Block* duplicateBlock(Block *ori_block)
{
  CSGNode i, k;
  // Declar Array with size
  Block* res_block = malloc(sizeof(*res_block) * leaderCount);
  
  int j = 0;
  // For each block
  for (j = 0 ; j < leaderCount ; j++)
  {
    // Declar requestLiveness for each instr
    i = ori_block[j]->first;
    res_block[j] = malloc(sizeof(struct BlockDesc));
    res_block[j]->first = malloc(sizeof(CSGNode));
    k = res_block[j]->first;
    // For each instr
    while (1) {
      k->requestLivenessList = (char*)malloc(sizeof(char) * MAX_LIVENESS);
      k->requestLivenessSize = 0;
      k->nxt = malloc(sizeof(CSGNode));
      if (i == ori_block[j]->last)
      {
        res_block[j]->last = k;
        break;
      }
      i = i->nxt;
      k = k->nxt;
    }
  }
  
  return res_block;
}

void mallocLivenessList(Block* block)
{
  CSGNode node;
  int i, j;
  for (i=0;i<leaderCount;i++)
  {
    node = block[i]->first;
    while(1)
    {
      node->requestLivenessList = malloc(sizeof(char) * MAX_LIVENESS);
      for(j=0; j<MAX_LIVENESS; j++)
        node->requestLivenessList[j] = '\0';
      if(node == block[i]->last)
        break;
      node = node->nxt;
    }
  }
}

void copyLiveness(Block *block_src, Block *block_dst)
{
  CSGNode src_ptr, dst_ptr;
  
  int i = 0;
  // For each block
  for (i = 0 ; i < leaderCount ; i++)
  {
    printf("Block at %d\n", i);
    src_ptr = block_src[i]->first;
    dst_ptr = block_dst[i]->first;
    // For each instr
    while(1)
    {
      dst_ptr->requestLivenessSize = src_ptr->requestLivenessSize;
      int j = 0;
      for (j=0; j<dst_ptr->requestLivenessSize; j++)
      {
        dst_ptr->requestLivenessList[j] = src_ptr->requestLivenessList[j];
      }
      
      if(src_ptr == block_src[i]->last)
        break;
      src_ptr = src_ptr->nxt;
      dst_ptr = dst_ptr->nxt;
    }
  }
}

void clearVisited(Block *block)
{
  CSGNode i;
  int j;
  for (j=0;j<leaderCount;j++)
  {
    i = block[j]->first;
    while(1)
    {
      i->isVisited = 0;
      if(i == block[j]->last)
        break;
      i = i->nxt;
    }
  }
}

int isAllStateEqual(Block *block_A, Block *block_B)
{
  printf("Checking if All are equal\n");
  CSGNode A_ptr, B_ptr;
  
  int i = 0;
  // For each block
  for (i = 0 ; i < leaderCount ; i++)
  {
    A_ptr = block_A[i]->first;
    B_ptr = block_B[i]->first;
    // For each instr
    while(1)
    {
      int j = 0;
      // Compare all liveness status
      for (j=0; j < A_ptr->requestLivenessSize; j++)
      {
        if(A_ptr->requestLivenessList[j] != B_ptr->requestLivenessList[j])
        {
          return 0;
        }
      }
      if(A_ptr == block_A[i]->last)
        break;
      A_ptr = A_ptr->nxt;
      B_ptr = B_ptr->nxt;
    }
  }
  return 1;
}

void printInstrLiveness(Block *block)
{
  CSGNode i;
  int j, k;
  for (j=0;j<leaderCount;j++)
  {
    printf("Block %d\n", j+1);
    
    i = block[j]->first;

    while(1)
    {
      printf(" %d Request ", i->requestLivenessSize);
      for(k=0; k< 10; k++)
      {
        printf("%c ", i->requestLivenessList[k]);
      }
      printf("\n");
      if(i == block[j]->last)
        break;
      i = i->nxt;
    }
  }
}

void updateLiveness(Block *block)
{
  CSGNode node;
  int i, j, k;
  
  // For every block
  for (i=0;i<leaderCount;i++)
  {
    //Start from bottom of the block
    node = block[i]->last;
    while(1)
    {
      // bottom block
      if(node == block[i]->last)
      {

      }
      // top block
      else if(node == block[i]->first)
      {

      }
      // middle block
      else
      {
        // For each request character, check if already existed or not.
        // If yes, check the rule if it's passable'
        for(j = 0; j < node->nxt->requestLivenessSize; j++)
        {
          int isAldRequested = 0;
          for(k = 0; k < node->requestLivenessSize; k++)
          {
            if (node->requestLivenessSize[k] == node->nxt->requestLivenessList[j])
            {
              isAldRequested = 1;
              break;
            }
          }

          if(isAldRequested == 0 )
          {
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
          }
        }
        node->nxt->requestLiveness
      }
      node->requestLivenessSize = 1;
      node->requestLivenessList[0] = 'A';
      if(node == block[i]->first)
        break;
      node = node->prv;
    }
  }
}

void calculateLiveness(Block *block)
{
  int enterBlockListCount = 0;
  int* enterBlockList = initBlockLink(block, &enterBlockListCount);
  register CSGNode i;
  i = code;

  Block* prev_block = duplicateBlock(block);
  mallocLivenessList(prev_block);

  int counter = 0;
  do
  {
    printf("-----------------Calculate liveness Round %d-----------------\n", counter++);
    
    clearVisited(block);
    
    copyLiveness(block, prev_block);

    updateLiveness(block);
    
    printInstrLiveness(block);
    printInstrLiveness(prev_block);

    if(isAllStateEqual(block, prev_block) == 0)
      printf("Is equal: 0\n");
    else
      printf("Is equal: 1\n");
    
    if(counter == 2)
      break;

  }while(isAllStateEqual(block, prev_block) == 0);
}

void calculateLiveness_Padunk(Block *block){

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

/*
Liveness Algo

do
{
  clearVisit(instr_head)
  instr_prev = instr_head
  updateLiveness(instr_head)

}while( !isAllLivenessEqual(instr_head, instr_prev) )

func clearVisit(instr head)
{
  clear all instr visited = false
}

func updateLiveness(instr)
{
  if(instr already visited)
    return

  applyRule
  visited = true

  forall child in instr:
    updateLiveness(child)
}

func isAllLivenessEqual(instr_prev, instr_head)
{
  return true, false
}

*/