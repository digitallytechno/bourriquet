/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <clib/alib_protos.h>
#include <exec/lists.h>
#include "Extrasrc.h"

#if defined(NEED_MOVELIST)
void MoveList(struct List *destList, struct List *sourceList)
{
    if(IsListEmpty(sourceList) == FALSE)
      {
        destList->lh_TailPred->ln_Succ = sourceList->lh_Head;
        destList->lh_TailPred->ln_Succ->ln_Pred = destList->lh_TailPred;
        destList->lh_TailPred = sourceList->lh_TailPred;
        destList->lh_TailPred->ln_Succ = (struct Node *)&destList->lh_Tail;
        NewList(sourceList);
      }
}
#else
  #warning "NEED_MOVELIST absence ou compilation inutile"
#endif
