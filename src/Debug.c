/***************************************************************************
Bourriquet  
	digitally
 
***************************************************************************/

#ifdef DEBUG

#include <stdio.h> // vsnprintf
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>

#include "Global.h"
#include "Utilities.h" 
#include "Extrasrc.h"

#include "SDI_compiler.h"
#include "timeval.h"

#define DEBUG_USE_MALLOC_REDEFINE 1
#include "Debug.h"

#if defined(__MORPHOS__)
#include <exec/rawfmt.h>
#elif defined(__AROS__)
#include <proto/arossupport.h>
#else
#include <clib/debug_protos.h>
#endif

static BOOL ansi_output = FALSE;
static BOOL stdout_output = FALSE;
static FILE *file_output = NULL;
static ULONG debug_flags = DBF_ALWAYS; 
static ULONG debug_classes = DBC_ERROR | DBC_DEBUG | DBC_WARNING | DBC_ASSERT | DBC_REPORT | DBC_MTRACK | DBC_TIMEVAL; // classes de débogage par défaut 
static char debug_modules[256] = "";
static char debug_files[256] = "";
static int timer_level = -1;
static struct TimeVal startTimes[8];

static void SetupDbgMalloc(void);
static void CleanupDbgMalloc(void);

#define ANSI_ESC_CLR        "\033[0m"
#define ANSI_ESC_BOLD       "\033[1m"
#define ANSI_ESC_UNDERLINE  "\033[4m"
#define ANSI_ESC_BLINK      "\033[5m"
#define ANSI_ESC_REVERSE    "\033[7m"
#define ANSI_ESC_INVISIBLE  "\033[8m"
#define ANSI_ESC_FG_BLACK   "\033[0;30m"
#define ANSI_ESC_FG_RED     "\033[0;31m"
#define ANSI_ESC_FG_GREEN   "\033[0;32m"
#define ANSI_ESC_FG_BROWN   "\033[0;33m"
#define ANSI_ESC_FG_BLUE    "\033[0;34m"
#define ANSI_ESC_FG_PURPLE  "\033[0;35m"
#define ANSI_ESC_FG_CYAN    "\033[0;36m"
#define ANSI_ESC_FG_LGRAY   "\033[0;37m"
#define ANSI_ESC_FG_DGRAY   "\033[1;30m"
#define ANSI_ESC_FG_LRED    "\033[1;31m"
#define ANSI_ESC_FG_LGREEN  "\033[1;32m"
#define ANSI_ESC_FG_YELLOW  "\033[1;33m"
#define ANSI_ESC_FG_LBLUE   "\033[1;34m"
#define ANSI_ESC_FG_LPURPLE "\033[1;35m"
#define ANSI_ESC_FG_LCYAN   "\033[1;36m"
#define ANSI_ESC_FG_WHITE   "\033[1;37m"
#define ANSI_ESC_BG         "\033[0;4"    
#define ANSI_ESC_BG_BLACK   "\033[0;40m"
#define ANSI_ESC_BG_RED     "\033[0;41m"
#define ANSI_ESC_BG_GREEN   "\033[0;42m"
#define ANSI_ESC_BG_BROWN   "\033[0;43m"
#define ANSI_ESC_BG_BLUE    "\033[0;44m"
#define ANSI_ESC_BG_PURPLE  "\033[0;45m"
#define ANSI_ESC_BG_CYAN    "\033[0;46m"
#define ANSI_ESC_BG_LGRAY   "\033[0;47m"

#define DBC_CTRACE_COLOR    ANSI_ESC_FG_BROWN
#define DBC_REPORT_COLOR    ANSI_ESC_FG_PURPLE
#define DBC_ASSERT_COLOR    ANSI_ESC_FG_RED
#define DBC_TIMEVAL_COLOR   ANSI_ESC_FG_BLUE
#define DBC_DEBUG_COLOR     ANSI_ESC_FG_GREEN
#define DBC_ERROR_COLOR     ANSI_ESC_FG_RED
#define DBC_WARNING_COLOR   ANSI_ESC_FG_YELLOW
#define DBC_CTRACE_BGCOLOR  ANSI_ESC_BG_BROWN
#define DBC_REPORT_BGCOLOR  ANSI_ESC_BG_PURPLE
#define DBC_ASSERT_BGCOLOR  ANSI_ESC_BG_RED
#define DBC_TIMEVAL_BGCOLOR ANSI_ESC_BG_BLUE
#define DBC_DEBUG_BGCOLOR   ANSI_ESC_BG_GREEN
#define DBC_ERROR_BGCOLOR   ANSI_ESC_BG_RED
#define DBC_WARNING_BGCOLOR ANSI_ESC_BG_CYAN

