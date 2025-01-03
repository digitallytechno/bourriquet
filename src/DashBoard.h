/***************************************************************************
 Bourriquet 
 	digitally
  
***************************************************************************/
#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <intuition/classes.h>
#include "Stringsizes.h"

// dénombrements du menu de la fenêtre principale 
enum
	{
  		MMEN_ABOUT=100,
		MMEN_ABOUTMUI,
		MMEN_VERSION,
		MMEN_HIDE,
		MMEN_QUIT,
		MMEN_MUI
	};

struct MA_GUIData
	{
  		Object *WI;
  		Object *MN_PROJECT;
  		Object *MS_MAIN;
  		Object *TO_TOOLBAR;
  		Object *TO_SERVER;
  		Object *GR_MAIN;
  		Object *GR_TOP;
  		Object *GR_PAGE;
  		Object *PG_PAGES[5];
		Object *LV_SERVER;
		Object *LV_SERVERS;
		Object *LV_RESEARCH;
		Object *LV_SHARED;
		Object *LV_TRANSFERT;
		Object *LV_CONFIG;
		Object *PREFS;
		Object *INFOS;
		Object *LOG;
  		Object *TXT;
		Object *NBSERVERS;
		Object *MI_ERRORS;
	};

enum DashBoardPage
	{
  		cp_Server = 0,
  		cp_Transfert,
  		cp_Research,
  		cp_Shared,
  		cp_Prefs,
  		cp_Max
	};

struct MA_ClassData  /* fenêtre principale */
	{
  		struct MA_GUIData GUI;
		Object *GR_PAGE;
  		Object *PG_PAGES[5];
  		char WinTitle[SIZE_COMMAND];
		char ScreenTitle[SIZE_DEFAULT];
	};

struct MA_ClassData *MA_New(void);
void  MA_SetSortFlag(void);
void  MA_SetupDynamicMenus(void);

#endif /* DASHBOARD_H */
