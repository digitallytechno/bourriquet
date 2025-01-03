/***************************************************************************
 Bourriquet 
 	digitally
***************************************************************************/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <exec/execbase.h>
#include <libraries/asl.h>
#include <mui/BetterString_mcc.h>
#include <mui/NBitmap_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NFloattext_mcc.h>
#include <mui/TheBar_mcc.h>
#include <mui/NBalance_mcc.h>
#include <proto/codesets.h>
#include <proto/datatypes.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/layers.h>
#include <proto/locale.h>
#include <proto/muimaster.h>
#include <proto/openurl.h>
#include <proto/rexxsyslib.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <proto/wb.h>

#include "Extrasrc.h"
#include "extrasrc/NewReadArgs.h"
#include "mui/ClassesExtra.h"
#include "mui/ClassesSetup.h"
#include "mui/SplashWindow.h"
#include "mui/BourriquetApplication.h"
#include "mui/Toolbar.h"
#include "mui/ShutdownWindow.h"

#include "tcp/Connection.h"

#include "Bourriquet.h"
#include "Global.h"
#include "MUIObjects.h"
#include "MethodStack.h"
#include "ImageCache.h"
#include "FileInfo.h"
#include "DashBoard.h"
#include "Timer.h"
#include "Threads.h"
#include "Busy.h"
#include "Locale.h"
#include "Config.h"
#include "ServerList.h"
#include "ServerMet.h"
#include "Debug.h"

/***************************************************************************
 Module: Origine
***************************************************************************/

struct Global *G = NULL;

struct Args
{
  char  *prefs;
  char  *iconstart;
  LONG   hide;
  LONG   noImgWarning;
  LONG   noCatalog;
  LONG   noSplashWindow;
};

static struct NewRDArgs nrda;
static struct Args args;
static BPTR olddirlock = (BPTR)-1; /* -1 est un indicateur inutilisé*/
static void Abort(const char *message, ...);

// Pour les sémaphores
static struct StartupSemaphore
{
   	struct SignalSemaphore semaphore; // la structure standard d'une sémaphore
   	ULONG UseCount;                   // nombre de participants qui connaissent cette sémaphore
} *startupSemaphore = NULL;

#define STARTUP_SEMAPHORE_NAME "Bourriquet_startup"

