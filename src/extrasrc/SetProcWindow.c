/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <dos/dosextens.h>
#include <proto/exec.h>

#include "Extrasrc.h"
#include "Debug.h"

#if defined(NEED_SETPROCWINDOW)

APTR SetProcWindow(const void *newWindowPtr)
{
    struct Process *pr;
    APTR oldWindowPtr;

    ENTER();
    pr = (struct Process *)FindTask(NULL);
    oldWindowPtr = pr->pr_WindowPtr;
    pr->pr_WindowPtr = (APTR)newWindowPtr;
    RETURN(oldWindowPtr);
    return(oldWindowPtr);
}
///
#else
  #warning "NEED_SETPROCWINDOW absence ou compilation inutile"
#endif
