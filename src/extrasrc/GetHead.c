/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <exec/lists.h>

#include "Extrasrc.h"
#if defined(NEED_GETHEAD)

struct Node *GetHead(struct List *list)
{
    struct Node *result = NULL;

    if(list != NULL && IsListEmpty(list) == FALSE) result = list->lh_Head;
    return(result);
}
#else
  #warning "NEED_GETHEAD absence ou compilation inutile"
#endif