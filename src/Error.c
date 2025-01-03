/***************************************************************************
 Bourriquet 
 	digitally
 
***************************************************************************/
#include <stdlib.h>
#include <string.h>

#include <clib/alib_protos.h>
#include <libraries/iffparse.h>
#include <mui/NListview_mcc.h>
#include <mui/NFloattext_mcc.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/muimaster.h>

#include "Extrasrc.h"
#include "SDI_hook.h"
#include "SDI_stdarg.h"
#include "Bourriquet.h"
#include "Error.h"
#include "DashBoard.h"
#include "Locale.h"
#include "MethodStack.h"
#include "MUIObjects.h"
#include "Threads.h"
#include "mui/ClassesExtra.h"
#include "mui/BourriquetApplication.h"
#include "Debug.h"

/* prototype local */
static struct ER_ClassData *ER_New(void);

///
/// ShowMessage
static void ShowMessage(BOOL isError, const char *message, va_list args)
	{
  		ENTER();
  		if(IsMainThread() == TRUE && G->ER == NULL)
  			{
    			if((G->ER = ER_New()) == NULL)
    				{
      					LEAVE();
      					return;
    				}
  			}
  		if(IsMainThread() == TRUE)
  			{
    			//D(DBF_STARTUP, "showing %s message %s", isError ? "error" : "warning", message);
    			set(G->ER->GUI.LV_ERROR, MUIA_Text_Contents, message); 
    			if(SafeOpenWindow(G->ER->GUI.WI) == FALSE)  DisposeModule(&G->ER);
  			}
  		LEAVE();
	}

///
/// ER_NewError
// Ajoute un nouveau message d'erreur et l'affiche 
void ER_NewError(const char *message, ...)
	{
  		va_list args;

  		ENTER();
  		va_start(args, message);
  		ShowMessage(TRUE, message, args);
  		va_end(args);
  		LEAVE();
	}

///
/// ER_NewWarning
// Ajoute un nouveau message d'attention
void ER_NewWarning(const char *message, ...)
	{
  		va_list args;
  		
		ENTER();
  		va_start(args, message);
  		ShowMessage(FALSE, message, args);
  		va_end(args);
  		LEAVE();
	}

///
/// ER_CloseFunc
// Ferme la fenêtre d'erreur
HOOKPROTONHNO(ER_CloseFunc, void, int *arg)
	{
  		ENTER();
  		set(G->ER->GUI.WI, MUIA_Window_Open, FALSE);
  		DisposeModulePush(&G->ER);
  		LEAVE();
	}
MakeStaticHook(ER_CloseHook, ER_CloseFunc);

///
/// ER_New
// Créée une fenêtre d'erreur
static struct ER_ClassData *ER_New(void)
	{
  		struct ER_ClassData *data;

  		ENTER();
  		if((data = calloc(1, sizeof(struct ER_ClassData))) != NULL)
  			{
    			Object *bt_close;
				
    			data->GUI.WI = WindowObject,
    	   									MUIA_Window_Title, "Message d'erreur",
       										MUIA_Window_ID, MAKE_ID('E','R','R','O'),
											MUIA_Window_CloseGadget, FALSE,
       										WindowContents, VGroup,
																		  	Child, data->GUI.LV_ERROR = TextObject,
																																VirtualFrame,
																																MUIA_Text_PreParse, "\033c",
																		  	End,
																			Child, bt_close = MakeButton("Fermer"),
       										End,
    					End;
    			if(data->GUI.WI != NULL)
    				{
      					DoMethod(G->App, OM_ADDMEMBER, data->GUI.WI);
      					DoMethod(bt_close, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 3, MUIM_CallHook, &ER_CloseHook, FALSE);
      				}
    			else
    				{
      					free(data);
      					data = NULL;
    				}
  			}
  		RETURN(data);
  		return data;
	}