///
/// CheckMCC
//  Vérifier si une certaine version d'une MCC est disponible
static BOOL CheckMCC(const char *name, ULONG minver, ULONG minrev, ULONG maxver, ULONG maxrev, BOOL req, const char *url)
{
	BOOL success = FALSE;
	BOOL flush = TRUE;
	BOOL gotoURLPossible = FALSE;

	ENTER();
	D(DBF_STARTUP, "vérification pour v%ld.%ld+ de '%s'", minver, minrev, name);
   //gotoURLPossible = GotoURLPossible();

  // Savoir si le MCC est disponible et dans les bonnes conditions pour être utilisé
	do
		{
			Object *obj;
			struct MUI_CustomClass *subclass;

    		// Nous essayons d'acquérir la version et la révision à travers MUI par la création d'un objet MCC
			if((obj = MUI_NewObject(name, TAG_DONE)) != NULL)
				{
					ULONG ver = xget(obj, MUIA_Version);
					ULONG rev = xget(obj, MUIA_Revision);

					MUI_DisposeObject(obj);
					obj = NULL;
					D(DBF_STARTUP, "Trouvé %s v%ld.%ld à travers MUIA_Version/Revision", name, ver, rev);
					if(VERSION_IS_AT_LEAST(ver, rev, minver, minrev) == TRUE)
						{
							D(DBF_STARTUP, "trouvé %s à être >= v%ld.%ld", name, minver, minrev);
							if(maxver > 0 && VERSION_IS_LOWER(ver, rev, maxver, maxrev) == FALSE)
								{
									W(DBF_STARTUP, "ATTENTION ! trouvé %s à être >= v%ld.%ld (max)", name, maxver, maxrev);
								}
							else success = TRUE;
						}
				}
			if((subclass = MUI_CreateCustomClass(NULL, name, NULL, 0, NULL)) != NULL)
				{
					D(DBF_STARTUP, "classe personnalisée de test '%s' créée avec succès", name);
					MUI_DeleteCustomClass(subclass);
					subclass = NULL;
				}
			else
				{
					W(DBF_STARTUP, "ATTENTION ! Ne peut pas créer la classe personnalisée '%s' de test", name);
					success = FALSE;
				}

			if(success == FALSE)
				{
					struct Library *base;
					char libname[SIZE_DEFAULT];

					snprintf(libname, sizeof(libname), "mui/%s", name);
					if((base = OpenLibrary(libname, 0)) != NULL)
						{
							ULONG ver = base->lib_Version;
							ULONG rev = base->lib_Revision;
							UWORD openCnt = base->lib_OpenCnt;

							D(DBF_STARTUP, "Ouverture de '%s' avec succès comme bibliothèque, trouvé v%ld.%ld, compteur %ld ouvert" , name, ver, rev, openCnt);
							CloseLibrary(base);
							base = NULL;
							if(VERSION_IS_AT_LEAST(ver, rev, minver, minrev) == TRUE)
								{
									success = TRUE;
									break;
								}
							if(openCnt > 1)
								{
									if(req == TRUE)
										{
											LONG answer;

											answer = MUI_Request(G != NULL ? G->App : NULL, NULL, MUIF_NONE, GetStr(MSG_ERROR_STARTUP), (gotoURLPossible == TRUE) ? GetStr(MSG_RETRY_HOMEPAGE_QUIT_GAD)  : GetStr(MSG_RETRY_QUIT_GAD), GetStr(MSG_MCC_IN_USE), name, minver, minrev, ver, rev, url);
											if(answer == 0) break;
											else if(answer == 1) flush = TRUE;
											else
												{
													//GotoURL(url, FALSE);
													break;
												}
										}
									else break;
								}
							if(flush == TRUE)
								{
									struct Library *result;

									D(DBF_STARTUP, "En essayant de trouver '%s'", name);
									Forbid();
									if((result = (struct Library *)FindName(&((struct ExecBase *)SysBase)->LibList, name)) != NULL) RemLibrary(result);
									Permit();
									flush = FALSE;
								}
							else
								{
									E(DBF_STARTUP, "ERREUR ! N'a pas pu trouvé le minimum requis v%ld.%ld de '%s'", minver, minrev, name);
          							// Pas de chance 
									if(req == TRUE)
										{
											LONG answer;

											answer = MUI_Request(G != NULL ? G->App : NULL, NULL, MUIF_NONE, GetStr(MSG_ERROR_STARTUP), (gotoURLPossible == TRUE) ? GetStr(MSG_RETRY_HOMEPAGE_QUIT_GAD) : GetStr(MSG_RETRY_QUIT_GAD), GetStr(MSG_MCC_OLD), name, minver, minrev, ver, rev, url);
											if(answer == 0)
												{
													// Annuler
													break;
												}
											else if(answer == 1)
												{
													// nettoyer et réessayer
													flush = TRUE;
												}
											else
												{
													// Visiter page d'acceuil du MCC si possible et sortir
													//GotoURL(url, FALSE);
													break;
												}
										}
									else break;
								}
						}
					else
						{
							LONG answer;

							// Pas de MCC du tout - pas nécessaire de nettoyer
							flush = FALSE;
							answer = MUI_Request(G != NULL ? G->App : NULL, NULL, MUIF_NONE, GetStr(MSG_ERROR_STARTUP), (gotoURLPossible == TRUE) ? GetStr(MSG_RETRY_HOMEPAGE_QUIT_GAD) : GetStr(MSG_RETRY_QUIT_GAD), GetStr(MSG_NO_MCC), name, minver, minrev, url);
							if(answer == 0)
								{
									// Annuler
									break;
								}
							else if(answer == 2)
								{
									// Visiter page d'acceuil du MCC si possible et sortir
									//GotoURL(url, FALSE);
									break;
								}
						}
				}
		}
	while(success == FALSE);

	if(success == FALSE && req == TRUE) exit(RETURN_ERROR); // pas cool
	RETURN(success);
	return(success);
}

///

/*** routines de vérification des bibliothèques et MCC ***/
/// InitLib()
static BOOL InitLib(const char *libname, ULONG version, ULONG revision, struct Library **libbase, BOOL required, const char *homepage)
{
    struct Library *base = NULL;

    ENTER();
    if(libbase != NULL)
        {
            base = OpenLibrary(libname, version);
            if(base != NULL && revision != 0)
                {
                    if(LIB_VERSION_IS_AT_LEAST(base, version, revision) == FALSE)
                        {
                            CloseLibrary(base);
                            base = NULL;
                        }
                }
			if(base != NULL) D(DBF_STARTUP, "Bibliothéque %s v%ld.%ld ouverte avec succes.", libname, base->lib_Version, base->lib_Revision);
        }
	else D(DBF_STARTUP, "Ne peut pas ouvrir la bibliothéque %s avec la version minimum v%ld.%ld", libname, version, revision);
    if(base == NULL && required == TRUE)
        {
            if(homepage != NULL)
                {
                    char error[SIZE_LINE];
					BOOL gotoURLPossible = FALSE; //GotoURLPossible();
                    LONG answer;

					snprintf(error, sizeof(error), GetStr(MSG_ERROR_LIB_URL), libname, version, revision, homepage);
                    if(MUIMasterBase != NULL && G != NULL && G->App != NULL)
                        {
							answer = MUI_Request(G->App, NULL, 0, GetStr(MSG_ERROR_STARTUP), (gotoURLPossible == TRUE) ? GetStr(MSG_RETRY_HOMEPAGE_QUIT_GAD) : GetStr(MSG_QUIT), error);
                        }
                    else if(IntuitionBase != NULL)
                        {
                            struct EasyStruct ErrReq;

                            ErrReq.es_StructSize   = sizeof(struct EasyStruct);
                            ErrReq.es_Flags        = 0;
							ErrReq.es_Title        = (STRPTR)GetStr(MSG_ERROR_STARTUP);
							ErrReq.es_TextFormat   = error;
							ErrReq.es_GadgetFormat = (gotoURLPossible == TRUE) ? (STRPTR)GetStr(MSG_RETRY_HOMEPAGE_QUIT_GAD) : (STRPTR)GetStr(MSG_QUIT);
                            answer = EasyRequestArgs(NULL, &ErrReq, NULL, NULL);
                        }
                    else
                        {
                            puts(error);
                            answer = 0;
                        }   
		    		if(answer == 1) D(DBF_STARTUP, "GotoURL()"); //GotoURL(homepage, FALSE);
                    Abort(NULL);
                }
		else Abort(GetStr(MSG_ERROR_LIB), libname, version, revision);
        }
    *libbase = base;
    RETURN((BOOL)(base != NULL));
    return((BOOL)(base != NULL));
}

