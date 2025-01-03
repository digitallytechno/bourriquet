/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <exec/lists.h>
#include "Extrasrc.h"

#if defined(NEED_GETTAIL)
struct Node *GetTail(struct List *list)
{
    struct Node *result = NULL;

    if(list != NULL && IsListEmpty(list) == FALSE) result = list->lh_TailPred;
    return(result);
}
#else
  #warning "NEED_GETTAIL absence ou compilation inutile"
#endif
