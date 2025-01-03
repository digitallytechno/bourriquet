/***************************************************************************
Bourriquet 
	digitally
 
***************************************************************************/
#ifndef DEBUG_H
#define DEBUG_H

// Tout d'abord, nous nous assurons que tous les symboles pécédemment définis sont indéfinis maintenant
// afin qu'aucun autre systeme de débogage interfère avec le nôtre.
#undef ENTER
#undef LEAVE
#undef RETURN
#undef SHOWVALUE
#undef SHOWPOINTER
#undef SHOWSTRING
#undef SHOWMSG
#undef STARTCLOCK
#undef STOPCLOCK
#undef MEMTRACK
#undef UNMEMTRACK
#undef D
#undef E
#undef W
#undef ASSERT

#if defined(DEBUG)

#include <stdarg.h>
#include <stddef.h>
#include <exec/types.h>
#include <utility/tagitem.h>

// Classes de débogage 
#define DBC_CTRACE   (1<<0) // appel de traçage (ENTER/LEAVE etc.)
#define DBC_REPORT   (1<<1) // rapports (SHOWVALUE/SHOWSTRING etc.)
#define DBC_ASSERT   (1<<2) // assertions (ASSERT)
#define DBC_TIMEVAL  (1<<3) // temps d'évaluation (STARTCLOCK/STOPCLOCK)
#define DBC_DEBUG    (1<<4) // sortie de débogage  (D())
#define DBC_ERROR    (1<<5) // sortie d'erreur     (E())
#define DBC_WARNING  (1<<6) // sortir d'avertissement   (W())
#define DBC_MTRACK   (1<<7) // traçage de la mémoire (MEMTRACK/UNMEMTRACK())
#define DBC_TAGS     (1<<8) // listes des balises (SHOWTAGS)
#define DBC_ALL      0xffffffff

// Drapeaux de débogage
#define DBF_ALWAYS   (1<<0)
#define DBF_STARTUP  (1<<1)     // pour les événements de démarrage/arrêt 
#define DBF_CLASSES   (1<<2)
#define DBF_TIMER    (1<<3)     // pour la sortie de débogage de la minuterie (Timer.c)
#define DBF_GUI      (1<<4)     // pour la sortie de la gestion graphique (GUI)
#define DBF_SERVER (1<<5)
#define DBF_MIME     (1<<6)     // pour la sortie de gestion MIME 
#define DBF_CONFIG (1<<7)	// pour la gestion de la configuration
#define DBF_NET      (1<<8)    // pour la sortie de gestion réseau 
#define DBF_UTIL     (1<<9)    // pour la sortie de gestion des utilitaires 
#define DBF_IMAGE    (1<<10)    // pour le chargement/la maintenance des fichiers images (et du cache)
#define DBF_UPDATE   (1<<11)    // pour facilité la mise à jour (UpdateCheck.c)
#define DBF_HASH     (1<<12)    // pour la gestion du HashTable (HashTable.c)
#define DBF_THEME    (1<<13)    // pour la gestion des thèmes (Themes.c)
#define DBF_THREAD   (1<<14)    // pour la gestion des threads (Thread.c)
#define DBF_MTRACK   (1<<15)    // pour montrer toutes les allocations mémoires
#define DBF_ALL      0xffffffff

// modules de débogage
#define DBM_NONE      NULL
#define DBM_ALL       "all"

// met DEBUG_MODULE à DBM_NONE par défaut au cas ou il n'était pas mis
#if !defined(DEBUG_MODULE)
#define DEBUG_MODULE DBM_NONE
#endif

void SetupDebug(void);
void CleanupDebug(void);
void DumpDbgMalloc(void);