static struct StartupSemaphore *CreateStartupSemaphore(void)
{
    struct StartupSemaphore *semaphore;

    ENTER();
	D(DBF_STARTUP, "création d'un sémaphore de démarrage...");
    Forbid();
    if((semaphore = (struct StartupSemaphore *)FindSemaphore((STRPTR)STARTUP_SEMAPHORE_NAME)) != NULL) semaphore->UseCount++;
    Permit();
    if(semaphore == NULL)
        {
            if((semaphore = AllocSysObjectTags(ASOT_SEMAPHORE,
                                                ASOSEM_Size, sizeof(*semaphore),
                                                ASOSEM_Name, (ULONG)STARTUP_SEMAPHORE_NAME,
                                                ASOSEM_CopyName, TRUE,
                                                ASOSEM_Public, TRUE,
                                                TAG_DONE)) != NULL) semaphore->UseCount = 1;
        }
    RETURN(semaphore);
    return(semaphore);
}

static void DeleteStartupSemaphore(void)
{
    ENTER();
    if(startupSemaphore != NULL)
        {
            ObtainSemaphore(&startupSemaphore->semaphore);
            Forbid();
            ReleaseSemaphore(&startupSemaphore->semaphore);
            startupSemaphore->UseCount--;
            if(startupSemaphore->UseCount == 0)
                {
                    FreeSysObject(ASOT_SEMAPHORE, startupSemaphore);
                    startupSemaphore = NULL;
					D(DBF_STARTUP, "libération du semaphore de demarrage avec freesysobject(...)");
                }
            Permit();
			D(DBF_STARTUP, "libération du semaphore de démarrage...");
        }
    LEAVE();
}

static void Terminate(void)
{
    ENTER();
    G->Terminating = TRUE;
	D(DBF_STARTUP, "Interruption de toutes les tâches de travail...");
	AbortWorkingThreads();
	D(DBF_STARTUP, "nettoyage du système de tâches...");
	CleanupThreads();
	D(DBF_STARTUP, "nettoyage du module de la configuration...");
	FreeConfig(CE);
  	CE = NULL;
	if(G->MA != NULL) set(G->MA->GUI.WI, MUIA_Window_Open, FALSE);
	D(DBF_STARTUP, "Libération du module de la fenêtre principale...");
  	if(G->MA != NULL) DisposeModule(&G->MA);
  	D(DBF_STARTUP, "libération des ressources de minuterie...");
	CleanupTimers();
	D(DBF_STARTUP, "libération de la liste privée codesets...");
	if(G->codesetsList != NULL)
		{
			CodesetsListDelete(CSA_CodesetList, G->codesetsList, TAG_DONE);
			G->codesetsList = NULL;
		}
	D(DBF_STARTUP, "Libération de l'objet de l'application principale...");
    if(G->App != NULL)
        {
            MUI_DisposeObject(G->App);
            G->App = NULL;
        }
	D(DBF_STARTUP, "Libération de la configuration...");
  	FreeConfig(C);
  	C = NULL;
	D(DBF_STARTUP, "Libération des images du thême...");
 	FreeTheme(&G->theme);
    D(DBF_STARTUP, "Libération du cache d'image...");
  	ImageCacheCleanup();
    Bourriquet_CleanupClasses();
	D(DBF_STARTUP, "Libération de la semaphore...");
	DeleteStartupSemaphore();
	D(DBF_STARTUP, "Nettoyage de l'empilement des procédés...");
    CleanupMethodStack();
	D(DBF_STARTUP, "Nettoyage du stuff de la connexion...");
  	CleanupConnections();
	D(DBF_STARTUP, "Suppression des actions en attente...");
    D(DBF_STARTUP, "Fermeture de toutes les bibliothéques ouvertes...");
    CLOSELIB(CyberGfxBase,    ICyberGfx);
    CLOSELIB(OpenURLBase,     IOpenURL);
    CLOSELIB(DataTypesBase,   IDataTypes);
    CLOSELIB(MUIMasterBase,   IMUIMaster);
    CLOSELIB(IFFParseBase,    IIFFParse);
    CLOSELIB(KeymapBase,      IKeymap);
    CLOSELIB(LayersBase,      ILayers);
    CLOSELIB(WorkbenchBase,   IWorkbench);
    CLOSELIB(GfxBase,         IGraphics);
    D(DBF_STARTUP, "Fermeture du catalogue...");
    CloseCatalogue();
    if(G->Locale != NULL)
      {
        CloseLocale(G->Locale);
        G->Locale = NULL;
      }
    CLOSELIB(CodesetsBase, ICodesets);
    CLOSELIB(LocaleBase, ILocale);
	// Libére la sémaphore globale
  	if(G->globalSemaphore != NULL)
  		{
    		FreeSysObject(ASOT_SEMAPHORE, G->globalSemaphore);
    		G->globalSemaphore = NULL;
  		}
  	if(G->ServerMet != NULL)
		{
    		if(IsServerListEmpty(G->ServerMet) != FALSE)  E(DBF_NET, "there are still %ld server", G->ServerMet->count);
    		DeleteServerList(G->ServerMet);
    		G->ServerMet = NULL;
  		}		
	if(G->lowMemHandler != NULL)
  		{
    		RemMemHandler(G->lowMemHandler);
    		FreeSysObject(ASOT_INTERRUPT, G->lowMemHandler);
    		G->lowMemHandler = NULL;
  		}
  	if(G->SharedMemPool != NULL)
  		{
    		FreeSysObject(ASOT_MEMPOOL, G->SharedMemPool);
    		G->SharedMemPool = NULL;
  		}
  	// enfin et surtout libération de la structure globale G
    if (G != NULL)
     {
        free(G);
        G = NULL;
     }
    LEAVE();
}

