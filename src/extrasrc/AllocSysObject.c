/***************************************************************************
Bourriquet 
2024 digitally
***************************************************************************/
#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <clib/alib_protos.h>
#include <exec/interrupts.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include <exec/ports.h>
#include <exec/semaphores.h>

#include "AllocSysObject.h"
#include "ItemPool.h"
#include "include/SDI_compiler.h"
#include "include/SDI_stdarg.h"

#include "Utilities.h"

#define DEBUG_USE_MALLOC_REDEFINE
#include "Debug.h"
#include "Extrasrc.h"

#if defined(NEED_ALLOCSYSOBJECT)

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack(2)
   #endif
#elif defined(__VBCC__)
   #pragma amiga-align
#endif

// structures internes pour certains objets afin de garder l'information qui a �t� allou�
struct SysMsgPort
{
    LONG signal;
    STRPTR name;
    BOOL public;
    BOOL copy;
    struct MsgPort port;
};

struct SysSignalSemaphore
{
    STRPTR name;
    BOOL public;
    BOOL copy;
    struct SignalSemaphore semaphore;
};

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

/// AllocSysObject
APTR AllocSysObject(ULONG type, struct TagItem *tags)
{
  	union 
		{
          	APTR pointer;
          	struct IORequest *iorequest;
          	struct Hook *hook;
          	struct List *list;
          	struct Node *node;
          	struct MsgPort *port;
          	struct Message *message;
          	struct SignalSemaphore *semaphore;
          	struct TagItem *taglist;
          	APTR mempool;
          	struct ItemPool *itempool;
          	struct Interrupt *interrupt;
        } object;
  	struct TagItem *tstate = tags;
  	struct TagItem *tag;
  	ULONG memFlags;

  	object.pointer = NULL;
  	memFlags = GetTagData(ASO_MemoryOvr, MEMF_ANY, tags);
  	switch(type)
    	{
        	case ASOT_IOREQUEST:
          		{
            		ULONG size = sizeof(struct IORequest);
            		struct MsgPort *port = NULL;
            		struct IORequest *duplicate = NULL;

            		if(tags != NULL)
              			{
                			while((tag = NextTagItem((APTR)&tstate)) != NULL)
                  				{
                    				switch(tag->ti_Tag)
                      					{
            								case ASOIOR_Size:
              									size = tag->ti_Data;
            									break;
								            case ASOIOR_ReplyPort:
								              	port = (struct MsgPort *)tag->ti_Data;
            									break;
            								case ASOIOR_Duplicate:
              									duplicate = (struct IORequest *)tag->ti_Data;
            									break;
          								}
        						}
      					}
      				// si aucun port de r�ponse n'est donn� mais qu'une requ�te IO existante a �t� dupliqu�e,
      				// alors nous utiliserons ce port de r�ponse � la place
      				if(duplicate != NULL)
      					{
        					if(size == sizeof(struct IORequest)) size = duplicate->io_Message.mn_Length;
        					if(port == NULL) port = duplicate->io_Message.mn_ReplyPort;
      					}
      				// cr�er la requ�te IO de mani�re habituelle
      				object.iorequest = CreateIORequest(port, size);
      				if(object.iorequest != NULL && duplicate != NULL) CopyMem(duplicate, object.iorequest, size);
    			}
    			break;
    		case ASOT_HOOK:
    			{
      				HOOKFUNC entry = NULL;
      				HOOKFUNC subentry = NULL;
      				HOOKFUNC data = NULL;
      				ULONG size = sizeof(struct Hook);

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
            								case ASOHOOK_Size:
              									size = tag->ti_Data;
            									break;
            								case ASOHOOK_Entry:
              									entry = (HOOKFUNC)tag->ti_Data;
            									break;
            								case ASOHOOK_Subentry:
              									subentry = (HOOKFUNC)tag->ti_Data;
            									break;
            								case ASOHOOK_Data:
              									data = (HOOKFUNC)tag->ti_Data;
            									break;
          								}
        						}
      					}
      				if((object.hook = AllocVec(size, memFlags)) != NULL)
      					{
        					object.hook->h_Entry = (HOOKFUNC)HookEntry;
        					object.hook->h_SubEntry = (entry != NULL) ? entry : subentry;
        					object.hook->h_Data = data;
      					}
    			}
    			break;
    		case ASOT_LIST:
    			{
      				BOOL min = (BOOL)GetTagData(ASOLIST_Min, FALSE, tags);
      				ULONG size;

      				if(min == FALSE) size = GetTagData(ASOLIST_Size, sizeof(struct List), tags);
      				else size = GetTagData(ASOLIST_Size, sizeof(struct MinList), tags);
      				if((object.list = AllocVec(size, memFlags)) != NULL)
      					{
        					NewList(object.list);
        					if(min == FALSE) object.list->lh_Type = GetTagData(ASOLIST_Type, NT_UNKNOWN, tags);
      					}
    			}
    			break;
    		case ASOT_NODE:
    			{
      				BOOL min = (BOOL)GetTagData(ASONODE_Min, FALSE, tags);
      				ULONG size;

      				if(min == FALSE) size = GetTagData(ASONODE_Size, sizeof(struct Node), tags);
      				else size = GetTagData(ASONODE_Size, sizeof(struct MinNode), tags);
				    if((object.node = AllocVec(size, memFlags)) != NULL)
      					{
        					object.node->ln_Succ = (struct Node *)0xffffffff;
        					object.node->ln_Pred = (struct Node *)0xffffffff;
        					if(min == FALSE)
        						{
          							object.node->ln_Type = GetTagData(ASONODE_Type, NT_UNKNOWN, tags);
          							object.node->ln_Pri = GetTagData(ASONODE_Pri, 0, tags);
          							object.node->ln_Name = (STRPTR)GetTagData(ASONODE_Name, (IPTR)NULL, tags);
        						}
      					}
    			}
    			break;
    		case ASOT_PORT:
    			{
      				STRPTR name = NULL;
      				ULONG size = sizeof(struct MsgPort);
      				ULONG action = 0;
      				LONG pri = 0;
      				LONG signum = -1;
      				APTR target = NULL;
      				BOOL allocsig = TRUE;
      				BOOL public = FALSE;
      				BOOL copy = FALSE;

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
            								case ASOPORT_Size:
              									size = MAX(size, tag->ti_Data);
            									break;
            								case ASOPORT_AllocSig:
              									allocsig = tag->ti_Data;
            									break;
            								case ASOPORT_Action:
              									action = tag->ti_Data;
            									break;
            								case ASOPORT_Pri:
              									pri = tag->ti_Data;
            									break;
            								case ASOPORT_Name:
              									name = (STRPTR)tag->ti_Data;
            									break;
            								case ASOPORT_Signal:
              									signum = tag->ti_Data;
            									break;
            								case ASOPORT_Target:
              									target = (APTR)tag->ti_Data;
            									break;
  								          	case ASOPORT_Public:
              									public = tag->ti_Data;
            									break;
            								case ASOPORT_CopyName:
              									copy = tag->ti_Data;
            									break;
          								}
        						}
      					}
      				// ajouter notre propre taille de donn�es � l'allocation
      				size += sizeof(struct SysMsgPort) - sizeof(struct MsgPort);
      				if((object.port = AllocVec(size, memFlags)) != NULL)
      					{
        					struct SysMsgPort *sobject = (struct SysMsgPort *)object.port;

        					sobject->name = NULL;
        					sobject->signal = -1;
        					sobject->public = public;
        					sobject->copy = copy;
        					// alloue un signal si n�cessaire
        					if(signum == -1 || allocsig != FALSE)
        						{
          							if((signum = AllocSignal(signum)) < 0)
          								{
            								FreeVec(object.port);
            								object.port = NULL;
            								goto done;
          								}
          							sobject->signal = signum;
        						}
        					// dupliquer le nom donn� si demand�
        					if(copy != FALSE && name != NULL)
        						{
								  	name = strdup(name);
								  	sobject->name = name;
								}
							NewList(&sobject->port.mp_MsgList);
							sobject->port.mp_SigTask = (target != NULL) ? (struct Task *)target : FindTask(NULL);
							sobject->port.mp_Node.ln_Name = name;
							sobject->port.mp_Node.ln_Pri = pri;
							sobject->port.mp_Node.ln_Type = NT_MSGPORT;
							sobject->port.mp_Flags = action;
							sobject->port.mp_SigBit = signum;
        					// Faire un port public si n�cessaire
        					if(public != FALSE && name != NULL) AddPort(&sobject->port);
					        object.port = &sobject->port;
      					}
    			}
    			break;
    		case ASOT_MESSAGE:
    			{
      				STRPTR name = NULL;
      				ULONG size = sizeof(struct Message);
      				ULONG length = 0;
      				APTR port = NULL;

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
            								case ASOMSG_Size:
            			  						size = MAX(size, tag->ti_Data);
            									break;
											case ASOMSG_ReplyPort:
											  	port = (APTR)tag->ti_Data;
												break;
											case ASOMSG_Length:
												length = MAX(length, tag->ti_Data);
												break;
											case ASOMSG_Name:
											  	name = (STRPTR)tag->ti_Data;
												break;
          								}
        						}
      					}
      				if(length == 0) length = size;
      				if((object.message = AllocVec(size, memFlags)) != NULL)
      					{
        					object.message->mn_Node.ln_Name = name;
        					object.message->mn_Node.ln_Type = NT_MESSAGE;
        					object.message->mn_ReplyPort = port;
        					object.message->mn_Length = length;
      					}
    			}
    			break;
    		case ASOT_SEMAPHORE:
    			{
      				ULONG size = sizeof(struct SignalSemaphore);
      				LONG pri = 0;
      				BOOL public = FALSE;
      				BOOL copy = FALSE;
      				STRPTR name = NULL;

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
											case ASOSEM_Size:
											  	size = MAX(size, tag->ti_Data);
												break;
											case ASOSEM_Name:
											  	name = (STRPTR)tag->ti_Data;
												break;
											case ASOSEM_Pri:
											  	pri = tag->ti_Data;
												break;
											case ASOSEM_Public:
											  	public = tag->ti_Data;
												break;
											case ASOSEM_CopyName:
											  	copy = tag->ti_Data;
												break;
          								}
        						}
      					}
      				// ajouter notre propre taille de donn�es � l'allocation
      				size += sizeof(struct SysSignalSemaphore) - sizeof(struct SignalSemaphore);
      				if((object.semaphore = AllocVec(size, memFlags)) != NULL)
      					{
        					struct SysSignalSemaphore *sobject = (struct SysSignalSemaphore *)object.semaphore;

        					sobject->name = NULL;
        					sobject->public = public;
        					sobject->copy = copy;
        					// duplique le nom donn� si n�cessaire
        					if(copy != FALSE && name != NULL)
        						{
          							name = strdup(name);
          							sobject->name = name;
        						}
        					// initialise la s�maphore
							memset(&sobject->semaphore, 0, sizeof(sobject->semaphore));
							InitSemaphore(&sobject->semaphore);
							sobject->semaphore.ss_Link.ln_Pri = pri;
							sobject->semaphore.ss_Link.ln_Name = name;
        					// faire un port public si n�cessaire
        					if(public != FALSE && name != NULL) AddSemaphore(&sobject->semaphore);
					        object.semaphore = &sobject->semaphore;
      					}
    			}
    			break;
    		case ASOT_TAGLIST:
    			{
      				ULONG entries = 0;

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
            								case ASOTAGS_NumEntries:
            		  							entries = tag->ti_Data;
            									break;
          								}
        						}
      					}
      				object.taglist = AllocVec(entries * sizeof(struct TagItem), memFlags);
    			}
    			break;
    		case ASOT_MEMPOOL:
    			{
      				ULONG memFlags = MEMF_ANY;
      				ULONG puddle = 8192;
      				ULONG thresh = 8000;

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
            								case ASOPOOL_MFlags:
            		 	 						memFlags = tag->ti_Data;
            									break;
            								case ASOPOOL_Puddle:
              									puddle = tag->ti_Data;
            									break;
            								case ASOPOOL_Threshold:
              									thresh = tag->ti_Data;
            									break;
          								}
        						}
        					if(thresh > puddle) puddle = thresh;
      					}
      				object.mempool = CreatePool(memFlags, puddle, thresh);
    			}
    			break;
    		case ASOT_ITEMPOOL:
    			{
      				ULONG memFlags = MEMF_ANY;
      				ULONG itemSize = 8;
      				ULONG batchSize = 32;
      				BOOL protected = FALSE;

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
            								case ASOITEM_MFlags:
              									memFlags = tag->ti_Data;
            									break;
											case ASOITEM_ItemSize:
											  	itemSize = MAX(8, tag->ti_Data);
												break;
											case ASOITEM_BatchSize:
												batchSize = tag->ti_Data;
												break;
											case ASOITEM_Protected:
											  	protected = tag->ti_Data;
												break;
          								}
        						}
      					}
      				if((object.itempool = AllocVec(sizeof(struct ItemPool), MEMF_CLEAR)) != NULL)
      					{
       							if(protected != FALSE) memFlags |= MEMF_SEM_PROTECTED;
        						if((object.itempool->pool = CreatePool(memFlags, batchSize*itemSize, batchSize*itemSize)) != NULL) object.itempool->itemSize = itemSize;
        						else
        							{
          								FreeVec(object.itempool);
          								object.itempool = NULL;
        							}
      					}
    			}
    			break;
    		case ASOT_INTERRUPT:
    			{
      				ULONG size = sizeof(struct Interrupt);
      				void (*code)() = NULL;
      				void (*data)() = NULL;

      				if(tags != NULL)
      					{
        					while((tag = NextTagItem((APTR)&tstate)) != NULL)
        						{
          							switch(tag->ti_Tag)
          								{
            								case ASOINTR_Size:
              									size = tag->ti_Data;
            									break;
            								case ASOINTR_Code:
              									code = (void (*)())tag->ti_Data;
            									break;
            								case ASOINTR_Data:
              									data = (void (*)())tag->ti_Data;
            									break;
          								}
        						}
      					}
      				if((object.interrupt = AllocVec(size, memFlags)) != NULL)
      					{
        					object.interrupt->is_Code = code;
        					object.interrupt->is_Data = data;
        					object.interrupt->is_Node.ln_Type = NT_INTERRUPT;
      					}
    			}
    			break;
    		default:
    			{
      				// ignored
    			}
    			break;
  		}
	done:
  		return(object.pointer);
}

