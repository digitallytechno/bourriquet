/***************************************************************************
 Bourriquet  
 	digitally
 
***************************************************************************/
#ifndef ERROR_H
#define ERROR_H

#include <intuition/classusr.h>
#include "Stringsizes.h"

struct ER_GUIData
	{
  		Object *WI;
  		Object *LV_ERROR;
  		Object *BT_NEXT;
  		Object *NB_ERROR;
  		Object *BT_PREV;
	};

struct ER_ClassData  /* fenêtre d'erreur*/
	{
  		struct ER_GUIData GUI;
  		char SliderLabel[SIZE_SMALL];
  		char ScreenTitle[SIZE_DEFAULT];
	};

void ER_NewError(const char *message, ...);
void ER_NewWarning(const char *message, ...);

#endif /* ERROR_H */