static void Abort(const char *message, ...)
{
    ENTER();
    if(message != NULL)
        {
            va_list a;
            char error[SIZE_LINE];

            va_start(a, message);
            vsnprintf(error, sizeof(error), message, a);
            va_end(a);
			W(DBF_STARTUP, "ATTENTION ! Interruption de l'application due à la raison '%s'", error);
            if(MUIMasterBase != NULL && G != NULL && G->App != NULL)
              {
				MUI_Request(G->App, NULL, 0, GetStr(MSG_ERROR_STARTUP), GetStr(MSG_QUIT), error);
              }
            else if(IntuitionBase != NULL)
              {
                struct EasyStruct ErrReq;

                ErrReq.es_StructSize   = sizeof(struct EasyStruct);
                ErrReq.es_Flags        = 0;
				ErrReq.es_Title        = (STRPTR)GetStr(MSG_ERROR_STARTUP);
                ErrReq.es_TextFormat   = error;
				ErrReq.es_GadgetFormat = (STRPTR)GetStr(MSG_QUIT);
                EasyRequestArgs(NULL, &ErrReq, NULL, NULL);
              }
            else puts(error);
        }
    else W(DBF_STARTUP, "ATTENTION! Abandon de l'application");
    exit(RETURN_ERROR);
    LEAVE();
}

static void bourriquet_exitfunc(void)
{
    ENTER();
	D(DBF_STARTUP, "Nettoyage...");
    if(olddirlock != (BPTR)-1)
        {
            Terminate();
            CurrentDir(olddirlock);
        }
    if(nrda.Template != NULL) NewFreeArgs(&nrda);
    CLOSELIB(DiskfontBase,   IDiskfont);
    CLOSELIB(UtilityBase,    IUtility);
    CLOSELIB(IconBase,       IIcon);
    CLOSELIB(IntuitionBase,  IIntuition);
    LEAVE();

    #if defined(DEBUG)
        CleanupDebug();
    #endif
}

static void SplashProgress(const char *txt, int percent)
{
    ENTER();
	D(DBF_STARTUP, "Progression %s, %ld", txt, percent);
	DoMethod(G->SplashWinObject, MUIM_SplashWindow_StatusChange, txt, percent);
    LEAVE();
}

void PopUp(void)
{
    ENTER();
    set(G->App, MUIA_Application_Iconified, FALSE);
    if(G->InStartupPhase == FALSE)
        {
	    	Object *window = G->MA->GUI.WI;

            if(xget(window, MUIA_Window_Open) == FALSE) set(window, MUIA_Window_Open, TRUE);
            DoMethod(window, MUIM_Window_ScreenToFront);
            DoMethod(window, MUIM_Window_ToFront);
			set(window, MUIA_Window_Activate, TRUE); 
        }
    LEAVE();
}

BOOL StayInProg(void)
{
	BOOL stayIn = FALSE;

    ENTER();
  	if(stayIn == FALSE)
  		{
    		if(MUI_Request(G->App, G->DashBoardWin, MUIF_NONE, GetStr(MSG_CONFIRM_REQ), GetStr(MSG_YES_NO_REQ), GetStr(MSG_QuitANGReq)) == 0) stayIn = TRUE;
    	}
    RETURN(stayIn);
    return(stayIn);
}

static int Root_GlobalDispatcher(ULONG app_input)
{
    int ret = 0;

    switch(app_input)
        {
            case MUIV_Application_ReturnID_Quit:
                {
                    if(xget(G->App, MUIA_Application_ForceQuit) == FALSE) ret = StayInProg() ? 0 : 1;
                    else ret = 1;
                }
            break;
            case ID_CLOSEALL:
				D(DBF_STARTUP, "ID_CLOSEALL");
				ret = StayInProg() ? 0 : 1;
				break;
            case ID_RESTART:
				D(DBF_STARTUP, "ID_RESTART");
				ret = StayInProg() ? 0 : 2;
				break;
			case ID_RESTART_FORCE:
				D(DBF_STARTUP, "ID_RESTART_FORCE");
				ret = 2;
				break;
        }
    return(ret);
}