#define DBC_CTRACE_STR      "C"
#define DBC_REPORT_STR      "R"
#define DBC_ASSERT_STR      "A"
#define DBC_TIMEVAL_STR     "T"
#define DBC_DEBUG_STR       "D"
#define DBC_ERROR_STR       "E"
#define DBC_WARNING_STR     "W"

#if defined(NO_THREADS)
#define THREAD_MAX          1
#define THREAD_LOCK         (void(0))
#define THREAD_UNLOCK       (void(0))
#define THREAD_ID           0
#define INDENT_LEVEL        indent_level[THREAD_ID]
#else
#define THREAD_MAX          256
#define THREAD_LOCK         ObtainSemaphore(&thread_lock)
#define THREAD_UNLOCK       ReleaseSemaphore(&thread_lock)
#define THREAD_ID           _thread_id(FindTask(NULL))
#define INDENT_LEVEL        indent_level[THREAD_ID]
static struct SignalSemaphore thread_lock;
static void *thread_id[THREAD_MAX];
#endif

#define INDENT_MAX          80
static int indent_level[THREAD_MAX];
static char indent_spaces[INDENT_MAX];

#if !defined(NO_THREADS)
static INLINE int _thread_id(const void *thread_ptr)
	{
  		int result=-1;
  		int i=0;

  		while(i < THREAD_MAX && thread_id[i] != NULL)
  			{
    			if(thread_id[i] == thread_ptr)
    				{
      					result = i;
      					break;
    				}
    			i++;
  			}
  		if(result == -1)
  			{
    			if(i < THREAD_MAX)
    				{
      					thread_id[i] = (void *)thread_ptr;
      					result = i;
    				}
  			}
  		return result;
	}

#endif

static void _DBPRINTF(const char *format, ...)
	{
  		va_list args;

  		va_start(args, format);
  		if(stdout_output == TRUE)
  			{
    			vprintf(format, args);
  			}
  		else if(file_output != NULL)
  			{
    			vfprintf(file_output, format, args);
    			fflush(file_output);
  			}
  		else VNewRawDoFmt(format, (APTR)RAWFMTFUNC_SERIAL, NULL, args);  			
  		va_end(args);
	}

/****************************************************************************/
static INLINE char *_INDENT(void)
	{
  		int levels = INDENT_LEVEL;
  		int i;

  		for(i=0; i < levels && i < INDENT_MAX; i++) indent_spaces[i] = ' ';
  		indent_spaces[i] = '\0';
  		return indent_spaces;
	}
/****************************************************************************/
static INLINE BOOL matchDebugSpec(const unsigned long c, const unsigned f, const char *m, const char *file)
	{
  		BOOL match = FALSE;

  		if((isFlagSet(debug_classes, c) && isFlagSet(debug_flags, f)) || (isFlagSet(c, DBC_ERROR) || isFlagSet(c, DBC_WARNING)))	match = TRUE;
  		else if(m != NULL && strcasestr(debug_modules, m) != NULL) match = TRUE;
  		else if(file != NULL && strcasestr(debug_files, file) != NULL) match = TRUE;
		return match;
	}
/****************************************************************************/
static char *_NOW(void)
	{
  		struct TimeVal now;
  		LONG hour;
  		LONG minute;
  		LONG second;
  		static char nowStr[64];

  		if(TimerBase != NULL)
  			{
    			GetSysTime(TIMEVAL(&now));
    			now.Seconds %= 86400;
  			}
  		else memset(&now, 0, sizeof(now));
	  	second = now.Seconds % 60;
  		minute = now.Seconds / 60;
  		hour = minute / 60;
  		minute %= 60;
  		snprintf(nowStr, sizeof(nowStr), "%02ld:%02ld:%02ld.%06ld", hour, minute, second, now.Microseconds);
  		return nowStr;
	}
