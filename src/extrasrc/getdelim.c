/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <limits.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "Extrasrc.h"
#include "Debug.h"

#if defined(NEED_GETDELIM)

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX ((size_t)(SIZE_MAX / 2))
#endif

ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream)
{
    ssize_t result = -1;

    ENTER();
    if(lineptr != NULL && n != NULL && stream != NULL)
      {
        if(*lineptr == NULL || *n == 0)
          {
            *n = 120;
            *lineptr = (char *)realloc(*lineptr, *n);
          }
        if(*lineptr != NULL)
          {
            size_t cur_len = 0;
            char *cptr = *lineptr; // nous utilisons cptr comme raccourci vers la fin de la ligne actuelle

            for(;;)
              {
                int i;

                i = getc(stream);
                if(i == EOF) break;
                if(cur_len+1 >= *n)
                  {
                    const size_t needed_max = SSIZE_MAX < SIZE_MAX ? (size_t)SSIZE_MAX + 1 : SIZE_MAX;
                    size_t needed = 2 * (*n) + 1; 
                    char *new_lineptr;

                    D(DBF_UTIL, "getline(): realloc � %ld", needed);
                    if(needed_max < needed) needed = needed_max;
                    if(cur_len+1 >= needed) goto out;
                    new_lineptr = (char *)realloc(*lineptr, needed);
                    if(new_lineptr == NULL) goto out;
                    *lineptr = new_lineptr;
                    *n = needed;
                    cptr = &new_lineptr[cur_len];
                  }
                *cptr = i;
                cptr++;
                cur_len++;
                if(i == delim) break;
              }
            *cptr = '\0';
            result = cur_len > 0 ? (ssize_t)cur_len : result;
          }
      }
  out:
    RETURN(result);
    return(result);
}

#else
  #warning "NEED_GETDELIM absence ou compilation inutile"
#endif
