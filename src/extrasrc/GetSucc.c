/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <exec/lists.h>
#include "Extrasrc.h"

#if defined(NEED_GETSUCC)
struct Node *GetSucc(struct Node *node)
{
    struct Node *result = NULL;

    if(node != NULL && node->ln_Succ != NULL && node->ln_Succ->ln_Succ != NULL) result = node->ln_Succ;
    return(result);
}
#else
  #warning "NEED_GETSUCC absence ou compilation inutile"
#endif
