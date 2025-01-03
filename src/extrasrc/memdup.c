/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "Extrasrc.h"

#if defined(NEED_MEMDUP)
void *memdup(const void *src, const size_t size)
{
    void *dst = NULL;
    void *new;

    if(src != NULL && (new = malloc(size)) != NULL) dst = memcpy(new, src, size);
    return(dst);
}
///
#else
  #warning "NEED_MEMDUP absence ou compilation inutile"
#endif
