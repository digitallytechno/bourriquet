/***************************************************************************
 Bourriquet  
 	digitally
 
***************************************************************************/
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <clib/alib_protos.h>

#include <proto/codesets.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/timer.h>
#include <proto/utility.h>

#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <mui/NBalance_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/TheBar_mcc.h>

#include "Extrasrc.h"
#include "SDI_hook.h"
#include "Bourriquet.h"
#include "Global.h"
#include "DashBoard.h"

#include "mui/Classes.h"
#include "mui/BourriquetApplication.h"
#include "mui/ServerList.h"
#include "mui/ResearchList.h"
#include "mui/ImageArea.h"
//#include "mui/Toolbar.h"
#include "mui/MainWindow.h"
#include "mui/Log.h"
#include "mui/ResearchPage.h"
#include "mui/SharedPage.h"
#include "mui/ConfigPage.h"
#include "mui/ServerPage.h"

#include "Server.h"
#include "AppIcon.h"
#include "Busy.h"
#include "FileInfo.h"
#include "Locale.h"
#include "MethodStack.h"
#include "MUIObjects.h"
#include "Threads.h"
#include "Debug.h"
#include "Stringsizes.h"

#include "tcp/Connection.h"
enum 
{ 
	TB_MAIN_CONNECTED = 0,
    TB_MAIN_DISCONNECTED,
    TB_MAIN_SERVER,
    TB_MAIN_TRANSFERT,
    TB_MAIN_RESEARCH,
    TB_MAIN_SHAREDFILES,
    TB_MAIN_PREFS
};

struct MUIS_TheBar_Button *tb;
/***************************************************************************
 Module: Tableau de bord principal
***************************************************************************/
///
/// MA_ChangePageFunc
HOOKPROTONHNO(MA_ChangePageFunc, void, int *arg)
	{
		LONG active;
		
  		ENTER();
		active = xget(G->MA->GUI.GR_PAGE, MUIA_Group_ActivePage);
		D(DBF_STARTUP, "page active %ld", arg[0]); 
		DoMethod(G->MA->GUI.PG_PAGES[active], MUIM_Group_InitChange);
		set(G->MA->GUI.PG_PAGES[active], MUIA_Group_ActivePage, arg[0]);
		DoMethod(G->MA->GUI.PG_PAGES[active], MUIM_Group_ExitChange);
		set(G->MA->GUI.GR_PAGE, MUIA_Group_ActivePage, arg[0]);
  		LEAVE();
	}
MakeHook(MA_ChangePageHook, MA_ChangePageFunc);

///
/// MA_ConnectNowFunc
HOOKPROTONHNO(MA_ConnectNowFunc, void, int *arg)
{
 	struct MA_GUIData *gui = &G->MA->GUI;
	LONG active;
		
  	ENTER();
	active = xget(G->MA->GUI.GR_PAGE, MUIA_Group_ActivePage);
	if(active == 0) DoMethod(gui->LV_SERVERS, MUIM_ServerPage_Connection);
  	LEAVE();
}
MakeHook(MA_ConnectNowHook, MA_ConnectNowFunc);