///
/// Root_New
//  Creation de l'application MUI
static BOOL Root_New(BOOL hidden)
{
    BOOL result = FALSE;

    ENTER();
	ObtainSemaphore(&startupSemaphore->semaphore);
	if((G->App = BourriquetApplicationObject,
							MUIA_Application_SingleTask, G->SingleTask,
							MUIA_BourriquetApplication_Hidden, hidden,
							End) != NULL)
		{
            if((G->SplashWinObject = SplashWindowObject, End) != NULL)
                {
                    G->InStartupPhase = TRUE;
                    set(G->SplashWinObject, MUIA_Window_Open, !(hidden || args.noSplashWindow));
                    result = TRUE;
                }
			else W(DBF_STARTUP, "ATTENTION ! Bourriquet ne peut pas créer l'objet de la fenêtre de démarrage !");
		}
	else W(DBF_STARTUP, "ATTENTION ! Bourriquet ne peut pas créer l'objet de la racine !");
	ReleaseSemaphore(&startupSemaphore->semaphore);
    RETURN(result);
    return(result);
}

///

/// InitAfterLogin
//  Phase 2 du programme d'initialisation
static void InitAfterLogin(void)
{
    BOOL splashWasActive;
    char pubScreenName[MAXPUBSCREENNAME + 1];
    struct Screen *pubScreen;
	int res;
	
    ENTER();
	D(DBF_STARTUP, "Chargement de la configuration...");
  	SplashProgress(GetStr(MSG_LOADING_CONFIG), 42);
  	res = LoadConfig(C, "PROGDIR:.config");
  	if(res == 0)Abort(NULL); 
	D(DBF_STARTUP, "Chargement des graphismes...");
	SplashProgress(GetStr(MSG_LOADING_GFX), 56);
	// Chargement du thême choisi par l'utilisateur (pour le moment c'est celui par défaut)
  	LoadTheme(&G->theme, C->ThemeName); //"default");
	D(DBF_STARTUP, "Création de l'interface utilisateur...");
	SplashProgress(GetStr(MSG_CREATING_GUI), 70);
   	// Creation de la fenêtre principale 
  	if((G->MA = MA_New()) == NULL) Abort(GetStr(MSG_ERROR_MUI_APPLICATION)); 
  	D(DBF_STARTUP, "Chargement du fichier server.met...");
	SplashProgress("server.met", 84);
	ParseServerMetFile("PROGDIR:server.met");
	G->InStartupPhase = FALSE;
	GetPubScreenName((struct Screen *)xget(G->SplashWinObject, MUIA_Window_Screen), pubScreenName, sizeof(pubScreenName));
	pubScreen = LockPubScreen(pubScreenName);
	SplashProgress("",100);
	splashWasActive = (args.noSplashWindow) ? TRUE : xget(G->SplashWinObject, MUIA_Window_Activate);
    set(G->SplashWinObject, MUIA_Window_Open, FALSE);
	DoMethod(G->App, OM_REMMEMBER, G->SplashWinObject);
    MUI_DisposeObject(G->SplashWinObject);
    G->SplashWinObject = NULL;
	xset(G->MA->GUI.WI, MUIA_Window_Activate, splashWasActive, MUIA_Window_Open, TRUE);
	UnlockPubScreen(pubScreenName, pubScreen);
    LEAVE();
}

