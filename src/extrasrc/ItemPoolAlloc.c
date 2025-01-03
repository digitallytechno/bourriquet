/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <exec/types.h>
#include <proto/exec.h>
#include "Extrasrc.h"
#include "ItemPool.h"
#define DEBUG_USE_MALLOC_REDEFINE
#include "Debug.h"

#if defined(NEED_ITEMPOOLALLOC)
APTR ItemPoolAlloc(APTR poolHeader)
{
    struct ItemPool *pool = poolHeader;
    APTR item;

    item = AllocPooled(pool->pool, pool->itemSize);
    return(item);
}
#else
  #warning "NEED_ITEMPOOLALLOC absence or compilation inutile"
#endif

///