/****************************************************************************/
static INLINE void _VDPRINTF(const unsigned long c, const char *file, unsigned long line, const char *format, va_list args)
	{
  		char buf[1024];
  		const char *fg;
  		const char *bg;
  		const char *id;
  		const int threadID = THREAD_ID;

  		vsnprintf(buf, sizeof(buf), format, args);
  		switch(c)
  			{
    			case DBC_CTRACE:  fg = DBC_CTRACE_COLOR;  bg = DBC_CTRACE_BGCOLOR;  id = DBC_CTRACE_STR;  break;
    			case DBC_REPORT:  fg = DBC_REPORT_COLOR;  bg = DBC_REPORT_BGCOLOR;  id = DBC_REPORT_STR;  break;
    			case DBC_ASSERT:  fg = DBC_ASSERT_COLOR;  bg = DBC_ASSERT_BGCOLOR;  id = DBC_ASSERT_STR;  break;
    			case DBC_TIMEVAL: fg = DBC_TIMEVAL_COLOR; bg = DBC_TIMEVAL_BGCOLOR; id = DBC_TIMEVAL_STR; break;
    			case DBC_DEBUG:   fg = DBC_DEBUG_COLOR;   bg = DBC_DEBUG_BGCOLOR;   id = DBC_DEBUG_STR;   break;
    			case DBC_ERROR:   fg = DBC_ERROR_COLOR;   bg = DBC_ERROR_BGCOLOR;   id = DBC_ERROR_STR;   break;
    			case DBC_WARNING: fg = DBC_WARNING_COLOR; bg = DBC_WARNING_BGCOLOR; id = DBC_WARNING_STR; break;
    			default:          fg = ANSI_ESC_FG_WHITE; bg = ANSI_ESC_FG_WHITE;   id = DBC_DEBUG_STR;   break;
  			}
  		if(ansi_output) _DBPRINTF("%s%ldm%02ld:%s%s%s%s%s|%s|%s%s:%ld:%s%s\n",  ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR, bg, id, ANSI_ESC_CLR, fg, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, buf, ANSI_ESC_CLR);
        else _DBPRINTF("%02ld:%s|%s|%s%s:%ld:%s\n", threadID, id, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, buf);
  }