///
/// InitBeforeLogin
//  Phase 1 du programme d' initialisation 
static void InitBeforeLogin(BOOL hidden)
{
    const char *failSuperClass;
    const char *failClass;
	//int i;

    ENTER();
    DateStamp(&G->StartDate);
    srand((unsigned int)GetDateStamp());
    if(INITLIB("locale.library", 53, 6, &LocaleBase, "main", 1, &ILocale, TRUE, NULL)) G->Locale = OpenLocale(NULL);
	INITLIB("codesets.library", 6, 21, &CodesetsBase,  "main", 1, &ICodesets,  TRUE, NULL);
    if(OpenCatalogue() == FALSE) Abort(NULL);
    INITLIB("graphics.library",      51, 14, &GfxBase,       "main", 1, &IGraphics,  TRUE,  NULL);
    INITLIB("layers.library",        52, 59, &LayersBase,    "main", 1, &ILayers,    TRUE,  NULL);
    INITLIB("workbench.library",     36, 0, &WorkbenchBase, "main", 1, &IWorkbench, TRUE,  NULL);
    INITLIB("keymap.library",        51, 5, &KeymapBase,    "main", 1, &IKeymap,    TRUE,  NULL);
    INITLIB("iffparse.library",      51, 4, &IFFParseBase,  "main", 1, &IIFFParse,  TRUE,  NULL);
    INITLIB("datatypes.library",     51, 4, &DataTypesBase, "main", 1, &IDataTypes, TRUE,  NULL);
    INITLIB("cybergraphics.library", 51, 14, &CyberGfxBase,  "main", 1, &ICyberGfx,  FALSE, NULL);
    INITLIB("muimaster.library",     22, 3, &MUIMasterBase, "main", 1, &IMUIMaster, TRUE, "http://muidev.de/");
    INITLIB("openurl.library",        8, 3, &OpenURLBase,   "main", 1, &IOpenURL,   FALSE, NULL);
  	if(InitConnections() == FALSE) Abort(GetStr("erreur dans la mise en place de la connexion"));
	if(InitMethodStack() == FALSE) Abort(GetStr(MSG_ERROR_METHODSTASK));
	if(InitThreads() == FALSE) Abort(GetStr(MSG_ERROR_THREADS));
    CheckMCC(MUIC_TheBar,       26,  22,   0,   0, TRUE, "https://github.com/amiga-mui/thebar");
	CheckMCC(MUIC_TheBarVirt,   26,  22,   0,   0, TRUE, "https://github.com/amiga-mui/thebar");
	CheckMCC(MUIC_TheButton,    26,  22,   0,   0, TRUE, "https://github.com/amiga-mui/thebar");
	CheckMCC(MUIC_BetterString, 11,  28,  30,   0, TRUE, "https://github.com/amiga-mui/betterstring");
	CheckMCC(MUIC_NList,        20, 147,   0,   0, TRUE, "https://github.com/amiga-mui/nlist");
	CheckMCC(MUIC_NListview,    19,  102,   0,   0, TRUE, "https://github.com/amiga-mui/nlist");
	CheckMCC(MUIC_NFloattext,   19,  83,   0,   0, TRUE, "https://github.com/amiga-mui/nlist");
	CheckMCC(MUIC_NListtree,    18,  54,   0,   0, TRUE, "https://github.com/amiga-mui/nlist");
	CheckMCC(MUIC_NBalance,     15,  28,   0,   0, TRUE, "https://github.com/amiga-mui/nlist");
	CheckMCC(MUIC_NBitmap,      15,  32,   0,   0, TRUE, "https://github.com/amiga-mui/nlist");
	G->codesetsList = CodesetsListCreateA(NULL);
	// prend le codeset par défaut du systême
	G->systemCodeset = CodesetsFindA(NULL, NULL);
    // création d'une sémaphore publique qui pourra être utilisée comme un seul thread pour certaines actions
	if((startupSemaphore = CreateStartupSemaphore()) == NULL) Abort(GetStr(MSG_ERROR_CREATE_SEMAPHORE));
    Forbid();
    G->DefIconsAvailable = (FindPort((APTR)"DEFICONS") != NULL || FindPort((APTR)"AMBIENT") != NULL);
    Permit();
	if(Bourriquet_SetupClasses(&failClass, &failSuperClass) == FALSE) Abort(GetStr(MSG_ERROR_CLASSES), failClass, failSuperClass);
    G->SingleTask = TRUE;
	G->ConnectionActive = 0;
  	// alloue l'objet MUI principal et la fenêtre popup pour la progression / au sujet de
	D(DBF_STARTUP, "creation de l'objet principal...");
    if(Root_New(hidden) == FALSE)
       {
			BOOL active;

			Forbid();
			active = (FindPort((APTR)"Bourriquet") != NULL);
			Permit();
			Abort(active ? NULL : GetStr(MSG_ERROR_MUI_APPLICATION));
       }
	SplashProgress(GetStr(MSG_INIT_TIMER), 14);
	if(InitTimers() == FALSE) Abort(GetStr(MSG_ERROR_TIMER));
	SplashProgress(GetStr(MSG_INIT_LIBS), 28);
    LEAVE();
}

/*** Routines d'analyse des arguments de la ligne de commande ***/
/// ParseCommandArgs
//
static LONG ParseCommandArgs(void)
{
    LONG result = 0;
    char *extHelp;

    ENTER();
  	// efface la structure args
    memset(&args, 0, sizeof(args));
    memset(&nrda, 0, sizeof(nrda));
  	// met  le modèle d'argument
	nrda.Template = (STRPTR)"MET/K";
  	// Nous construisons une page texte d'aide
	if((asprintf(&extHelp, "%s (%s)\n%s\n\nUsage: Bourriquet <options>\nOptions/Tooltypes:\n"
						   "  MET=<filename>: fichier de server qui sera utilisé à\n"
						   "                    la place de celui par défaut.\n"
						   "%s:", bourriquetversion, bourriquetversiondate, bourriquetcopyright, nrda.Template)) != -1)
		{
    // met le pointeur extHelp 
			nrda.ExtHelp = (STRPTR)extHelp;
    // met le reste des éléments de la nouvelle structure args à lire
			nrda.Window = NULL;
			nrda.Parameters = (APTR)&args;
			nrda.FileParameter = -1;
			nrda.PrgToolTypesOnly = FALSE;
    // appel de NewReadArgs pour analyser toutes nos arguments de ligne de commande/tooltype en conformité
    // au modèle ci-dessus
			result = NewReadArgs(WBmsg, &nrda);
			free(extHelp);
			nrda.ExtHelp = NULL;
		}
	else E(DBF_STARTUP, "asprintf() a retourné -1");
    RETURN(result);
    return(result);
}

