/***************************************************************************
Bourriquet 
	digitally
 
***************************************************************************/
#ifndef MUIOBJECTS_H
#define MUIOBJECTS_H

#include "SDI_compiler.h"

struct FileRequester;
struct fcstr;

Object *MakeButton(const char *txt);
Object *MakeCheck(const char *label);
Object *MakeCheckGroup(Object **check, const char *label);
Object *MakeCycle(const char *const *labels, const char *label);
Object *MakeInteger(int maxlen, const char *label);
Object *MakeNumeric(int min, int max, BOOL percent);
Object *MakePassString(const char *label);
Object *MakeString(int maxlen, const char *label);
Object *MakeAddressField(Object **string, const char *label, const struct fcstr * const help, int abmode, int winnr, ULONG flags);
Object *MakeCloseButton(void);
char ShortCut(const char *label);

extern struct Hook FilereqStartHook;
extern struct Hook FilereqStopHook;
extern struct Hook PO_WindowHook;

#define GetMUICheck(o)   (BOOL)xget((o), MUIA_Selected)
#define GetMUICycle(o)   (int)xget((o), MUIA_Cycle_Active)
#define GetMUIInteger(o) (int)xget((o), MUIA_String_Integer)
#define GetMUINumer(o)   (int)xget((o), MUIA_Numeric_Value)
#define GetMUIRadio(o)   (int)xget((o), MUIA_Radio_Active)

void GetMUIString(char *s, Object *o, size_t len);
void GetMUIText(char *s, Object *o, size_t len);

#define SetHelp(o,str)        set(o, MUIA_ShortHelp, str)

// macros pour une création plus facile des objets
#define HBarT(str)            RectangleObject, \
                                MUIA_FixHeightTxt, (str), \
                                MUIA_Rectangle_BarTitle, (str), \
                                MUIA_Rectangle_HBar, TRUE

#define VBarT(str)            RectangleObject, \
                                MUIA_FixWidthTxt, (str), \
                                MUIA_Rectangle_BarTitle, (str), \
                                MUIA_Rectangle_VBar, TRUE

BOOL isChildOfGroup(Object *group, Object *child);
BOOL isChildOfFamily(Object *family, Object *child);
const char *CreateScreenTitle(char *dst, size_t dstlen, const char *text);

#define COLLECT_SIZE 32
#define COLLECT_RETURNIDS { \
                            ULONG returnID[COLLECT_SIZE], csize = COLLECT_SIZE, rpos = COLLECT_SIZE, userData, userSigs = 0; \
                            while(csize && userSigs == 0 && (userData = DoMethod(G->App, MUIM_Application_NewInput, &userSigs))) \
                              returnID[--csize] = userData

#define REISSUE_RETURNIDS   while(rpos > csize) \
                              DoMethod(G->App, MUIM_Application_ReturnID, returnID[--rpos]); \
                          }

/// xget()
//  Obtient une valeur d'attribut à partir d'un objet MUI
IPTR xget(Object *obj, const IPTR attr);
#if defined(__GNUC__)
  #define xget(OBJ, ATTR) ({IPTR b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif

///
/// xset()
//  Met des attributs à un objet MUI
ULONG xset(Object *obj, ...);
#if defined(__GNUC__) || defined(__VBCC__)
  #define xset(obj, ...)  SetAttrs((obj), __VA_ARGS__, TAG_DONE)
#endif

///

#endif /* MUIOBJECTS_H */
