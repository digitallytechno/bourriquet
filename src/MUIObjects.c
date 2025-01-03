/***************************************************************************
Bourriquet 
  digitally
 
***************************************************************************/
#include <stdlib.h>
#include <string.h>

#include <clib/alib_protos.h>
#include <mui/BetterString_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include "Extrasrc.h"
#include "SDI_hook.h"

#include "Bourriquet.h"
#include "Global.h"
#include "mui/ClassesExtra.h"
#include "Locale.h"
#include "MUIObjects.h"
#include "Debug.h"

/// MakeCycle
//  Création d'un bouton de cycle MUI
Object *MakeCycle(const char *const *labels, const char *label)
{
    return CycleObject,
           MUIA_CycleChain,    TRUE,
           MUIA_Font,          MUIV_Font_Button,
           MUIA_Cycle_Entries, labels,
           MUIA_ControlChar,   ShortCut(label),
           End;
}

///
/// MakeButton
// Création d'un bouton MUI
Object *MakeButton(const char *txt)
{
    Object *obj;

    if((obj = MUI_MakeObject(MUIO_Button,(IPTR)txt)) != NULL) set(obj, MUIA_CycleChain, TRUE);
    return(obj);
}

///
/// MakeCheck
// Création d'un bouton à encoche MUI
Object *MakeCheck(const char *label)
{
  return ImageObject,
           ImageButtonFrame,
           MUIA_InputMode   , MUIV_InputMode_Toggle,
           MUIA_Image_Spec  , MUII_CheckMark,
           MUIA_Background  , MUII_ButtonBack,
           MUIA_ShowSelState, FALSE,
           MUIA_ControlChar , ShortCut(label),
           MUIA_CycleChain  , TRUE,
         End;
}

///
/// MakeCheckGroup
//  Crée un objet MUI étiqueté avec une encoche
Object *MakeCheckGroup(Object **check, const char *label)
{
  return HGroup,
           Child, *check = MakeCheck(label),
           Child, Label1(label),
           Child, HSpace(0),
         End;
}

///
/// MakeString
//  Crée un objet de chaîne MUI
Object *MakeString(int maxlen, const char *label)
{
  return BetterStringObject,
           StringFrame,
           MUIA_String_MaxLen,      maxlen,
           MUIA_String_AdvanceOnCR, TRUE,
           MUIA_ControlChar,        ShortCut(label),
           MUIA_CycleChain,         TRUE,
         End;
}

///
/// MakePassString
//  Crée un objet MUI de type chaîne de caractères avec un texte caché
Object *MakePassString(const char *label)
{
  return BetterStringObject,
           StringFrame,
           MUIA_String_MaxLen,       SIZE_PASSWORD,
           MUIA_String_Secret,       TRUE,
           MUIA_String_AdvanceOnCR,  TRUE,
           MUIA_ControlChar,         ShortCut(label),
           MUIA_CycleChain,          TRUE,
         End;
}

///
/// MakeInteger
//  Crée un objet MUI de type chaîne de caractères pour la saisie numérique
Object *MakeInteger(int maxlen, const char *label)
{
  return BetterStringObject,
           StringFrame,
           MUIA_String_MaxLen,       maxlen+1,
           MUIA_String_AdvanceOnCR,  TRUE,
           MUIA_ControlChar,         ShortCut(label),
           MUIA_CycleChain,          TRUE,
           MUIA_String_Integer,      0,
           MUIA_String_Accept,       "0123456789",
         End;
}

///
/// MakeNumeric
//  Création d'un curseur numérique MUI
Object *MakeNumeric(int min, int max, BOOL percent)
{
  return NumericbuttonObject,
           MUIA_Numeric_Min, min,
           MUIA_Numeric_Max, max,
           MUIA_Numeric_Format, percent ? "%ld%%" : "%ld",
           MUIA_CycleChain, TRUE,
         End;
}

///
/// MakeCloseButton
// création d'un bouton de fermeture
Object *MakeCloseButton(void)
{
    Object *obj;

    ENTER();
    if(LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 5500))
      {
        obj = ImageObject,
        // Image.mui supprimera le cadre et l'arrière-plan si l'image le nécessite
              ButtonFrame,
              MUIA_Background, MUII_ButtonBack,
              MUIA_Image_Spec, MUII_Close,
              MUIA_InputMode, MUIV_InputMode_RelVerify,
              MUIA_CycleChain, TRUE,
            End;
      }
    else
      {
    // créer un bouton simple avec un « X » en gras
        obj = TextObject,
                ButtonFrame,
                MUIA_CycleChain,     TRUE,
                MUIA_Font,           MUIV_Font_Tiny,
                MUIA_InputMode,      MUIV_InputMode_RelVerify,
                MUIA_Background,     MUII_ButtonBack,
                MUIA_Text_SetMax,    TRUE,
                MUIA_Text_Copy,      FALSE,
                MUIA_Text_PreParse,  MUIX_B,
                MUIA_Text_Contents,  "X",
              End;
      }
    RETURN(obj);
    return(obj);
}