///
/// LowMemHandler
// gestionnaire de mémoire faible pour vider tous les index des dossiers
static LONG ASM LowMemHandler(REG(a6, UNUSED struct ExecBase *ExecBase), REG(a0, UNUSED struct MemHandlerData *memHandlerData), REG(a1, APTR bourriquet))
{
 	ENTER();
    if(G->LowMemSituation == FALSE)
  		{
    		G->LowMemSituation = TRUE;
    		Signal(bourriquet, SIGBREAKF_CTRL_E);
  		}
  	RETURN(MEM_ALL_DONE);
  	return MEM_ALL_DONE;
}

/*** fonction d'entrée principale ***/
/// main
//  point d'entré du programme, boucle principale
int main(int argc, char **argv)
{
	BOOL active = FALSE;
    BPTR progdir;
    LONG err;

  // initialise notre systéme de débogage (mise au point)
	#if defined(DEBUG)
		SetupDebug();
	#endif 
	InitCatalogue();
	atexit(bourriquet_exitfunc);
	WBmsg = (struct WBStartup *)(0 == argc ? argv : NULL);
    INITLIB("intuition.library", 60, 25, &IntuitionBase, "main", 1, &IIntuition, TRUE, NULL);
    INITLIB("icon.library",      41, 41, &IconBase,      "main", 1, &IIcon,      TRUE, NULL);
    INITLIB("utility.library",   51,  5, &UtilityBase,   "main", 1, &IUtility,   TRUE, NULL);
    INITLIB("diskfont.library",  50, 20, &DiskfontBase,  "main", 1, &IDiskfont,  TRUE, NULL);
	if((err = ParseCommandArgs()) != 0)
		{
			PrintFault(err, "BOURRIQUET");
			SetIoErr(err);
			exit(RETURN_ERROR);
		}
  	//  par sécurité seulement, peut arriver pour les résidents seulement
    if((progdir = GetProgramDir()) == ZERO) exit(RETURN_ERROR);
    olddirlock = CurrentDir(progdir);
	for(active=TRUE;;)
	  {
			ULONG signals;
			ULONG timerSig;
			ULONG appSig;
			ULONG threadSig;
			ULONG wakeupSig;
			ULONG methodStackSig;
			int ret;
		
			if((G = calloc(1, sizeof(*G))) == NULL)
				{
					W(DBF_STARTUP, "ATTENTION ! G est out ! ..");
					break;
				}
			if((C = AllocConfig()) == NULL)
	    		{
	      			W(DBF_STARTUP, "ATTENTION ! C est out ! ..");
	      			break;
	    		}
	    	if((G->SharedMemPool = AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags,    MEMF_SHARED|MEMF_CLEAR,
																									ASOPOOL_Puddle,    2048,
																									ASOPOOL_Threshold, 1024,
																						  			ASOPOOL_Name, (ULONG)"Bourriquet shared pool",
																						  			ASOPOOL_LockMem, FALSE,
      																								TAG_DONE)) == NULL)
    			{
      				break;
    			}
			if((G->globalSemaphore = AllocSysObjectTags(ASOT_SEMAPHORE, TAG_DONE)) == NULL)
    			{
      				break;
    			}
			if((G->ServerMet = CreateServerList()) == NULL)
    			{
      				break;
    			}
			if((G->lowMemHandler = AllocSysObjectTags(ASOT_INTERRUPT,
																								  ASOINTR_Code, (ULONG)LowMemHandler,
																								  ASOINTR_Data, (ULONG)FindTask(NULL),
																								  TAG_DONE)) != NULL)
    			{
      				G->lowMemHandler->is_Node.ln_Pri = 50;
      				G->lowMemHandler->is_Node.ln_Name = (STRPTR)"Bourriquet index lowmem";
      				AddMemHandler(G->lowMemHandler);
    			}
    		else
    			{
      				break;
    			}
        	NewMinList(&G->normalBusyList);	
        	NameFromLock(progdir, G->ProgDir, sizeof(G->ProgDir));
        	if(WBmsg != NULL && WBmsg->sm_NumArgs > 0)
        	  {
        	    strlcpy(G->ProgName, (char *)WBmsg->sm_ArgList[0].wa_Name, sizeof(G->ProgName));
        	  }
			else
        	  {
        	    char buf[SIZE_PATHFILE];
	
        	    GetProgramName((STRPTR)&buf[0], sizeof(buf));
        	    strlcpy(G->ProgName, (char *)FilePart(buf), sizeof(G->ProgName));
        	  }
			AddPath(G->ThemesDir, G->ProgDir, "resources/themes", sizeof(G->ThemesDir));
			D(DBF_STARTUP, "ThemesDir: '%s'", G->ThemesDir);
    		if(FileExists(G->ThemesDir) == FALSE)
    			{
      				char deprecatedPath[SIZE_PATH];
	
      				AddPath(deprecatedPath, G->ProgDir, "themes", sizeof(deprecatedPath));
      				if(FileExists(deprecatedPath) == TRUE)  strlcpy(G->ThemesDir, deprecatedPath, sizeof(G->ThemesDir));
					D(DBF_STARTUP, "ThemesDir: '%s'", G->ThemesDir);
    			}
        	G->NoCatalogTranslation = args.noCatalog ? TRUE : FALSE;
			G->NoImageWarning = TRUE; 
			ImageCacheSetup();
			if(active == TRUE)
				{
					InitBeforeLogin(args.hide ? TRUE : FALSE);
					InitAfterLogin();
				}
			else
				{
					InitBeforeLogin(FALSE);
					InitAfterLogin();
				}
			if(active == TRUE) active = FALSE;
        	// prépare tous les bits de signaux
			timerSig          = (1UL << G->timerData.port->mp_SigBit);
			appSig            = (1UL << G->AppPort->mp_SigBit);
			threadSig         = (1UL << G->threadPort->mp_SigBit);
			methodStackSig    = (1UL << G->methodStack->mp_SigBit);
			wakeupSig         = (1UL << ThreadWakeupSignal());
			signals = 0;
			ret = 1;
			while((ret = Root_GlobalDispatcher(DoMethod(G->App, MUIM_Application_NewInput, &signals))) == 0)
				{
					if(signals != 0)
						{
							signals = Wait(signals | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_D | timerSig | appSig | threadSig | wakeupSig | methodStackSig);
							if(isFlagSet(signals, SIGBREAKF_CTRL_C))
								{
									ret = 1;
									break;
								}
							if(isFlagSet(signals, SIGBREAKF_CTRL_D))
								{
									ret = 0;
									break;
								}				
							if(isFlagSet(signals, SIGBREAKF_CTRL_F)) PopUp();
							if(isFlagSet(signals, methodStackSig)) CheckMethodStack();
							if(isFlagSet(signals, timerSig))
        						{
#if defined(DEBUG)
          							char dateString[64];

          							DateStamp2String(dateString, sizeof(dateString), NULL, DSS_DATETIME, TZC_NONE);
          							D(DBF_TIMER, "timer signal received @ %s", dateString);
#endif   
									// appeler ProcessTimerEvent() pour vérifier que toutes
	          						// nos minuteries sont traitées en conséquence.
	          						ProcessTimerEvent();
	       			 			}
							if(isFlagSet(signals, threadSig)) HandleThreads(TRUE);        
						}
				}	 
			if(ret == 1)
				{
					if(xget(G->App, MUIA_Application_Iconified) == FALSE && args.noSplashWindow == FALSE)  ShutdownWindowObject, End;
					SetIoErr(RETURN_OK);
					exit(RETURN_OK);
				}
			D(DBF_STARTUP, "Relance émise");
			Terminate();
	  	}
    SetIoErr(RETURN_OK);
   	return(RETURN_OK);
}

