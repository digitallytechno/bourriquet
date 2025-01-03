/***************************************************************************
Bourriquet  
	digitally
 
***************************************************************************/
#ifndef EXTRASRC_H
#define EXTRASRC_H

#include <sys/types.h>
#include <stddef.h>
#include <stdio.h>

#ifndef EXEC_TYPES_H
  #include <exec/types.h>         // ULONG
#endif
#ifndef INTUITION_CLASSUSR_H
  #include <intuition/classusr.h> // Object
#endif
#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif
#ifndef SDI_STDARG_H
  #include "SDI_stdarg.h"
#endif

struct TagItem;
struct IClass;
struct MinList;
struct NewRDArgs;
struct WBStartup;
struct List;
struct Node;

#define NEED_ALLOCSYSOBJECT
#define NEED_EXAMINEDIR
#define NEED_GETHEAD
#define NEED_GETPRED
#define NEED_GETSUCC
#define NEED_GETTAIL
#define NEED_ITEMPOOLALLOC
#define NEED_ITEMPOOLFREE
#define NEED_MOVELIST
#define NEED_NEWREADARGS
#define NEED_SETPROCWINDOW
#define NEED_ASPRINTF
#define NEED_GETDELIM
#define NEED_MEMDUP
#define NEED_VASPRINTF
#define NEED_CHANGEFILEPOSITION
#define NEED_TZLIB

#if defined(NEED_STRLCPY)
size_t strlcpy(char *, const char *, size_t);
#endif

#if defined(NEED_STRLCAT)
size_t strlcat(char *, const char *, size_t);
#endif

#if defined(NEED_STRTOK_R)
char *strtok_r(char *, const char *, char **);
#endif

#if defined(NEED_VSNPRINTF)
int vsnprintf(char *buffer, size_t maxlen, const char *fmt, VA_LIST args);
#endif

#if defined(NEED_SNPRINTF)
int snprintf(char *buffer, size_t maxlen, const char *fmt, ...);
#endif

#if defined(NEED_VASPRINTF)
int vasprintf(char **ptr, const char *format, VA_LIST ap);
#endif

#if defined(NEED_ASPRINTF)
int asprintf(char **ptr, const char *format, ...);
#endif

#if defined(NEED_STCGFE)
int stcgfe(char *, const char *);
#endif

#if defined(NEED_STRDUP)
char *strdup(const char *);
#endif

#if defined(NEED_MEMDUP)
void *memdup(const void *source, const size_t size);
#endif

#if defined(NEED_GETDELIM)
#define getline(p, n, s) getdelim((p), (n), '\n', (s))
ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);
#endif

#if defined(NEED_XGET)
IPTR xget(Object *obj, const IPTR attr);
#endif

#if defined(NEED_XSET)
ULONG VARARGS68K xset(Object *obj, ...);
#endif

#if defined(NEED_DOSUPERNEW)
#if defined(__GNUC__) && __GNUC__ == 2
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...);
#define DoSuperNew(cl, obj, ...) ({ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; (Object *)DoSuperMethod(cl, obj, OM_NEW, (struct TagItem *)_tags, NULL);})
#endif
#endif

#if defined(NEED_SETPROCWINDOW)
APTR SetProcWindow(const void *newWindowPtr);
#endif

#if defined(NEED_EXAMINEDIR)
APTR ObtainDirContext(struct TagItem *tags);
#define ObtainDirContextTags(...) ({ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; ObtainDirContext((struct TagItem *)_tags);})
void ReleaseDirContext(APTR context);
struct ExamineData *ExamineDir(APTR context);
#include "extrasrc/ExamineDir.h"
#endif

#if defined(NEED_ALLOCSYSOBJECT)
APTR AllocSysObject(ULONG type, struct TagItem *tags);
#define AllocSysObjectTags(type, ...) ({ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; AllocSysObject(type, (struct TagItem *)_tags);})
void FreeSysObject(ULONG type, APTR object);
#include "Extrasrc/AllocSysObject.h"
#endif

#if defined(NEED_MOVELIST)
void MoveList(struct List *destList, struct List *sourceList);
#endif

#if defined(NEED_GETHEAD)
struct Node *GetHead(struct List *list);
#endif

#if defined(NEED_GETPRED)
struct Node *GetPred(struct Node *node);
#endif

#if defined(NEED_GETSUCC)
struct Node *GetSucc(struct Node *node);
#endif

#if defined(NEED_GETTAIL)
struct Node *GetTail(struct List *list);
#endif

#if defined(NEED_ITEMPOOLALLOC)
APTR ItemPoolAlloc(APTR poolHeader);
#endif

#if defined(NEED_ITEMPOOLFREE)
APTR ItemPoolFree(APTR poolHeader, APTR item);
#endif

#if defined(NEED_NEWMINLIST)
#undef NewMinList
void NewMinList(struct MinList *list);
#endif

#if defined(NEED_ALLOCVECPOOLED)
APTR AllocVecPooled(APTR poolHeader, ULONG memSize);
#endif

#if defined(NEED_FREEVECPOOLED)
void FreeVecPooled(APTR poolHeader, APTR memory);
#endif

#if defined(NEED_CHANGEFILEPOSITION)
#define ChangeFilePosition(fh, pos, mode)   Seek(fh, pos, mode)
#endif

#if defined(NEED_NEWREADARGS)
LONG NewReadArgs(struct WBStartup *, struct NewRDArgs *);
void NewFreeArgs(struct NewRDArgs *);
#endif

#if defined(NEED_STRCASESTR)
char *strcasestr(const char *haystack, const char *needle);
#endif

#endif /* EXTRASRC_H */