void _ENTER(const unsigned long c, const char *m, const char *file, const unsigned long line, const char *function);
void _LEAVE(const unsigned long c, const char *m, const char *file, const unsigned long line, const char *function);
void _RETURN(const unsigned long c, const char *m, const char *file, const unsigned long line, const char *function, unsigned long result);
void _CHECKINDENT(const unsigned long c, const char *file, const unsigned long line, const long level);
void _SHOWVALUE(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const unsigned long value, const int size, const char *name);
void _SHOWPOINTER(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const void *p, const char *name);
void _SHOWSTRING(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const char *string, const char *name);
void _SHOWMSG(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const char *msg);
void _SHOWTAGS(const unsigned long c, const unsigned long f, const char *m, const char *file, unsigned long line, const struct TagItem *tags);
void _DPRINTF(const unsigned long c, const unsigned long f, const char *m, const char *file, unsigned long line, const char *format, ...);
void _STARTCLOCK(const unsigned long c, const unsigned long f, const char *m, const char *file, unsigned long line);
void _STOPCLOCK(const unsigned long c, const unsigned long f, const char *m, const char *file, unsigned long line, const char *msg);
void _MEMTRACK(const char *file, const int line, const char *func, void *ptr, size_t size);
void _UNMEMTRACK(const char *file, const int line, const void *ptr);
void _FLUSH(void);

