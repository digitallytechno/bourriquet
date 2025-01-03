/***************************************************************************
Bourriquet 
	digitally
 
***************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <clib/alib_protos.h>
#include <dos/dostags.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "Bourriquet.h"
#include "SDI_stdarg.h"
#include "Extrasrc.h"
#include "Locale.h"
#include "MethodStack.h"
#include "Threads.h"
#include "Debug.h"

struct PushedMethod
{
    struct Message msg;  // Faire de ce message un véritable message d'Exec
    Object *object;      // pointeur sur l'objet destiné à recevoir l'appel de méthode
    ULONG flags;         // divers drapeaux
    ULONG argCount;      // nombre d'arguments à suivre
    IPTR *args;          // pointeur sur une zone de mémoire configurée pour contenir les arguments
    IPTR result;         // la valeur retour d'un appel synchrone
};

#define PMF_SYNC       (1<<0)

/// InitMethodStack
// initialise la pile de méthode globale
BOOL InitMethodStack(void)
{
    BOOL success = FALSE;

    ENTER();

    if((G->methodStack = AllocSysObjectTags(ASOT_PORT, TAG_DONE)) != NULL)
      {
        success = TRUE;
      }

    RETURN(success);
    return(success);
}

///
/// CleanupMethodStack
// nettoie la pile de méthode globale
void CleanupMethodStack(void)
{
    ENTER();

    if(G->methodStack != NULL)
      {
        struct Message *msg;

        // retirer toutes les méthodes de la pile sans les traiter
        while((msg = GetMsg(G->methodStack)) != NULL)
          {
            struct PushedMethod *pm = (struct PushedMethod *)msg;

            free(pm->args);
            FreeSysObject(ASOT_MESSAGE, pm);
          }

        // libére la pile
        FreeSysObject(ASOT_PORT, G->methodStack);
        G->methodStack = NULL;
      }
    LEAVE();
}

///
/// PushMethodOnStack
// pousser une méthode avec tous les paramètres donnés sur la pile des méthodes
BOOL PushMethodOnStackA(Object *obj, ULONG argCount, struct TagItem *tags)
{
    struct PushedMethod *pm;
    BOOL success = FALSE;

    ENTER();

    if((pm = AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(*pm), TAG_DONE)) != NULL)
      {
        // remplir les données
        pm->object = obj;
        // exécuter celle-ci de manière asynchrone
        pm->flags = 0;
        pm->argCount = argCount;
        pm->args = memdup((void *)tags, argCount*sizeof(IPTR));
        // pousser la méthode sur la pile
        PutMsg(G->methodStack, (struct Message *)pm);
        success = TRUE;
      }

    RETURN(success);
    return(success);
}

///
/// PushMethodOnStackWait
// pousser une méthode avec tous les paramètres donnés sur la pile de méthodes et attendre
// que l'application la traite
IPTR PushMethodOnStackWaitA(Object *obj, ULONG argCount, struct TagItem *tags)
{
    struct PushedMethod *pm;
    IPTR result = (IPTR)-1;

    ENTER();

    if((pm = AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(*pm), TAG_DONE)) != NULL)
      {
        // remplir les données
        pm->object = obj;
        // exécuter celle-ci de manière asynchrone
        pm->flags = PMF_SYNC;
        pm->argCount = argCount;
        pm->args = memdup((void *)tags, argCount*sizeof(IPTR));

        if(IsMainThread() == TRUE)
          {
            // nous avons été appelés depuis le fil d'exécution principal, nous allons donc traiter 
            // immédiatement cette méthode. Mais d'abord, nous traitons toute méthode déjà en attente sur
            // sur la pile.
            CheckMethodStack();
            // effectuer l'action souhaitée et obtenir la valeur de retour
            result = DoMethodA(pm->object, (Msg)pm->args);
          }
        else
          {
            struct Process *me = (struct Process *)FindTask(NULL);
            struct MsgPort *replyPort = &me->pr_MsgPort;

            // définir le port du processus comme port de réponse
            pm->msg.mn_ReplyPort = replyPort;
            pm->result = (IPTR)-1;
            // pousser la méthode sur la pile
            PutMsg(G->methodStack, (struct Message *)pm);
            // attendre que la méthode soit traitée
            WaitPort(replyPort);
            GetMsg(replyPort);
            result = pm->result;
          }
        // libère finalement la méthode traitée
        free(pm->args);
        FreeSysObject(ASOT_MESSAGE, pm);
      }
    RETURN(result);
    return(result);
}

///
/// CheckMethodStack
// gérer les méthodes poussées en attente sur la pile
void CheckMethodStack(void)
{
  struct Message *msg;

  ENTER();
  // essayer de sortir une méthode de la pile
   while((msg = GetMsg(G->methodStack)) != NULL)
    {
      struct PushedMethod *pm = (struct PushedMethod *)msg;

      // vérification de l'exécution synchrone ou asynchrone
      if(isFlagSet(pm->flags, PMF_SYNC))
        {
          // effectuer l'action souhaitée et obtenir la valeur de retour
          if(pm->object != NULL) pm->result = DoMethodA(pm->object, (Msg)pm->args);
          else pm->result = (IPTR)-1;
          // retour à l'expéditeur
          ReplyMsg((struct Message *)pm);
        }
      else
        {
          // effectuer l'action souhaitée
          DoMethodA(pm->object, (Msg)pm->args);
          // libérer la méthode traitée
          free(pm->args);
          FreeSysObject(ASOT_MESSAGE, pm);
        }
    }
  LEAVE();
}

///
