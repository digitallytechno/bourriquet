/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include "SDI_compiler.h"
#include "SDI_stdarg.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "Extrasrc.h"

#if defined(NEED_ASPRINTF)
int asprintf(char **ptr, const char * format, ...)
{
    va_list ap;
    int ret;

    *ptr = NULL;
    va_start(ap, format);
    ret = vasprintf(ptr, format, ap);
    va_end(ap);

    return(ret);
}
#else
  #warning "NEED_ASPRINTF absence ou compilation inutile"
#endif