// Messages de classe d'information 
#define ENTER()               _ENTER(DBC_CTRACE, DEBUG_MODULE, __FILE__, __LINE__, __FUNCTION__)
#define LEAVE()               _LEAVE(DBC_CTRACE, DEBUG_MODULE, __FILE__, __LINE__, __FUNCTION__)
#define RETURN(r)             _RETURN(DBC_CTRACE, DEBUG_MODULE, __FILE__, __LINE__, __FUNCTION__, (long)r)
#define CHECKINDENT(l)        _CHECKINDENT(DBC_REPORT, __FILE__, __LINE__, l)
#define SHOWVALUE(f, v)       _SHOWVALUE(DBC_REPORT, f, DEBUG_MODULE, __FILE__, __LINE__, (long)v, sizeof(v), #v)
#define SHOWPOINTER(f, p)     _SHOWPOINTER(DBC_REPORT, f, DEBUG_MODULE, __FILE__, __LINE__, p, #p)
#define SHOWSTRING(f, s)      _SHOWSTRING(DBC_REPORT, f, DEBUG_MODULE, __FILE__, __LINE__, s, #s)
#define SHOWMSG(f, msg)       _SHOWMSG(DBC_REPORT, f, DEBUG_MODULE, __FILE__, __LINE__, msg)
#define SHOWTAGS(f, t)        _SHOWTAGS(DBC_TAGS, f, DEBUG_MODULE, __FILE__, __LINE__, t)
#define STARTCLOCK(f)         _STARTCLOCK(DBC_TIMEVAL, f, DEBUG_MODULE, __FILE__, __LINE__)
#define STOPCLOCK(f, msg)     _STOPCLOCK(DBC_TIMEVAL, f, DEBUG_MODULE, __FILE__, __LINE__, msg)
#define MEMTRACK(f, p, s)     _MEMTRACK(__FILE__, __LINE__, f, p, s)
#define UNMEMTRACK(p)         _UNMEMTRACK(__FILE__, __LINE__, p)
#define FLUSH()               _FLUSH()
#define D(f, ...)             _DPRINTF(DBC_DEBUG, f, DEBUG_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define E(f, ...)             _DPRINTF(DBC_ERROR, f, DEBUG_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define W(f, ...)             _DPRINTF(DBC_WARNING, f, DEBUG_MODULE, __FILE__, __LINE__, __VA_ARGS__)
#define ASSERT(expression)      \
  ((void)                       \
   ((expression) ? 0 :          \
    (                           \
     _DPRINTF(DBC_ASSERT,       \
              DBF_ALWAYS,       \
              DEBUG_MODULE,     \
              __FILE__,         \
              __LINE__,         \
              "failed assertion '%s'", \
              #expression),     \
     abort(),                   \
     0                          \
    )                           \
   )                            \
  )

#if !defined(DEBUG_USE_MALLOC_REDEFINE) && !defined(__SASC) && !defined(__VBCC__)

// Fonctions mémoire standard 
#define malloc(s)               ({void *P = malloc(s);     _MEMTRACK(__FILE__, __LINE__, "malloc", P, s); P;})
#define calloc(n, s)            ({void *P = calloc(n, s);  _MEMTRACK(__FILE__, __LINE__, "calloc", P, n*s); P;})
#define realloc(p, s)           ({void *P; _UNMEMTRACK(__FILE__, __LINE__, p); P = realloc(p, s); _MEMTRACK(__FILE__, __LINE__, "realloc", P, s); P;})
#define strdup(s)               ({char *P = strdup(s);     _MEMTRACK(__FILE__, __LINE__, "strdup", P, strlen(s)+1); P;})
#define memdup(p, s)            ({void *P = memdup(p, s);  _MEMTRACK(__FILE__, __LINE__, "memdup", P, s); P;})
#define asprintf(p, f, ...)     ({int P = asprintf(p, f, __VA_ARGS__); if(P != -1) _MEMTRACK(__FILE__, __LINE__, "asprintf", *(p), 1); P;})
#define vasprintf(p, f, a)      ({int P = vasprintf(p, f, a); if(P != -1) _MEMTRACK(__FILE__, __LINE__, "vasprintf", *(p), 1); P;})
#define free(p)                 ({_UNMEMTRACK(__FILE__, __LINE__, p); free(p);})

// Fonctions IO standard 
#define fopen(f, m)             ({FILE *P = fopen(f, m); _MEMTRACK(__FILE__, __LINE__, "fopen", P, 1); P;})
#define fclose(p)               ({int P; _UNMEMTRACK(__FILE__, __LINE__, p); P = fclose(p); P;})

// tracage mémoire des fonctions internes de l'AmigaOS
#if !defined(__AROS__)
#undef AllocPooled
#undef FreePooled
#undef AllocVecPooled
#undef FreeVecPooled
#undef AllocDosObject
#undef AllocDosObjectTags
#undef FreeDosObject
#undef AllocSysObject
#undef AllocSysObjectTags
#undef FreeSysObject
#undef ItemPoolAlloc
#undef ItemPoolFree
#undef ExamineObject
#undef ExamineObjectTags
#undef AllocBitMap
#undef FreeBitMap
#undef ObtainDirContext
#undef ObtainDirContextTags
#undef ReleaseDirContext
#undef AllocSignal
#undef FreeSignal
#undef StartNotify
#undef EndNotify
#endif

#if defined(__amigaos4__)

#define AllocPooled(p, s)             ({APTR P = IExec->AllocPooled(p, s); _MEMTRACK(__FILE__, __LINE__, "AllocPooled", P, s); P;})
#define FreePooled(p, m, s)           ({_UNMEMTRACK(__FILE__, __LINE__, m); IExec->FreePooled(p, m, s);})
#define AllocVecPooled(p, s)          ({APTR P = IExec->AllocVecPooled(p, s); _MEMTRACK(__FILE__, __LINE__, "AllocVecPooled", P, s); P;})
#define FreeVecPooled(p, m)           ({_UNMEMTRACK(__FILE__, __LINE__, m); IExec->FreeVecPooled(p, m);})
#define AllocDosObject(t, p)          ({APTR P = IDOS->AllocDosObject(t, p); _MEMTRACK(__FILE__, __LINE__, "AllocDosObject", P, t+1); P;})
#define AllocDosObjectTags(t, ...)    ({APTR P = IDOS->AllocDosObjectTags(t, __VA_ARGS__); _MEMTRACK(__FILE__, __LINE__, "AllocDosObjectTags", P, t+1); P;})
#define ExamineObject(t)              ({APTR P = IDOS->ExamineObject(t); _MEMTRACK(__FILE__, __LINE__, "ExamineObject", P, 1); P;})
#define ExamineObjectTags(t, ...)     ({APTR P = IDOS->ExamineObjectTags(t, __VA_ARGS__); _MEMTRACK(__FILE__, __LINE__, "ExamineObjectTags", P, 1); P;})
#define FreeDosObject(t, p)           ({_UNMEMTRACK(__FILE__, __LINE__, p); IDOS->FreeDosObject(t, p);})
#define AllocSysObject(t, p)          ({APTR P = IExec->AllocSysObject(t, p); _MEMTRACK(__FILE__, __LINE__, "AllocSysObject", P, t+1); P;})
#define AllocSysObjectTags(t, ...)    ({APTR P = IExec->AllocSysObjectTags(t, __VA_ARGS__); _MEMTRACK(__FILE__, __LINE__, "AllocSysObjectTags", P, t+1); P;})
#define FreeSysObject(t, p)           ({_UNMEMTRACK(__FILE__, __LINE__, p); IExec->FreeSysObject(t, p);})
#define ItemPoolAlloc(p)              ({APTR P = IExec->ItemPoolAlloc(p); _MEMTRACK(__FILE__, __LINE__, "ItemPoolAlloc", P, 1); P; })
#define ItemPoolFree(p, i)            ({_UNMEMTRACK(__FILE__, __LINE__, i); IExec->ItemPoolFree(p, i);})
#define AllocBitMap(sx, sy, d, f, bm) ({APTR P = IGraphics->AllocBitMap(sx, sy, d, f, bm); _MEMTRACK(__FILE__, __LINE__, "AllocBitMap", P, sx); P;})
#define FreeBitMap(p)                 ({_UNMEMTRACK(__FILE__, __LINE__, p); IGraphics->FreeBitMap(p);})
#define ObtainDirContext(t)           ({APTR P = IDOS->ObtainDirContext(t); _MEMTRACK(__FILE__, __LINE__, "ObtainDirContext", P, 1); P;})
#define ObtainDirContextTags(...)     ({APTR P = IDOS->ObtainDirContextTags(__VA_ARGS__); _MEMTRACK(__FILE__, __LINE__, "ObtainDirContextTags", P, 1); P;})
#define ReleaseDirContext(p)          ({_UNMEMTRACK(__FILE__, __LINE__, p); IDOS->ReleaseDirContext(p);})
#define AllocSignal(s)                ({BYTE P = IExec->AllocSignal(s); _MEMTRACK(__FILE__, __LINE__, "AllocSignal", (APTR)(LONG)P, (size_t)s); P;})
#define FreeSignal(s)                 ({_UNMEMTRACK(__FILE__, __LINE__, (APTR)s); IExec->FreeSignal(s);})
#define StartNotify(p)                ({LONG P = IDOS->StartNotify(p); _MEMTRACK(__FILE__, __LINE__, "StartNotify", p, (size_t)p); P;})
#define EndNotify(p)                  ({_UNMEMTRACK(__FILE__, __LINE__, p); IDOS->EndNotify(p);})

#elif defined(__MORPHOS__)

#define AllocPooled(__p0, __p1) ({ \
  APTR P = LP2(708, APTR , AllocPooled, \
    APTR , __p0, a0, \
    ULONG , __p1, d0, \
    , EXEC_BASE_NAME, 0, 0, 0, 0, 0, 0); \
   _MEMTRACK(__FILE__, __LINE__, "AllocPooled", P, __p1); \
   P; \
})