///
/// MA_ConnectNowFunc
HOOKPROTONHNO(MA_DisConnectNowFunc, void, int *arg)
{
	struct MA_GUIData *gui = &G->MA->GUI;
	BOOL res = DoMethod(gui->TO_TOOLBAR, MUIM_TheBar_GetAttr, 0L, MUIV_TheBar_Attr_Hide);
		
  	ENTER();
	if((G->ConnectionActive == TRUE) && res == TRUE)
		{
			DoMethod(G->MA->GUI.TO_TOOLBAR, MUIM_TheBar_SetAttr, 1L, MUIA_TheBar_Attr_Hide, TRUE);
			DoMethod(G->MA->GUI.TO_TOOLBAR, MUIM_TheBar_SetAttr, 0L, MUIA_TheBar_Attr_Hide, FALSE);
			AbortWorkingThreads();
			G->ConnectionActive = 0;
			//DisconnectFromHost(G->Connection);
			DeleteConnection(G->Connection);
		}
  	LEAVE();
}
MakeHook(MA_DisConnectNowHook, MA_DisConnectNowFunc);
///
/// MA_New
//  Créer la fenêtre principale
struct MA_ClassData *MA_New(void)
	{
  		struct MA_ClassData *data;
		struct MUIS_TheBar_Button buttons[] =
			{
    			{ TB_MAIN_CONNECTED, 0,     GetStr(MSG_DB_CONNECT),                 NULL,                           0, 0, NULL, NULL },
				{ TB_MAIN_DISCONNECTED, 1,     GetStr(MSG_DB_DISCONNECT),                 NULL,                           MUIV_TheBar_ButtonFlag_Hide, 0, NULL, NULL },
				{ MUIV_TheBar_BarSpacer, -1,  NULL, NULL, 0, 0, NULL, NULL },
    			{ TB_MAIN_SERVER,    2,     GetStr(MSG_DB_SERVER),                 NULL,                           0, 0, NULL, NULL },
    			{ TB_MAIN_TRANSFERT,     TB_MAIN_TRANSFERT,     GetStr(MSG_DB_TRANSFER),                 NULL,                           0, 0, NULL, NULL },
    			{ TB_MAIN_RESEARCH,   TB_MAIN_RESEARCH,   GetStr(MSG_DB_RESEARCH),                 NULL,                           0, 0, NULL, NULL },
    			{ TB_MAIN_SHAREDFILES,  TB_MAIN_SHAREDFILES,  GetStr(MSG_DB_SHARED),                 NULL,                           0, 0, NULL, NULL },
				{ MUIV_TheBar_BarSpacer, -1,  NULL, NULL, 0, 0, NULL, NULL },
	    		{ TB_MAIN_PREFS,  TB_MAIN_PREFS,  GetStr(MSG_DB_PREFS),                 NULL,                           0, 0, NULL, NULL },
				{ MUIV_TheBar_End,       -1,  NULL, NULL, 0, 0, NULL, NULL },
			};
		STRPTR GadImg[] =
			{			
    			G->theme.itemImages[SI_CONNECT], 
				G->theme.itemImages[SI_DISCONNECT],
    			G->theme.itemImages[SI_SERVER],
    			G->theme.itemImages[SI_TRANSFERT],
    			G->theme.itemImages[SI_RESEARCH],
    			G->theme.itemImages[SI_SHARED],
    			G->theme.itemImages[SI_PREFS],
    			NULL
			};
	
		ENTER();		
		if((data = calloc(1, sizeof(struct MA_ClassData))) != NULL)
  			{
				data->GUI.MS_MAIN = MenustripObject,
      													MenuChild, data->GUI.MN_PROJECT = MenuObject,
        																														MUIA_Menu_Title, GetStr(MSG_MA_PROJECT),
        																														MUIA_Menu_CopyStrings, FALSE,
        																														MenuChild, Menuitem(GetStr(MSG_MA_ABOUT), "?", TRUE, FALSE, MMEN_ABOUT),
        																														MenuChild, Menuitem(GetStr(MSG_MA_ABOUT_MUI), NULL, TRUE, FALSE, MMEN_ABOUTMUI),
        																														//MenuChild, data->GUI.MI_UPDATECHECK = Menuitem(tr(MSG_MA_UPDATECHECK), NULL, TRUE, FALSE, MMEN_VERSION),
																																MenuChild, MenuBarLabel,
																																MenuChild, Menuitem(GetStr(MSG_HIDE), "H", TRUE, FALSE, MMEN_HIDE),
																																MenuChild, Menuitem(GetStr(MSG_QUIT), "Q", TRUE, FALSE, MMEN_QUIT),
      																														End,
														End;
				snprintf(data->WinTitle, sizeof(data->WinTitle), bourriquetversionstring);
				snprintf(data->ScreenTitle, sizeof(data->ScreenTitle), GetStr(MSG_WINDOW_TITLE), bourriquetver);
    			data->GUI.WI = MainWindowObject,
												MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered,
    											MUIA_Window_TopEdge, MUIV_Window_TopEdge_Centered,
												MUIA_Window_Title, data->WinTitle,
											  	MUIA_Window_ScreenTitle, data->ScreenTitle,
											  	MUIA_HelpNode, "Windows/Mainwindow",
											  	MUIA_Window_ID, MAKE_ID('M','A','I','N'),
											  	MUIA_Window_Menustrip, data->GUI.MS_MAIN,												
											  	WindowContents, data->GUI.GR_MAIN = VGroup, 
																													Child, VGroup,
																																Child, HGroup,
																																			Child, VGroup, 
																																						TextFrame,	
																																						Child, data->GUI.TO_TOOLBAR  = TheBarObject, 
																																																					MUIA_Group_Horiz, FALSE,
																																																					MUIA_TheBar_IgnoreAppearance, TRUE,
																																																					MUIA_TheBar_Borderless, TRUE,
																																																					MUIA_TheBar_Raised, TRUE,
																																																					MUIA_TheBar_Buttons, buttons,
																																																					MUIA_TheBar_Pics, GadImg,
																																																					MUIA_TheBar_NtRaiseActive, TRUE, 
																																																					End,
																																						End,
        																																				Child, data->GUI.GR_PAGE = PageGroup,
          																																																NoFrame,
          																																																MUIA_Group_ActivePage, 0,
          																																																Child,  data->GUI.PG_PAGES[0]  = HGroup,
																																																																	MUIA_Group_Horiz, FALSE,
																																																																	Child, VGroup, 
																																																																				Child, data->GUI.LV_SERVERS = ServerPageObject, End,
																																																																	End,
																																																																	Child, RectangleObject,
																																																																				MUIA_FixHeight, 4,
																																																																				MUIA_Rectangle_HBar, TRUE,
																																																																	End,
																																																																End,
																																																		Child, data->GUI.PG_PAGES[1]   = HGroup,
																																																																	MUIA_Group_Horiz, FALSE,
																																																																	Child, VGroup, 
																																																																				Child, data->GUI.LV_TRANSFERT = TransfertPageObject, End,
																																																																	End,
																																																																	Child, RectangleObject,
																																																																				MUIA_FixHeight, 4,
																																																																				MUIA_Rectangle_HBar, TRUE,
																																																																	End,
																																																																End,
          																																																Child, data->GUI.PG_PAGES[2]   = HGroup,
																																																																	MUIA_Group_Horiz, FALSE,
																																																																	Child, VGroup, 
																																																																					Child, data->GUI.LV_RESEARCH = ResearchPageObject, End,
																																																																	End,
																																																																	Child, RectangleObject,
																																																																					MUIA_FixHeight, 4,
																																																																					MUIA_Rectangle_HBar, TRUE,
																																																																	End,
																																																																End,
																																																		Child, data->GUI.PG_PAGES[3]   = HGroup,
																																																																	MUIA_Group_Horiz, FALSE,
																																																																		Child, VGroup, 
																																																																					Child, data->GUI.LV_SHARED = SharedPageObject, End,
																																																																		End,
																																																																	Child, RectangleObject,
																																																																				MUIA_FixHeight, 4,
																																																																				MUIA_Rectangle_HBar, TRUE,
																																																																	End,
																																																																End,
																																																		Child, data->GUI.PG_PAGES[4]   = HGroup,
																																																																	MUIA_Group_Horiz, FALSE,
																																																																	Child, VGroup, 																																																															
																																																																				Child, data->GUI.LV_CONFIG = ConfigPageObject, End, 
																																																																	End,
																																																																	Child, RectangleObject,
																																																																				MUIA_FixHeight, 4,
																																																																				MUIA_Rectangle_HBar, TRUE,
																																																																	End,
																																																																End,																																																									
        																																																End,
      																																					End,
																																						Child, HGroup,
																																									Child, data->GUI.TXT = TextObject,
																																																			NoFrame,
																																																			MUIA_HorizWeight, 350,
																																																			MUIA_Text_Contents, "Non connecté à un serveur Edonkey",
																																									End,
																																									Child, MakeImageObject("user",  G->theme.itemImages[SI_USER]), 
																																									Child, TextObject,
																																											  NoFrame,
																																											  MUIA_HorizWeight, 150,
																																											  MUIA_Text_Contents, "Utilisateurs | Fichiers",
																																									End,
																																									Child, MakeImageObject("updown",  G->theme.itemImages[SI_UPDOWN]),
																																									Child, TextObject,
																																											 NoFrame,
																																											 MUIA_HorizWeight, 100,
																																											 MUIA_Text_Contents, "DL(0/0) -- (0/0)UL",
																																									End,
																																									Child, MakeImageObject("connectednotnot",  G->theme.itemImages[SI_CONNECTEDNOTNOT]),
																																									Child, TextObject,
																																												NoFrame,
																																												MUIA_Text_Contents, "Non connecté",
																																									End,
																																						End,
																																			End,
    																												End, 
    											TAG_DONE);									
  		 		if(data->GUI.WI != NULL)
    				{	
   						DoMethod(G->App, OM_ADDMEMBER, data->GUI.WI);
						DoMethod(data->GUI.WI, MUIM_Notify, MUIA_Window_MenuAction, MMEN_ABOUT,          MUIV_Notify_Application, 1, MUIM_BourriquetApplication_OpenAboutWindow);
      					DoMethod(data->GUI.WI, MUIM_Notify, MUIA_Window_MenuAction, MMEN_ABOUTMUI,    MUIV_Notify_Application, 2, MUIM_Application_AboutMUI, data->GUI.WI);
      					DoMethod(data->GUI.WI, MUIM_Notify, MUIA_Window_MenuAction, MMEN_QUIT,           	MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
     					DoMethod(data->GUI.WI, MUIM_Notify, MUIA_Window_MenuAction, MMEN_HIDE,           	MUIV_Notify_Application, 3, MUIM_Set, MUIA_Application_Iconified, TRUE);
						DoMethod(data->GUI.WI, MUIM_Notify, MUIA_Window_CloseRequest,  TRUE,  					MUIV_Notify_Application,  2, MUIM_Application_ReturnID, ID_CLOSEALL);
						DoMethod(data->GUI.TO_TOOLBAR, MUIM_TheBar_Notify, TB_MAIN_CONNECTED, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_CallHook, &MA_ConnectNowHook);
						DoMethod(data->GUI.TO_TOOLBAR, MUIM_TheBar_Notify, TB_MAIN_DISCONNECTED, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_CallHook, &MA_DisConnectNowHook);
						DoMethod(data->GUI.TO_TOOLBAR, MUIM_TheBar_Notify, TB_MAIN_SERVER, MUIA_Pressed, FALSE, MUIV_Notify_Application, 3, MUIM_CallHook, &MA_ChangePageHook, cp_Server);
						DoMethod(data->GUI.TO_TOOLBAR, MUIM_TheBar_Notify, TB_MAIN_TRANSFERT, MUIA_Pressed, FALSE, MUIV_Notify_Application, 3, MUIM_CallHook, &MA_ChangePageHook, cp_Transfert);
						DoMethod(data->GUI.TO_TOOLBAR, MUIM_TheBar_Notify, TB_MAIN_RESEARCH, MUIA_Pressed, FALSE, MUIV_Notify_Application, 3, MUIM_CallHook, &MA_ChangePageHook, cp_Research);
						DoMethod(data->GUI.TO_TOOLBAR, MUIM_TheBar_Notify, TB_MAIN_SHAREDFILES, MUIA_Pressed, FALSE, MUIV_Notify_Application, 3, MUIM_CallHook, &MA_ChangePageHook, cp_Shared);
						DoMethod(data->GUI.TO_TOOLBAR, MUIM_TheBar_Notify, TB_MAIN_PREFS, MUIA_Pressed, FALSE, MUIV_Notify_Application, 3, MUIM_CallHook, &MA_ChangePageHook, cp_Prefs);
					}
			}
		else
  			{
    			E(DBF_GUI, "Bourriquet ne peut pas créer l'objet de la fenêtre principale!");
    			free(data);
    			data = NULL;
  			}
  		RETURN(data);
  		return data;
	}

///
