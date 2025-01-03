/***************************************************************************
 BOurriquet
  digitally
***************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "Extrasrc.h"

#if defined(NEED_VASPRINTF)

int vasprintf(char **ptr, const char * format, va_list ap)
{
    int ret;
    va_list ap2;

    *ptr = NULL;
    VA_COPY(ap2, ap);
    ret = vsnprintf(NULL, 0, format, ap2);
    if(ret <= 0) return(ret);
    *ptr = (char *)malloc(ret+1);
    if(*ptr == NULL) return(-1);
    VA_COPY(ap2, ap);
    ret = vsnprintf(*ptr, ret+1, format, ap2);
    return(ret);
}
#else
  #warning "NEED_VASPRINTF absence ou compilation inutile"
#endif