#define FreePooled(__p0, __p1, __p2) ({ \
   _UNMEMTRACK(__FILE__, __LINE__, __p1); \
   LP3NR(714, FreePooled, \
      APTR , __p0, a0, \
      APTR , __p1, a1, \
      ULONG , __p2, d0, \
      , EXEC_BASE_NAME, 0, 0, 0, 0, 0, 0); \
})

#define AllocVecPooled(__p0, __p1) ({ \
  APTR P = LP2(894, APTR , AllocVecPooled, \
    APTR , __p0, a0, \
    ULONG , __p1, d0, \
    , EXEC_BASE_NAME, 0, 0, 0, 0, 0, 0); \
   _MEMTRACK(__FILE__, __LINE__, "AllocVecPooled", P, __p1); \
   P; \
})

#define FreeVecPooled(__p0, __p1) ({ \
   _UNMEMTRACK(__FILE__, __LINE__, __p1); \
  LP2NR(900, FreeVecPooled, \
    APTR , __p0, a0, \
    APTR , __p1, a1, \
    , EXEC_BASE_NAME, 0, 0, 0, 0, 0, 0); \
})

#define AllocDosObject(__p0, __p1) ({ \
  APTR P = LP2(228, APTR , AllocDosObject, \
    ULONG , __p0, d1, \
    CONST struct TagItem *, __p1, d2, \
    , DOS_BASE_NAME, 0, 0, 0, 0, 0, 0); \
  _MEMTRACK(__FILE__, __LINE__, "AllocDosObject", P, __p0); \
  P; \
})

