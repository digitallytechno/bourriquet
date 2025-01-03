/**************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <stdlib.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include "ExamineDir.h"
#include "Stringsizes.h"
#include "Utilities.h"

#include "SDI_compiler.h"
#include "SDI_stdarg.h"

#define DEBUG_USE_MALLOC_REDEFINE
#include "Debug.h"
#include "Extrasrc.h"

#if defined(NEED_EXAMINEDIR)

struct DirContext
{
    BPTR lock;
    BPTR oldCD;
    struct ExAllControl *eaControl;
    struct ExAllData *eaBuffer;
    struct ExAllData *eaData;
    ULONG dataFields;
    LONG more;
    LONG exAllError;
    BOOL restoreOldCD;
    struct ExamineData exData;
};

void ReleaseDirContext(APTR context);

/// ObtainDirContext
// obtenir un contexte d'analyse de répertoire pour un répertoire donné, y compris la recherche de motifs
APTR ObtainDirContext(struct TagItem *tags)
{
    struct DirContext *ctx;
    BOOL success = FALSE;

    ENTER();
    SHOWTAGS(DBF_UTIL, tags);
    if((ctx = (struct DirContext *)calloc(1, sizeof(*ctx))) != NULL)
      {
        char *dir;

        if((dir = (char *)GetTagData(EX_StringName, (ULONG)NULL, tags)) != NULL)
          {
            //SHOWSTRING(DBF_FOLDER, dir);
            if((ctx->lock = Lock(dir, SHARED_LOCK)) != ZERO)
              {
                if((ctx->eaControl = AllocDosObject(DOS_EXALLCONTROL, NULL)) != NULL)
                  {
                    ctx->eaControl->eac_LastKey = 0;
                    ctx->eaControl->eac_MatchString = (STRPTR)GetTagData(EX_MatchString, (ULONG)NULL, tags);
                    ctx->eaControl->eac_MatchFunc = NULL;
                    //SHOWSTRING(DBF_FOLDER, ctx->eaControl->eac_MatchString);
                    if((ctx->eaBuffer = malloc(SIZE_EXALLBUF)) != NULL)
                      {
                        if(GetTagData(EX_DoCurrentDir, FALSE, tags) == TRUE)
                          {
                            ctx->oldCD = CurrentDir(ctx->lock);
                            ctx->restoreOldCD = TRUE;
                          }
                        else ctx->restoreOldCD = FALSE;
                        ctx->dataFields = GetTagData(EX_DataFields, EXF_ALL, tags);
                        //SHOWVALUE(DBF_FOLDER, ctx->dataFields);
                        ctx->eaData = NULL;
                        ctx->more = 1;
                        ctx->exAllError = ERROR_NO_MORE_ENTRIES;
                        success = TRUE;
                      }
                  }
              }
          }
      }
    if(success == FALSE)
      {
        ReleaseDirContext(ctx);
        ctx = NULL;
      }
    RETURN(ctx);
    return(ctx);
}

void ReleaseDirContext(APTR context)
{
    ENTER();

    if(context != NULL)
      {
        struct DirContext *ctx = (struct DirContext *)context;

        if(ctx->more != 0 && ctx->lock != ZERO && ctx->eaBuffer != NULL && ctx->eaControl != NULL)
            ExAllEnd(ctx->lock, ctx->eaBuffer, SIZE_EXALLBUF, ED_COMMENT, ctx->eaControl);
        if(ctx->restoreOldCD == TRUE) CurrentDir(ctx->oldCD);
        free(ctx->eaBuffer);
        if(ctx->eaControl != NULL) FreeDosObject(DOS_EXALLCONTROL, ctx->eaControl);
        if(ctx->lock != ZERO) UnLock(ctx->lock);
        free(context);
      }
    LEAVE();
}

struct ExamineData *ExamineDir(APTR context)
{
    struct DirContext *ctx = (struct DirContext *)context;
    struct ExamineData *ed = NULL;

    ENTER();
    if(ctx->eaData == NULL && ctx->more != 0)
      {
        ctx->more = ExAll(ctx->lock, ctx->eaBuffer, SIZE_EXALLBUF, ED_COMMENT, ctx->eaControl);
        if((ctx->exAllError = IoErr()) == 0) ctx->exAllError = ERROR_NO_MORE_ENTRIES;
        if(ctx->more != 0 || ctx->eaControl->eac_Entries > 0) ctx->eaData = ctx->eaBuffer;
        //SHOWVALUE(DBF_FOLDER, ctx->more);
        //SHOWVALUE(DBF_FOLDER, ctx->exAllError);
        //SHOWVALUE(DBF_FOLDER, ctx->eaControl->eac_Entries);
      }

    if(ctx->eaData != NULL && ctx->eaControl->eac_Entries > 0)
      {
        ed = &ctx->exData;
        ed->Name = isFlagSet(ctx->dataFields, EXF_NAME) ? ctx->eaData->ed_Name : NULL;
        ed->FileSize = isFlagSet(ctx->dataFields, EXF_SIZE) ? (LONG)ctx->eaData->ed_Size : -1;
        if(isFlagSet(ctx->dataFields, EXF_TYPE))
          {
            if(EAD_IS_FILE(ctx->eaData)) ed->Type = FSO_TYPE_FILE;
            else if(EAD_IS_DRAWER(ctx->eaData)) ed->Type = FSO_TYPE_DIRECTORY;
            else if(EAD_IS_SOFTLINK(ctx->eaData)) ed->Type = FSO_TYPE_SOFTLINK;
          }
        else ed->Type = FSO_TYPE_MASK-1;
        //SHOWVALUE(DBF_FOLDER, ed->Type);
        //SHOWVALUE(DBF_FOLDER, ed->FileSize);
        //SHOWSTRING(DBF_FOLDER, ed->Name);
        ctx->eaData = ctx->eaData->ed_Next;
      }
    if(ctx->eaData == NULL && ctx->more == 0)
      {
        if(ctx->exAllError == 0) ctx->exAllError = ERROR_NO_MORE_ENTRIES;
        SetIoErr(ctx->exAllError);
      }
    RETURN(ed);
    return(ed);
}

#else
  #warning "NEED_EXAMINEDIR absence ou compilation inutile"
#endif

///

