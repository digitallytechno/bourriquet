/***************************************************************************
Bourriquet
  digitally
***************************************************************************/
#ifndef EXAMINEDIR_H
#define EXAMINEDIR_H 1

#include <exec/types.h>

struct ExamineData
{
  LONG Type;
  LONG FileSize;
  STRPTR Name;
};

#define FSO_TYPE_MASK                0xff
#define FSO_TYPE_SOFTLINK            0
#define FSO_TYPE_FILE                1
#define FSO_TYPE_DIRECTORY           2
#define EXD_IS_FILE(exd)             (((exd)->Type & FSO_TYPE_MASK) == FSO_TYPE_FILE)
#define EXD_IS_DIRECTORY(exd)        (((exd)->Type & FSO_TYPE_MASK) == FSO_TYPE_DIRECTORY)
#define EAD_IS_FILE(ead)             ((ead)->ed_Type <  0)
#define EAD_IS_DRAWER(ead)           ((ead)->ed_Type >= 0 && \
                                      (ead)->ed_Type != ST_SOFTLINK)
#define EAD_IS_SOFTLINK(ead)         ((ead)->ed_Type == ST_SOFTLINK)
#define EX_Dummy                     (TAG_USER+4711)
#define EX_StringName                (EX_Dummy+1)
#define EX_DataFields                (EX_Dummy+2)
#define EX_MatchString               (EX_Dummy+3)
#define EX_DoCurrentDir              (EX_Dummy+4)
#define EXF_NAME                     (1<<0)
#define EXF_TYPE                     (1<<1)
#define EXF_SIZE                     (1<<2)
#define EXF_ALL                      (0xffffffff)   // all of the above

#endif /* EXAMINEDIR_H */