#define FreeDosObject(__p0, __p1) ({ \
   _UNMEMTRACK(__FILE__, __LINE__, __p1); \
  LP2NR(234, FreeDosObject, \
    ULONG , __p0, d1, \
    APTR , __p1, d2, \
    , DOS_BASE_NAME, 0, 0, 0, 0, 0, 0); \
})

#define AllocSysObject(t, p)       ({APTR P = AllocSysObject(t, p); _MEMTRACK(__FILE__, __LINE__, "AllocSysObject", P, t+1); P;})
#define AllocSysObjectTags(t, ...) ({ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; AllocSysObject(t, (struct TagItem *)_tags);})
#define FreeSysObject(t, p)        ({_UNMEMTRACK(__FILE__, __LINE__, p); FreeSysObject(t, p);})
#define ItemPoolAlloc(p)           ({APTR P = ItemPoolAlloc(p); _MEMTRACK(__FILE__, __LINE__, "ItemPoolAlloc", P, 1); P; })
#define ItemPoolFree(p, i)         ({_UNMEMTRACK(__FILE__, __LINE__, i); ItemPoolFree(p, i);})

#define AllocBitMap(__p0, __p1, __p2, __p3, __p4) ({ \
  APTR P = LP5(918, struct BitMap *, AllocBitMap, \
    ULONG , __p0, d0, \
    ULONG , __p1, d1, \
    ULONG , __p2, d2, \
    ULONG , __p3, d3, \
    CONST struct BitMap *, __p4, a0, \
    , GRAPHICS_BASE_NAME, 0, 0, 0, 0, 0, 0); \
   _MEMTRACK(__FILE__, __LINE__, "AllocBitMap", P, __p0); \
   P; \
})

#define FreeBitMap(__p0) ({ \
   _UNMEMTRACK(__FILE__, __LINE__, __p0); \
  LP1NR(924, FreeBitMap, \
    struct BitMap *, __p0, a0, \
    , GRAPHICS_BASE_NAME, 0, 0, 0, 0, 0, 0); \
})

#define ObtainDirContext(t)       ({APTR P = ObtainDirContext(t); _MEMTRACK(__FILE__, __LINE__, "ObtainDirContext", P, 1); P;})
#define ObtainDirContextTags(...) ({ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; ObtainDirContext((struct TagItem *)_tags);})
#define ReleaseDirContext(p)      ({_UNMEMTRACK(__FILE__, __LINE__, p); ReleaseDirContext(p);})

#define AllocSignal(__p0) ({ \
   BYTE P = LP1(330, BYTE , AllocSignal, \
      LONG , __p0, d0, \
      , EXEC_BASE_NAME, 0, 0, 0, 0, 0, 0); \
   _MEMTRACK(__FILE__, __LINE__, "AllocSignal", (APTR)(LONG)P, (size_t)__p0); \
   P; \
})