/****************************************************************************/
void SetupDebug(void)
	{
  		char var[256];

  		#if !defined(NO_THREADS)
  			memset(&thread_lock, 0, sizeof(thread_lock));
  			memset(&thread_id, 0, sizeof(thread_id));
  			InitSemaphore(&thread_lock);
  		#endif
  		memset(&indent_level, 0, sizeof(indent_level));
  		if(GetVar("Bourriquetdebug", var, sizeof(var), 0) > 0)
  			{
    			char *s = var;
    			char *t;
    			static const struct { const char *token; unsigned long flag; } dbclasses[] =
    				{
      					{ "ctrace",  DBC_CTRACE   },
      					{ "report",  DBC_REPORT   },
      					{ "assert",  DBC_ASSERT   },
      					{ "timeval", DBC_TIMEVAL  },
      					{ "debug",   DBC_DEBUG    },
      					{ "error",   DBC_ERROR    },
      					{ "warning", DBC_WARNING  },
      					{ "mtrack",  DBC_MTRACK   },
      					{ "tags",    DBC_TAGS     },
      					{ "all",     DBC_ALL      },
      					{ NULL,      0            }
    				};
    			static const struct { const char *token; unsigned long flag; } dbflags[] =
    				{
      					{ "always",   DBF_ALWAYS  },
      					{ "startup",  DBF_STARTUP },
						{ "classes",   DBF_CLASSES },
      					{ "timer",    DBF_TIMER   },     		
      					{ "gui",      DBF_GUI     },
						{ "server",  DBF_SERVER },
      					{ "mime",     DBF_MIME    },
						{ "config",     DBF_CONFIG    },
      					{ "net",      DBF_NET     },
      					{ "util",     DBF_UTIL    },
      					{ "image",    DBF_IMAGE   },
      					{ "update",   DBF_UPDATE  },
      					{ "hash",     DBF_HASH    },
      					{ "theme",    DBF_THEME   },
      					{ "thread",   DBF_THREAD  },
      					{ "mtrack",   DBF_MTRACK  },
      					{ "all",      DBF_ALL     },
      					{ NULL,       0           }
    				};
					
    			if(strcasestr(s, "ansi") != NULL) ansi_output = TRUE;
			    if(strcasestr(s, "stdout") != NULL) stdout_output = TRUE;
			    if((t = strcasestr(s, "file:")) != NULL)
    				{
      					char *e;
      					char filename[256];
      
						t += 5;
      					if((e = strpbrk(t, " ,;")) == NULL) strlcpy(filename, t, sizeof(filename));
      					else strlcpy(filename, t, e-t+1);
      					file_output = fopen(filename, "w");
    				}
    		_DBPRINTF("** %s Date %s (Construction %s) Démarrage  **********************\n", bourriquetversion, bourriquetversiondate, bourriquetbuildid);
    		_DBPRINTF("Exec version: v%ld.%ld\n", (unsigned long)((struct Library *)SysBase)->lib_Version, (unsigned long)((struct Library *)SysBase)->lib_Revision);
    		_DBPRINTF("Initialisation du débogage d'execution:\n");
    		_DBPRINTF("Analyse du contenu de ENV:Bourriquetdebug:'%s'\n", s);
    		while(*s)
    			{
      				ULONG i;
      				char *e;

      				if((e = strpbrk(s, " ,;")) == NULL) e = s+strlen(s);
      				if(s[0] == '@')
      					{
        					s++;
        					if(s[0] == '!')
        						{
          							s++;
          							for(i=0; dbclasses[i].token; i++)
          								{
            								if(strnicmp(s, dbclasses[i].token, strlen(dbclasses[i].token)) == 0)
            									{
              										_DBPRINTF("Effacement du drapeau '%s' de la classe débogage.\n", dbclasses[i].token);
										            clearFlag(debug_classes, dbclasses[i].flag);
    									            break;
             									}
          								}
        						}
        					else
        						{
          							for(i=0; dbclasses[i].token; i++)
          								{
            								if(strnicmp(s, dbclasses[i].token, strlen(dbclasses[i].token)) == 0)
            									{
              										_DBPRINTF("Met le drapeau '%s' de la classe débogage.\n", dbclasses[i].token);
              										setFlag(debug_classes, dbclasses[i].flag);
										            break;
            									}
          								}
        						}
      					}
      				else
      					{
        					if(s[0] == '!')
        						{
          							s++;
          							for(i=0; dbflags[i].token; i++)
          								{
            								if(strnicmp(s, dbflags[i].token, strlen(dbflags[i].token)) == 0)
            									{
              										_DBPRINTF("Effacement du drapeau '%s' de débogage\n", dbflags[i].token);
										            clearFlag(debug_flags, dbflags[i].flag);
										            break;
            									}
          								}
        						}
        					else
        						{
          							if(strnicmp(s, "ansi", 4) == 0)
          								{
            								_DBPRINTF("Sortie ANSI activée\n");
            								ansi_output = TRUE;
          								}
          							else if(strnicmp(s, "stdout", 6) == 0)
          								{
            								stdout_output = TRUE;
            								_DBPRINTF("Sortie CON: activée\n");
          								}
          							else if(strnicmp(s, "file:", 5) == 0)
          								{
            								char *ee;
            								char *tt = s+5;
            								char filename[256];

            								if((ee = strpbrk(tt, " ,;")) == NULL) strlcpy(filename, tt, sizeof(filename));
            								else strlcpy(filename, tt, ee-tt+1);
            								_DBPRINTF("Sortie FILE '%s' activée\n", filename);
          								}
          							else
          								{
            								int found=0;

            								for(i=0; dbflags[i].token; i++)
            									{
             	 									if(strnicmp(s, dbflags[i].token, strlen(dbflags[i].token)) == 0)
              											{
                											//_DBPRINTF("Mise en place du drapeau '%s' de débogage\n", dbflags[i].token);
											                setFlag(debug_flags, dbflags[i].flag);
											                found=1;
                											break;
              											}
            									}
								            if(found == 0)
            									{
              										if(debug_files[0] != '\0') strlcat(debug_files, " ", sizeof(debug_files));
              										strlcat(debug_files, s, sizeof(debug_files));
              										//_DBPRINTF("fichiers '%s' de débogage ajoutés\n", s);
            									}
          								}
        						}
      					}
      				if(*e) s = ++e;
      				else break;
    			}
  			}
  		else
  			{
        		_DBPRINTF("** %s construction: %s Démarrage **********************\n", bourriquetversion, bourriquetbuildid);
    			_DBPRINTF("Exec version: v%ld.%ld\n", (unsigned long)((struct Library *)SysBase)->lib_Version, (unsigned long)((struct Library *)SysBase)->lib_Revision);
    			_DBPRINTF("Pas de variable ENV:Bourriquetdebug trouvée\n");
  			}
  		_DBPRINTF("Mise en place des classes/drapeaux de débogage (ENV:Bourriquetdebug): %08lx/%08lx\n", debug_classes, debug_flags);
  		_DBPRINTF("Mise en place des fichiers '%s' de débogage\n", var); //debug_files);
  		_DBPRINTF("** Traitement normal suivant ***************************************\n");
  		SetupDbgMalloc();
	}