///
/// ShortCut
//  Recherche d'un raccourci clavier dans une étiquette de texte
char ShortCut(const char *label)
{
  char scut = '\0';
  char *ptr;

  ENTER();
  if(label != NULL && (ptr = strchr(label, '_')) != NULL) scut = (char)ToLower((ULONG)(*++ptr));
  RETURN(scut);
  return(scut);
}

///
/// FilereqStartFunc
// Sera exécuté dès que l'utilisateur voudra faire apparaître une fenêtre de demande de fichiers.
// pour sélectionner des fichiers
HOOKPROTONO(FilereqStartFunc, BOOL, struct TagItem *tags)
{
    Object *strObj = (Object *)hook->h_Data;
    char *str;

    ENTER();

    str = (char *)xget(strObj, MUIA_String_Contents);
    if(IsStrEmpty(str) == FALSE)
      {
        int i=0;
        static char buf[SIZE_PATHFILE];
        char *p;

        // s'assurer que la chaîne n'est pas entre guillemets.
        strlcpy(buf, str, sizeof(buf));
	      //UnquoteString(buf, FALSE);
        if((p = PathPart(buf)))
          {
            static char drawer[SIZE_PATHFILE];

            strlcpy(drawer, buf, MIN(sizeof(drawer), (unsigned int)(p - buf + 1)));
            tags[i].ti_Tag = ASLFR_InitialDrawer;
            tags[i].ti_Data= (ULONG)drawer;
            i++;
          }
        tags[i].ti_Tag = ASLFR_InitialFile;
        tags[i].ti_Data = (ULONG)FilePart(buf);
        i++;
        tags[i].ti_Tag = TAG_DONE;
      }
    RETURN(TRUE);
    return(TRUE);
}
MakeHook(FilereqStartHook, FilereqStartFunc);

///
/// FilereqStopFunc
// Sera exécuté dès que l'utilisateur aura sélectionné un fichier
HOOKPROTONO(FilereqStopFunc, void, struct FileRequester *fileReq)
{
    Object *strObj = (Object *)hook->h_Data;

    ENTER();

  //vérifier si un fichier a été sélectionné ou non
    if(IsStrEmpty(fileReq->fr_File) == FALSE)
      {
        char buf[SIZE_PATHFILE];

	    //AddPath(buf, fileReq->fr_Drawer, fileReq->fr_File, sizeof(buf));

      // vérifier s'il y a un espace dans le chemin d'accès
        if(strchr(buf, ' ') != NULL)
          {
            int len = strlen(buf);

            memmove(&buf[1], buf, len+1);
            buf[0] = '"';
            buf[len+1] = '"';
            buf[len+2] = '\0';
          }
        set(strObj, MUIA_String_Contents, buf);
      }
    LEAVE();
}
MakeHook(FilereqStopHook, FilereqStopFunc);

///
/// PO_Window
// Crochet de fenêtre pour les objets popup
HOOKPROTONH(PO_Window, void, Object *pop, Object *win)
{
    ENTER();
    set(win, MUIA_Window_DefaultObject, pop);
    LEAVE();
}
MakeHook(PO_WindowHook, PO_Window);

///
/// GetMUIString
// copier le contenu d'un objet chaîne MUI dans une chaîne de caractères
void GetMUIString(char *s, Object *o, size_t len)
{
    char *c;

    ENTER();

    if((c = (char *)xget(o, MUIA_String_Contents)) != NULL) strlcpy(s, c, len);
    else
      {
        E(DBF_GUI, "Le contenu de la cha�ne de l'objet %08lx est NULL", o);
        s[0] = '\0';
      }
    LEAVE();
}

///
/// GetMUIText
// copier le contenu d'un objet texte MUI dans une chaîne de caractères
void GetMUIText(char *s, Object *o, size_t len)
{
    char *c;

    ENTER();
    if((c = (char *)xget(o, MUIA_Text_Contents)) != NULL) strlcpy(s, c, len);
    else
      {
        E(DBF_GUI, "NULL text contents of object %08lx", o);
        s[0] = '\0';
      }
    LEAVE();
}

///
/// CreateScreenTitle
const char *CreateScreenTitle(char *dst, size_t dstlen, const char *text)
{
    char *result = NULL;
  
    ENTER();
    if(dst != NULL && dstlen > 0)
      {
        if(text != NULL) snprintf(dst, dstlen, "Bourriquet %s - %s", bourriquetver, text);
        else snprintf(dst, dstlen, "Bourriquet %s", bourriquetver);
        result = dst;
      }
    RETURN(result);
    return(result);
}

///