#define FreeSignal(__p0) ({ \
   _UNMEMTRACK(__FILE__, __LINE__, (APTR)__p0); \
   LP1NR(336, FreeSignal, \
      LONG , __p0, d0, \
      , EXEC_BASE_NAME, 0, 0, 0, 0, 0, 0); \
})

#define StartNotify(__p0) ({ \
   BOOL P = LP1(888, BOOL , StartNotify, \
      struct NotifyRequest *, __p0, d1, \
      , DOS_BASE_NAME, 0, 0, 0, 0, 0, 0); \
   _MEMTRACK(__FILE__, __LINE__, "StartNotify", __p0, (size_t)__p0); \
   P; \
})

#define EndNotify(__p0) ({ \
   _UNMEMTRACK(__FILE__, __LINE__, __p0); \
   LP1NR(894, EndNotify, \
      struct NotifyRequest *, __p0, d1, \
      , DOS_BASE_NAME, 0, 0, 0, 0, 0, 0); \
})

#elif !defined(__AROS__) // AmigaOS 3

#include <inline/macros.h>

#define AllocPooled(__p0, __p1) ({ \
  APTR P = LP2(0x2c4, APTR , AllocPooled, \
    APTR , __p0, a0, \
    ULONG , __p1, d0, \
    , EXEC_BASE_NAME); \
   _MEMTRACK(__FILE__, __LINE__, "AllocPooled", P, __p1); \
   P; \
})

#define FreePooled(__p0, __p1, __p2) ({ \
   _UNMEMTRACK(__FILE__, __LINE__, __p1); \
   LP3NR(0x2ca, FreePooled, \
      APTR , __p0, a0, \
      APTR , __p1, a1, \
      ULONG , __p2, d0, \
      , EXEC_BASE_NAME); \
})

#define AllocVecPooled(p, s) ({APTR P = AllocVecPooled(p, s); _MEMTRACK(__FILE__, __LINE__, "AllocVecPooled", P, s); P;})
#define FreeVecPooled(p, m)  ({_UNMEMTRACK(__FILE__, __LINE__, m); FreeVecPooled(p, m);})

#define AllocDosObject(__p0, __p1) ({ \
  APTR P = LP2(0xe4, APTR , AllocDosObject, \
    ULONG , __p0, d1, \
    CONST struct TagItem *, __p1, d2, \
    , DOS_BASE_NAME); \
  _MEMTRACK(__FILE__, __LINE__, "AllocDosObject", P, __p0); \
  P; \
})

#define FreeDosObject(__p0, __p1) ({ \
  _UNMEMTRACK(__FILE__, __LINE__, __p1); \
  LP2NR(0xea, FreeDosObject, \
    ULONG , __p0, d1, \
    APTR , __p1, d2, \
    , DOS_BASE_NAME); \
})

#define AllocSysObject(t, p)       ({APTR P = AllocSysObject(t, p); _MEMTRACK(__FILE__, __LINE__, "AllocSysObject", P, t+1); P;})
#define AllocSysObjectTags(t, ...) ({ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; AllocSysObject(t, (struct TagItem *)_tags);})
#define FreeSysObject(t, p)        ({_UNMEMTRACK(__FILE__, __LINE__, p); FreeSysObject(t, p);})
#define ItemPoolAlloc(p)           ({APTR P = ItemPoolAlloc(p); _MEMTRACK(__FILE__, __LINE__, "ItemPoolAlloc", P, 1); P; })
#define ItemPoolFree(p, i)         ({_UNMEMTRACK(__FILE__, __LINE__, i); ItemPoolFree(p, i);})