/****************************************************************************/
void CleanupDebug(void)
	{
  		CleanupDbgMalloc();
  		_DBPRINTF("** Nettoyage du débogage ********************************************\n");
  		if(file_output != NULL)
  			{
    			fclose(file_output);
    			file_output = NULL;
  			}
	}
/****************************************************************************/
#define checkIndentLevel(l) { \
  if(INDENT_LEVEL < l) \
  { \
    if(ansi_output) \
      _DBPRINTF("%s%s:%ld:indent level less than %ld (%ld)%s\n", ANSI_ESC_FG_PURPLE, file, line, l, INDENT_LEVEL, ANSI_ESC_CLR); \
    else \
      _DBPRINTF("%s:%ld:indent level less than %ld (%ld)\n", file, line, l, INDENT_LEVEL); \
  } \
}

/****************************************************************************/
void _ENTER(const unsigned long c, const char *m, const char *file, const unsigned long line, const char *function)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, 0, m, file) == TRUE)
  			{
    			const int threadID = THREAD_ID;

    			if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld: Entrer dans %s%s\n",
                  							ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  							DBC_CTRACE_BGCOLOR DBC_CTRACE_STR ANSI_ESC_CLR DBC_CTRACE_COLOR,
                  							_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, function, ANSI_ESC_CLR);
    				}
    			else _DBPRINTF("%02ld:%s|%s|%s%s:%ld:Entrer dans %s\n", threadID, DBC_CTRACE_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, function);
                checkIndentLevel(0);
  			}
  		INDENT_LEVEL+=1;
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _LEAVE(const unsigned long c, const char *m, const char *file, const unsigned long line, const char *function)
	{
  		THREAD_LOCK;
  		INDENT_LEVEL-=1;
  		if(matchDebugSpec(c, 0, m, file) == TRUE)
  			{
    			const int threadID = THREAD_ID;

				if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld:Laisser %s%s\n",
                  								ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  								DBC_CTRACE_BGCOLOR DBC_CTRACE_STR ANSI_ESC_CLR DBC_CTRACE_COLOR,
                  								_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, function, ANSI_ESC_CLR);
    				}
    			else _DBPRINTF("%02ld:%s|%s|%s%s:%ld:Laisser %s\n", threadID, DBC_CTRACE_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, function);
			    checkIndentLevel(0);
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _RETURN(const unsigned long c, const char *m, const char *file, const unsigned long line, const char *function, unsigned long result)
	{
  		THREAD_LOCK;
  		INDENT_LEVEL-=1;
  		if(matchDebugSpec(c, 0, m, file) == TRUE)
  			{
    			const int threadID = THREAD_ID;

    			if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld:Laisser %s (résultat 0x%08lx, %ld)%s\n",
                  							ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  							DBC_CTRACE_BGCOLOR DBC_CTRACE_STR ANSI_ESC_CLR DBC_CTRACE_COLOR,
                  							_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, function, result, result, ANSI_ESC_CLR);
    				}
    			else	_DBPRINTF("%02ld:%s|%s|%s%s:%ld:Laisser %s (résultat 0x%08lx, %ld)\n", threadID, DBC_CTRACE_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, function, result, result);
     			checkIndentLevel(0);
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _CHECKINDENT(const unsigned long c, const char *file, const unsigned long line, const long level)
	{
  		THREAD_LOCK;
		if(matchDebugSpec(c, 0, NULL, file) == TRUE)  checkIndentLevel(level);
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _SHOWVALUE(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const unsigned long value, const int size, const char *name)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
    			const int threadID = THREAD_ID;
    			const char *fmt;

    			if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld:%s = %ld, 0x",
                  								ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  								DBC_REPORT_BGCOLOR DBC_REPORT_STR ANSI_ESC_CLR DBC_REPORT_COLOR,
                  								_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, name, value);
    				}
    			else	_DBPRINTF("%02ld:%s|%s|%s%s:%ld:%s = %ld, 0x", threadID, DBC_CTRACE_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, name, value);
    		    switch(size)
    				{
      					case 1:
      						fmt = "%02lx";
      						break;
      					case 2:
        					fmt = "%04lx";
     						break;
      					default:
        					fmt = "%08lx";
      						break;
    				}
    			_DBPRINTF(fmt, value);
    			if(size == 1 && value < 256)
    				{
      					if(value < ' ' || (value >= 127 && value < 160)) _DBPRINTF(", '\\x%02lx'", value);
      					else _DBPRINTF(", '%c'", (unsigned char)value);
    				}
    			if(ansi_output) _DBPRINTF("%s\n", ANSI_ESC_CLR);
    			else _DBPRINTF("\n");
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _SHOWPOINTER(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const void *p, const char *name)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
    			const int threadID = THREAD_ID;

    			if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld:%s = ",
                  								ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  								DBC_REPORT_BGCOLOR DBC_REPORT_STR ANSI_ESC_CLR DBC_REPORT_COLOR,
                  								_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, name);
    				}
    			else _DBPRINTF("%02ld:%s|%s|%s%s:%ld:%s = ", threadID, DBC_CTRACE_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, name);
	    		if(p != NULL) _DBPRINTF("0x%08lx", p);
    			else _DBPRINTF("NULL");
    			if(ansi_output) _DBPRINTF("%s\n", ANSI_ESC_CLR);
    			else _DBPRINTF("\n");
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _SHOWSTRING(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const char *string, const char *name)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
    			const int threadID = THREAD_ID;

    			if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld:%s = 0x%08lx \"%s\"%s\n",
                  								ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  								DBC_REPORT_BGCOLOR DBC_REPORT_STR ANSI_ESC_CLR DBC_REPORT_COLOR,
                  								_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, name, (unsigned long)string, string, ANSI_ESC_CLR);
    				}
    			else _DBPRINTF("%02ld:%s|%s|%s%s:%ld:%s = 0x%08lx \"%s\"\n", threadID, DBC_REPORT_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, name, (unsigned long)string, string);
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _SHOWMSG(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const char *msg)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
    			const int threadID = THREAD_ID;

    			if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld:%s%s\n",
                  								ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  								DBC_REPORT_BGCOLOR DBC_REPORT_STR ANSI_ESC_CLR DBC_REPORT_COLOR,
                  								_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, msg, ANSI_ESC_CLR);
    				}
    			else	_DBPRINTF("%02ld:%s|%s|%s%s:%ld:%s\n", threadID, DBC_REPORT_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, msg);
      		}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _SHOWTAGS(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const struct TagItem *tags)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
  	  			int i;
    			struct TagItem *tag;
    			struct TagItem *tstate = (struct TagItem *)tags;
    			const int threadID = THREAD_ID;

    			if(ansi_output)
    				{
      					_DBPRINTF("%s%ldm%02ld:%s%s|%s|%s%s:%ld:liste de balises %08lx%s\n",
                  								ANSI_ESC_BG, (threadID+1)%6, threadID, ANSI_ESC_CLR,
                  								DBC_REPORT_BGCOLOR DBC_REPORT_STR ANSI_ESC_CLR DBC_REPORT_COLOR,
                  								_NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, tags, ANSI_ESC_CLR);
    				}
    			else _DBPRINTF("%02ld:%s|%s|%s%s:%ld:liste de balises %08lx\n", threadID, DBC_REPORT_STR, _NOW(), _INDENT(), (strrchr(file, '/') ? strrchr(file, '/')+1 : file), line, tags);
        		INDENT_LEVEL+=1;
    			i = 0;
    			while((tag = NextTagItem(&tstate)) != NULL)
    				{
      					i++;
      					if(ansi_output) _DBPRINTF("%s%2ld: balise=%08lx donnée=%08lx%s\n", DBC_REPORT_COLOR, i, tag->ti_Tag, tag->ti_Data, ANSI_ESC_CLR);
      					else _DBPRINTF("%2ld: balise=%08lx donnée=%08lx\n", i, tag->ti_Tag, tag->ti_Data);
    				}
    			INDENT_LEVEL-=1;
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _DPRINTF(const unsigned long c, const unsigned long f, const char *m, const char *file, unsigned long line, const char *format, ...)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
    			va_list args;

    			va_start(args, format);
    			_VDPRINTF(c, file, line, format, args);
    			va_end(args);
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _STARTCLOCK(const unsigned long c, const unsigned long f, const char *m,  const char *file, const unsigned long line)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
    			if(timer_level + 1 < (int)ARRAY_SIZE(startTimes))
    				{
      					timer_level++;
      					GetSysTime(TIMEVAL(&startTimes[timer_level]));
    				}
    			else _DPRINTF(DBC_ERROR, DBF_ALWAYS, m, file, line, "déjà %ld horloges en utilisation!", ARRAY_SIZE(startTimes));
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
void _STOPCLOCK(const unsigned long c, const unsigned long f, const char *m, const char *file, const unsigned long line, const char *msg)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(c, f, m, file) == TRUE)
  			{
    			if(timer_level >= 0)
    				{
      					struct TimeVal stopTime;

      					GetSysTime(TIMEVAL(&stopTime));
      					SubTime(TIMEVAL(&stopTime), TIMEVAL(&startTimes[timer_level]));
      					_DPRINTF(DBC_TIMEVAL, f, m, file, line, "L'opération '%s' a prise %ld.%06ld secondes", msg, stopTime.Seconds, stopTime.Microseconds);
      					timer_level--;
    				}
    			else _DPRINTF(DBC_ERROR, DBF_ALWAYS, m, file, line, "Pas d'horloge en utilisation!");
  			}
  		THREAD_UNLOCK;
	}