///
/// MiniMainLoop
// une version "dépouillée au strict minimum" de la boucle principale
// va être utilisé dans des situations où nous avons à attendre des événements spécifiques
void MiniMainLoop(void)
{
	ULONG signals;
	ULONG threadSig;
	ULONG wakeupSig;
	ULONG methodStackSig;

	ENTER();
	threadSig      = (1UL << G->threadPort->mp_SigBit);
	wakeupSig      = (1UL << ThreadWakeupSignal());
	methodStackSig = (1UL << G->methodStack->mp_SigBit);
	D(DBF_STARTUP, " Signaux bourriquet alloués:");
	D(DBF_STARTUP, " threadSig         = %08lx", threadSig);
	D(DBF_STARTUP, " wakeupSig         = %08lx", wakeupSig);
	D(DBF_STARTUP, " methodStackSig    = %08lx", methodStackSig);

  // démarre la boucle d'événements
	signals = 0;
	while((LONG)DoMethod(G->App, MUIM_Application_NewInput, &signals) != MUIV_Application_ReturnID_Quit)
		{
			if(signals != 0)
				{
					signals = Wait(signals | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F | threadSig | wakeupSig | methodStackSig);
					if(isFlagSet(signals, SIGBREAKF_CTRL_C)) break;
					if(isFlagSet(signals, SIGBREAKF_CTRL_F)) PopUp();
					if(isFlagSet(signals, methodStackSig)) CheckMethodStack();
					if(isFlagSet(signals, threadSig)) HandleThreads(TRUE);
					if(isFlagSet(signals, wakeupSig)) D(DBF_STARTUP, "allons réveiller le signal");
      			}
		}
	LEAVE();
}

///
/// MicroMainLoop
// une version encore plus "dépouillée au strict minimum" de la  boucle principale
// pour être utilisé dans des situations où nous avons à sonder des événements basiques spécifiques
void MicroMainLoop(void)
{
	ULONG signals;
	ULONG threadSig;
	ULONG methodStackSig;

	ENTER();
	threadSig = (1UL << G->threadPort->mp_SigBit);
	if(G->methodStack != NULL) methodStackSig = (1UL << G->methodStack->mp_SigBit);
	else methodStackSig = 0L;
	D(DBF_STARTUP, " Signaux bourriquet alloués:");
	D(DBF_STARTUP, " threadSig         = %08lx", threadSig);
	D(DBF_STARTUP, " methodStackSig    = %08lx", methodStackSig);
	SetSignal(0UL, threadSig|methodStackSig);
	if(G->methodStack != NULL) CheckMethodStack();
	HandleThreads(TRUE);
	if(G->App != NULL) DoMethod(G->App, MUIM_Application_NewInput, &signals);
	LEAVE();
}   