#define AllocBitMap(__p0, __p1, __p2, __p3, __p4) ({ \
  APTR P = LP5(0x396, struct BitMap *, AllocBitMap, \
    ULONG , __p0, d0, \
    ULONG , __p1, d1, \
    ULONG , __p2, d2, \
    ULONG , __p3, d3, \
    CONST struct BitMap *, __p4, a0, \
    , GRAPHICS_BASE_NAME); \
   _MEMTRACK(__FILE__, __LINE__, "AllocBitMap", P, __p0); \
   P; \
})

#define FreeBitMap(__p0) ({ \
  _UNMEMTRACK(__FILE__, __LINE__, __p0); \
  LP1NR(0x39c, FreeBitMap, \
    struct BitMap *, __p0, a0, \
    , GRAPHICS_BASE_NAME); \
})

#define ObtainDirContext(t)       ({APTR P = ObtainDirContext(t); _MEMTRACK(__FILE__, __LINE__, "ObtainDirContext", P, 1); P;})
#define ObtainDirContextTags(...) ({ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; ObtainDirContext((struct TagItem *)_tags);})
#define ReleaseDirContext(p)      ({_UNMEMTRACK(__FILE__, __LINE__, p); ReleaseDirContext(p);})

#define AllocSignal(__p0) ({ \
  BYTE P = \
  LP1(0x14a, BYTE , AllocSignal, LONG , __p0, d0, \
  , EXEC_BASE_NAME); \
  _MEMTRACK(__FILE__, __LINE__, "AllocSignal", (APTR)(LONG)P, (size_t)__p0); \
  P; \
})

#define FreeSignal(__p0) ({ \
  _UNMEMTRACK(__FILE__, __LINE__, (APTR)__p0); \
  LP1NR(0x150, FreeSignal, LONG , __p0, d0, \
  , EXEC_BASE_NAME); \
})

#define StartNotify(__p0) ({ \
  BOOL P = \
  LP1(0x378, BOOL, StartNotify, struct NotifyRequest *, __p0, d1, \
  , DOS_BASE_NAME); \
  _MEMTRACK(__FILE__, __LINE__, "StartNotify", __p0, (size_t)__p0); \
  P; \
})

#define EndNotify(__p0) ({ \
  _UNMEMTRACK(__FILE__, __LINE__, __p0); \
  LP1NR(0x37e, EndNotify, struct NotifyRequest *, __p0, d1, \
  , DOS_BASE_NAME); \
})

#endif // amigaos4

#endif // !DEBUG_USE_MALLOC_REDEFINE

#else // DEBUG

// remplacer avec des NOPs est important ici!
#define ENTER()               ((void)0)
#define LEAVE()               ((void)0)
#define RETURN(r)             ((void)0)
#define CHECKINDENT(l)        ((void)0)
#define SHOWVALUE(f, v)       ((void)0)
#define SHOWPOINTER(f, p)     ((void)0)
#define SHOWSTRING(f, s)      ((void)0)
#define SHOWMSG(f, m)         ((void)0)
#define SHOWTAGS(f, t)        ((void)0)
#define STARTCLOCK(f)         ((void)0)
#define STOPCLOCK(f, m)       ((void)0)
#define MEMTRACK(f, p, s)     ((void)0)
#define UNMEMTRACK(p)         ((void)0)
#define FLUSH()               ((void)0)
#define D(f, ...)             ((void)0)
#define E(f, ...)             ((void)0)
#define W(f, ...)             ((void)0)
#define ASSERT(expression)    ((void)0)

#define DumpDbgMalloc()       ((void)0)

// nous sortons un warning si printf()/kprintf()
// sont utilisés quand une version non-debug doit être compiler
#if (defined(__GNUC__) && __GNUC__ >= 3)
#define DO_PRAGMA(x) _Pragma(#x)
#define kprintf(...) DO_PRAGMA(message("#warning: kprintf() used in non-debug mode"))
#define printf(...)  DO_PRAGMA(message("#warning: printf() used in non-debug mode"))
#else
#define kprintf(...) ((void)0)
#define printf(...)  ((void)0)
#endif

#endif // DEBUG

#endif // DEBUG_H