/****************************************************************************/
#if defined(NO_VARARG_MARCOS)
void D(const unsigned long f, const char *format, ...)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(DBC_DEBUG, f, NULL, NULL) == TRUE)
  			{
    			va_list args;

    			va_start(args, format);
    			_VDPRINTF(DBC_DEBUG, __FILE__, __LINE__, format, args);
    			va_end(args);
  			}
  		THREAD_UNLOCK;
	}
#endif
/****************************************************************************/
#if defined(NO_VARARG_MARCOS)
void E(const unsigned long f, const char *format, ...)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(DBC_ERROR, f, NULL, NULL) == TRUE)
  			{
    			va_list args;

    			va_start(args, format);
    			_VDPRINTF(DBC_ERROR, __FILE__, __LINE__, format, args);
    			va_end(args);
  			}
  		THREAD_UNLOCK;
	}
#endif
/****************************************************************************/
#if defined(NO_VARARG_MARCOS)
void W(const unsigned long f, const char *format, ...)
	{
  		THREAD_LOCK;
  		if(matchDebugSpec(DBC_WARNING, f, NULL, NULL) == TRUE)
  			{
    			va_list args;

    			va_start(args, format);
    			_VDPRINTF(DBC_WARNING, f, __FILE__, __LINE__, format, args);
    			va_end(args);
  			}
  		THREAD_UNLOCK;
	}