///
/// FreeSysObject
// libère un objet système et effectue des nettoyages de base en fonction du type
// pas d'appels de macro ENTER/RETURN volontairement, car cela gonflerait trop le journal de trace
void FreeSysObject(ULONG type, APTR object)
{
  if(object != NULL)
  	{
    	switch(type)
    		{
      			case ASOT_PORT:
      				{
        				struct SysMsgPort *sobject = (struct SysMsgPort *)((IPTR)object - OFFSET_OF(struct SysMsgPort, port));

        				// retirer le port de la liste publique, s'il était public
        				if(sobject->public != FALSE) RemPort(&sobject->port);
				        // libère le nom de la mémoire, s'il était dupliqué
        				if(sobject->copy != FALSE) free(sobject->name);
				        // libère le signal
        				if(sobject->signal != -1) FreeSignal(sobject->signal);
#if defined(DEBUG)
        				if(IsListEmpty(&sobject->port.mp_MsgList) == FALSE)
          				W(DBF_UTIL, "freeing MsgPort %08lx with pending messages", &sobject->port);
#endif
        				FreeVec(sobject);
      				}
      				break;
      			case ASOT_IOREQUEST:
      				{
        				DeleteIORequest(object);
      				}
      				break;
      			case ASOT_HOOK:
      				{
        				FreeVec(object);
      				}
      				break;
      			case ASOT_LIST:
      				{
#if defined(DEBUG)
        				if(IsListEmpty((struct List *)object) == FALSE)
          				W(DBF_UTIL, "freeing non-empty list %08lx", object);
#endif
        				FreeVec(object);
      				}
      				break;
      			case ASOT_NODE:
      				{
        				FreeVec(object);
      				}
      				break;
      			case ASOT_MESSAGE:
      				{
        				FreeVec(object);
      				}
      				break;
      			case ASOT_TAGLIST:
      				{
        				FreeVec(object);
      				}
      				break;
      			case ASOT_SEMAPHORE:
      				{
        				struct SysSignalSemaphore *sobject = (struct SysSignalSemaphore *)((IPTR)object - OFFSET_OF(struct SysSignalSemaphore, semaphore));

        				// libère la sémaphore à partir de la liste publique, si elle était publique
        				if(sobject->public != FALSE) RemSemaphore(&sobject->semaphore);
        				// libère le nom de la mémoire, s'il était dupliqué
        				if(sobject->copy != FALSE) free(sobject->name);
        				FreeVec(sobject);
      				}
      				break;
      			case ASOT_MEMPOOL:
      				{
        				DeletePool(object);
      				}
      				break;
      			case ASOT_ITEMPOOL:
      				{
        				struct ItemPool *sobject = (struct ItemPool *)object;

        				DeletePool(sobject->pool);
        				FreeVec(sobject);
      				}
      				break;
      			case ASOT_INTERRUPT:
      				{
        				FreeVec(object);
      				}
      				break;
      			default:
      				{
        				// ignoré
      				}
      				break;
    		}
  	}
}

#else
  #warning "NEED_ALLOCSYSOBJECT Absence ou compilation inutile"
#endif

///
