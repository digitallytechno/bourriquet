/***************************************************************************
Bourriquet  
	digitally
 
***************************************************************************/
#ifndef METHODSTACK_H
#define METHODSTACK_H

#include "SDI_compiler.h"

#ifndef INTUITION_CLASSUSR_H
  #include <intuition/classusr.h> 
#endif

BOOL InitMethodStack(void);
void CleanupMethodStack(void);
BOOL PushMethodOnStackA(Object *obj, ULONG argCount, struct TagItem *tags);
#define PushMethodOnStack(obj, argCount, ...) ({ ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; PushMethodOnStackA(obj, argCount, (struct TagItem *)_tags); })
IPTR PushMethodOnStackWaitA(Object *obj, ULONG argCount, struct TagItem *tags);
#define PushMethodOnStackWait(obj, argCount, ...) ({ ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; PushMethodOnStackWaitA(obj, argCount, (struct TagItem *)_tags); })
void CheckMethodStack(void);

#endif /* METHODSTACK_H */