#endif
/****************************************************************************/
struct DbgMallocNode
	{
  		struct MinNode node;
  		void *memory;
  		size_t size;
  		const char *file;
  		const char *func;
  		int line;
	};

static struct MinList DbgMallocList[256];
static struct SignalSemaphore DbgMallocListSema;
static ULONG DbgMallocCount;
#define ptr2hash(p)           ((((ULONG)(p)) >> 4) & 0xff)
static struct DbgMallocNode *findDbgMallocNode(const void *ptr)
	{
  		struct DbgMallocNode *result = NULL;
  		struct DbgMallocNode *dmn;

  		IterateList(&DbgMallocList[ptr2hash(ptr)], struct DbgMallocNode *, dmn)
  			{
    			if(dmn->memory == ptr)
    				{
      					result = dmn;
      					break;
    				}
  			}
  		return result;
	}

void _MEMTRACK(const char *file, const int line, const char *func, void *ptr, size_t size)
	{
  		if(isFlagSet(debug_classes, DBC_MTRACK))
  			{
    			if(ptr != NULL && size != 0)
    				{
      					struct DbgMallocNode *dmn;

      					if((dmn = malloc(sizeof(*dmn))) != NULL)
      						{
        						dmn->memory = ptr;
        						dmn->size = size;
        						dmn->file = file;
        						dmn->line = line;
        						dmn->func = func;
        						ObtainSemaphore(&DbgMallocListSema);
        						AddTail((struct List *)&DbgMallocList[ptr2hash(ptr)], (struct Node *)&dmn->node);
        						DbgMallocCount++;
        						ReleaseSemaphore(&DbgMallocListSema);
        						if(isFlagSet(debug_flags, DBF_MTRACK)) _DPRINTF(DBC_DEBUG, DBF_ALWAYS, NULL, file, line, "%s(%d): 0x%08lx", func, size, ptr);
      						}
    				}
    			else _DPRINTF(DBC_WARNING, DBF_ALWAYS, NULL, file, line, "Appel potentiel %s invalide avec retour (0x%08lx, 0x%08lx)", func, ptr, size);
  			}
	}

void _UNMEMTRACK(const char *file, const int line, const void *ptr)
	{
  		if(isFlagSet(debug_classes, DBC_MTRACK) && ptr != NULL)
  			{
    			BOOL success = FALSE;
    			struct DbgMallocNode *dmn;

    			ObtainSemaphore(&DbgMallocListSema);
    			if((dmn = findDbgMallocNode(ptr)) != NULL)
    				{
      					Remove((struct Node *)dmn);
      					free(dmn);
      					DbgMallocCount--;
      					success = TRUE;
    				}
    			if(success == FALSE) _DPRINTF(DBC_WARNING, DBF_ALWAYS, NULL, file, line, "Libération de la zone de mémoire 0x%08lx tentée", ptr);
                else if(isFlagSet(debug_flags, DBF_MTRACK)) _DPRINTF(DBC_DEBUG, DBF_ALWAYS, NULL, file, line, "Libération de 0x%08lx avec succès", ptr);
    			ReleaseSemaphore(&DbgMallocListSema);
  			}
	}

void _FLUSH(void)
	{
  		THREAD_LOCK;
  		if(stdout_output == TRUE) fflush(stdout);
  		else if(file_output != NULL) fflush(file_output);
  		THREAD_UNLOCK;
	}

static void SetupDbgMalloc(void)
	{
  		ENTER();
  		if(isFlagSet(debug_classes, DBC_MTRACK))
  			{
    			ULONG i;

    			for(i = 0; i < ARRAY_SIZE(DbgMallocList); i++) NewMinList(&DbgMallocList[i]);
    			DbgMallocCount = 0;
        		memset(&DbgMallocListSema, 0, sizeof(DbgMallocListSema));
    			InitSemaphore(&DbgMallocListSema);
  			}
  		LEAVE();
	}

static void CleanupDbgMalloc(void)
	{
  		ENTER();
  		if(isFlagSet(debug_classes, DBC_MTRACK))
  			{
    			_DBPRINTF("** Nettoyage du tracé de la mémoire *************************************\n");
    			ObtainSemaphore(&DbgMallocListSema);
    			if(DbgMallocCount != 0)
    				{
      					ULONG i;

      					E(DBF_ALWAYS, "Il y a toujours %ld trace(s) de mémoire non libérée", DbgMallocCount);
      					for(i = 0; i < ARRAY_SIZE(DbgMallocList); i++)
      						{
        						struct DbgMallocNode *dmn;
        						struct DbgMallocNode *next;

        						SafeIterateList(&DbgMallocList[i], struct DbgMallocNode *, dmn, next)
        							{
          								_DPRINTF(DBC_ERROR, DBF_ALWAYS, NULL, dmn->file, dmn->line, "Traçage de la mémoire : 0x%08lx non libérée, taille/type %ld, de la fonction (%s)", dmn->memory, dmn->size, dmn->func);
          								free(dmn);
        							}
        						NewMinList(&DbgMallocList[i]);
      						}
    				}
    			else	D(DBF_ALWAYS, "Tous les traces de mémoire on été libérées correctement");
    			ReleaseSemaphore(&DbgMallocListSema);
 	 		}
  		LEAVE();
	}

void DumpDbgMalloc(void)
	{
  		ENTER();

  		if(isFlagSet(debug_classes, DBC_MTRACK))
  			{
    			ULONG i;

    			ObtainSemaphore(&DbgMallocListSema);
    			D(DBF_ALWAYS, "%ld zone(s) de mémoire tracée(s)", DbgMallocCount);
    			for(i = 0; i < ARRAY_SIZE(DbgMallocList); i++)
    				{
      					struct DbgMallocNode *dmn;;

      					IterateList(&DbgMallocList[i], struct DbgMallocNode *, dmn)
      						{
        						_DPRINTF(DBC_MTRACK, DBF_ALWAYS, NULL, dmn->file, dmn->line, "zone mémoire 0x%08lx, taille/type %ld, fonction (%s)", dmn->memory, dmn->size, dmn->func);
      						}
    				}
    			ReleaseSemaphore(&DbgMallocListSema);
  			}
  		LEAVE();
	}

///

#endif /* DEBUG */
