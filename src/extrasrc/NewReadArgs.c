/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#include <dos/dos.h>
#include <exec/memory.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <string.h>

#include "SDI_compiler.h"
#include "Extrasrc.h"
#include "NewReadArgs.h"
#include "Debug.h"

#if defined(NEED_NEWREADARGS)
#ifndef MEMF_SHARED
#define MEMF_SHARED MEMF_PUBLIC
#endif

void NewFreeArgs(struct NewRDArgs *);
LONG NewReadArgs(struct WBStartup *, struct NewRDArgs *);

void NewFreeArgs(struct NewRDArgs *rdargs)
{
    ENTER();

	//D(DBF_STARTUP, "FreeArgs(rdargs->FreeArgs)");
    FreeArgs(rdargs->FreeArgs);

    if(rdargs->RDArgs != NULL)
		{
			free((void *)rdargs->RDArgs->RDA_Source.CS_Buffer);
			//D(DBF_STARTUP, "FreeDosObject(DOS_RDARGS, rdargs->RDArgs)");
			FreeDosObject(DOS_RDARGS, rdargs->RDArgs);
		}
    if(rdargs->Args != NULL)
		{
			ULONG i;

			for(i=0; i < rdargs->MaxArgs; i++)
				{
					if(rdargs->Args[i] != NULL) FreeVecPooled(rdargs->Pool, rdargs->Args[i]);
				}
			FreeVecPooled(rdargs->Pool, rdargs->Args);
			rdargs->Args = NULL;
		}
    if(rdargs->ToolWindow != NULL)
		{
			FreeVecPooled(rdargs->Pool, rdargs->ToolWindow);
			rdargs->ToolWindow = NULL;
		}
    if(rdargs->WinFH != 0)
		{
			D(DBF_STARTUP, "SelectOutput( .. ) .. Close( ... )");
			SelectOutput(rdargs->OldOutput);
			SelectInput(rdargs->OldInput);
			Close(rdargs->WinFH);
			rdargs->WinFH = 0;
		}
    if(rdargs->Pool != NULL)
		{
			DeletePool(rdargs->Pool);
			rdargs->Pool = NULL;
		}
	//D(DBF_STARTUP, "memory freed");
    LEAVE();
}

STATIC LONG IsArg(CONST_STRPTR template, STRPTR keyword)
{
    UBYTE buffer[128], c;
    STRPTR ptr = (STRPTR)buffer;

    while((c = *keyword++) && (c != '=')) *ptr++ = c;
    *ptr = 0;
    return(FindArg(template, (STRPTR)buffer));
}

LONG NewReadArgs(struct WBStartup *WBStartup, struct NewRDArgs *nrdargs)
{
  #ifdef ICONGETA_RemapIcon
  static const struct TagItem icontags[] =
	{
		{ ICONGETA_RemapIcon, FALSE   },
		{ TAG_DONE,           TAG_END }
	};
  #endif

    ENTER();
    nrdargs->RDArgs     = NULL;
    nrdargs->FreeArgs   = NULL;
    nrdargs->Args       = NULL;
    nrdargs->MaxArgs    = 0;
    nrdargs->ToolWindow = NULL;
    nrdargs->WinFH      = 0;
    nrdargs->Pool       = NULL;
    if((nrdargs->RDArgs = (struct RDArgs *)AllocDosObject(DOS_RDARGS, NULL)) != NULL)
		{
			APTR pool = NULL;
			STRPTR ToolWindow = nrdargs->Window;

			//D(DBF_STARTUP, "nrdargs->RDArgs");
			if(WBStartup != NULL)
				{
					struct WBArg *wbarg;
					STRPTR *Args;
					STRPTR ptr;
					LONG MaxArgs = 1;
					LONG *ArgLen;
					LONG num = WBStartup->sm_NumArgs;
					LONG FileArgs = nrdargs->FileParameter;
					LONG FArgNum = -1L;
					LONG MultiArg = -1L;

					//D(DBF_STARTUP, "WBStartup != NULL");
					if((ptr = nrdargs->Template) == NULL)
						{
							//D(DBF_STARTUP, "nrdargs->Template == NULL");
                            RETURN(ERROR_BAD_TEMPLATE);
							return(ERROR_BAD_TEMPLATE);
						}
					while(*ptr)
						{
							if(*ptr++ == '/' && *ptr == 'M' )
								{
									MultiArg = MaxArgs-1L;
									ptr++;
								}
							else if(*(ptr-1) == ',')
							MaxArgs++;
						}
					//D(DBF_STARTUP, "nombre maximum d'arguments %ld", MaxArgs);
					ptr = nrdargs->Template;
					FileArgs = (FileArgs > num) ? num : ((FileArgs == -1) ? 0L : num);
					MaxArgs += FileArgs;
					pool = nrdargs->Pool = CreatePool(MEMF_CLEAR, 1024, 1024);
					if(pool == NULL || (Args = nrdargs->Args = AllocVecPooled(pool, MaxArgs * sizeof(STRPTR) * 2)) == NULL)
						{
							RETURN(ERROR_NO_FREE_STORE);
							return(ERROR_NO_FREE_STORE);
						}
					nrdargs->MaxArgs = MaxArgs;
					ArgLen = (LONG *)&Args[MaxArgs];
					for(wbarg = WBStartup->sm_ArgList, num = 0L; num < WBStartup->sm_NumArgs; num++, wbarg++)
						{
							struct DiskObject *dobj;
							BPTR olddir;

							if(FileArgs)
								{
									char buf[300];

									if(FArgNum < FileArgs && FArgNum >= 0L)
										{
											D(DBF_STARTUP, "ICON: %s", wbarg->wa_Name);
											if(NameFromLock(wbarg->wa_Lock, buf, sizeof(buf)) && AddPart(buf, (char *)wbarg->wa_Name, sizeof(buf)))
												{
													STRPTR dst;
													LONG len = strlen(buf) + 2L;

													if((Args[FArgNum] = dst = AllocVecPooled(pool, len+1)) != NULL)
														{
															snprintf(dst, len+1, "\"%s\"", buf);
															ArgLen[FArgNum] = len;
														}
													else
														{
															RETURN(ERROR_NO_FREE_STORE);
															return(ERROR_NO_FREE_STORE);
														}
												}
											else
												{
													RETURN(ERROR_LINE_TOO_LONG);
													return(ERROR_LINE_TOO_LONG);
												}
										}
									FArgNum++;
								}
							if(nrdargs->PrgToolTypesOnly && num) continue;
							olddir = CurrentDir(wbarg->wa_Lock);
							dobj =
									#ifdef ICONGETA_RemapIcon
									(((struct Library *)IconBase)->lib_Version >= 44L) ?
									GetIconTagList((char *)wbarg->wa_Name, (struct TagItem *)icontags) :
									#endif
									GetDiskObject((char *)wbarg->wa_Name);
									if(dobj != NULL)
										{
											if(dobj->do_ToolTypes && (dobj->do_Type == WBTOOL || dobj->do_Type == WBPROJECT))
												{
													STRPTR *tarray = (STRPTR *)dobj->do_ToolTypes;

													while(*tarray != NULL)
														{
															if(**tarray != '(')
																{
																	STRPTR src = *tarray;
																	LONG i;

																	//D(DBF_STARTUP, "tooltype: '%s'", *tarray);
																	if((i = IsArg(ptr, src)) > -1)
																		{
																			STRPTR dst;
																			LONG len;

																			i += FileArgs;
																			if(ArgLen[i] == 0L || (i-FileArgs) != MultiArg)
																				{
																					len = strlen(src);
																					if((Args[i] = dst = AllocVecPooled(pool, len+3)) != NULL)
																						{
																							BOOL quotes = FALSE;
                                            
																							while(*src)
																								{
																									if(((*dst++ = *src++) == '=') && (*src != '"'))
																										{
																											*dst++ = Args[i][len+1] = '"';
																											len+=2;
																											quotes = TRUE;
																										}
																								}
																							if(quotes == TRUE) *dst++ = '"';
																							*dst = '\0';
																							ArgLen[i] = len;
																						}
																					else
																						{
																							RETURN(ERROR_NO_FREE_STORE);
																							return(ERROR_NO_FREE_STORE);
																						}
																				}
																			else
																				{
																					while(*src && *src++ != '=' ) ;
																					len = strlen(src) + 1 + ArgLen[i];
																					if((dst = AllocVecPooled(pool, len + 2)) != NULL)
																						{
																							BOOL quotes = FALSE;
																							UBYTE c;

																							memcpy(dst, Args[i], len);
																							Args[i] = dst;
																							dst += ArgLen[i];
																							*dst++ = ' ';
																							if(*src != '"')
																								{
																									quotes = TRUE;
																									*dst++ = '"';
																									len += 2;
																								}
																							while(c = *src++, c)    *dst++ = c;
																							if(quotes) *dst++ = '"';
																							*dst = '\0';
																							ArgLen[i] = len;
																						}
																				}
																		}
																}
															tarray++;
														}
												}
											FreeDiskObject(dobj);
										}
									CurrentDir(olddir);
								}
							for(num = FileArgs = 0; FileArgs < MaxArgs; FileArgs++) num += ArgLen[FileArgs];
							if(num > 0)
								{
									num += MaxArgs;
									nrdargs->RDArgs->RDA_Source.CS_Length = num;
									nrdargs->RDArgs->RDA_Source.CS_Buffer = calloc(1, num+1);
									ptr = (char *)nrdargs->RDArgs->RDA_Source.CS_Buffer;
									if(ptr != NULL)
										{
											int args = 0;

											for(FileArgs = 0; FileArgs < MaxArgs; FileArgs++)
												{
													if(Args[FileArgs] != NULL && ArgLen[FileArgs] > 0)
														{
															if(args > 0) strlcat(ptr, " ", num + 1);
															strlcat(ptr, Args[FileArgs], num + 1);
															args++;
														}
													//D(DBF_STARTUP, "args: %s", ptr);
												}
										}
								}
							else
								{
									nrdargs->RDArgs->RDA_Source.CS_Length = 1;
									nrdargs->RDArgs->RDA_Source.CS_Buffer = calloc(1, 2);
								}
							if(nrdargs->RDArgs->RDA_Source.CS_Buffer != NULL)
								{
									strlcat((char *)nrdargs->RDArgs->RDA_Source.CS_Buffer, "\n", nrdargs->RDArgs->RDA_Source.CS_Length+1);
									//D(DBF_STARTUP, "CS_Buffer[%d]: %s", num, nrdargs->RDArgs->RDA_Source.CS_Buffer);
								}
							else
								{
									E(DBF_STARTUP, "allocation RDA_Source.CS_Buffer(%ld) erron�e", nrdargs->RDArgs->RDA_Source.CS_Length);
									RETURN(ERROR_NO_FREE_STORE);
									return(ERROR_NO_FREE_STORE);
								}
						}
					nrdargs->RDArgs->RDA_ExtHelp = nrdargs->ExtHelp;
					if((nrdargs->FreeArgs = ReadArgs(nrdargs->Template, (APTR)nrdargs->Parameters, nrdargs->RDArgs)) == NULL)
						{
							LONG error = IoErr();

							E(DBF_STARTUP, "ReadArgs() erron�, IoErr()=%ld", error);
							RETURN(error);
							return(error);
						}
					if(SetSignal(0, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
						{
							E(DBF_STARTUP, "ReadArgs() annul� par CTRL-C");
							RETURN(ERROR_BREAK);
							return(ERROR_BREAK);
						}
					//else D(DBF_STARTUP, "ReadArgs() ok");
					if(ToolWindow && WBStartup)
						{
							D(DBF_STARTUP, "WINDOW a �t� d�finie");
							if((nrdargs->WinFH = Open(ToolWindow, MODE_READWRITE)) != (BPTR)NULL)
								{
									D(DBF_STARTUP, "Ouverture WINDOW=%s", ToolWindow);
									nrdargs->OldInput = SelectInput(nrdargs->WinFH);
									nrdargs->OldOutput = SelectOutput(nrdargs->WinFH);
								}
							else
								{
									LONG error = IoErr();

									E(DBF_STARTUP, "Ouverture WINDOW=%s interrompue, IoErr()=%ld", ToolWindow, error);
									RETURN(error);
									return(error);
								}
						}
					if(nrdargs->Args != NULL)
						{
							ULONG i;

							for(i=0; i < nrdargs->MaxArgs; i++)
								{
									//D(DBF_STARTUP, "freevecpooled nrdargs->Pool nrdargs->Args[%d] ...", i);
									if(nrdargs->Args[i] != NULL) FreeVecPooled(nrdargs->Pool, nrdargs->Args[i]);
								}
							//D(DBF_STARTUP, "freevecpooled nrdargs->pool");
							FreeVecPooled(nrdargs->Pool, nrdargs->Args);
							nrdargs->Args = NULL;
						}
					if(nrdargs->ToolWindow != NULL)
						{
							//D(DBF_STARTUP, "nrargs->Pool, nrdargs->ToolWindow");
							FreeVecPooled(nrdargs->Pool, nrdargs->ToolWindow);
							nrdargs->ToolWindow = NULL;
						}
					if(nrdargs->Pool != NULL)
						{
							//D(DBF_STARTUP, "deletepool nrdargs->Pool");
							DeletePool(nrdargs->Pool);
							nrdargs->Pool = NULL;
						}
				}
			else
				{
					RETURN(ERROR_NO_FREE_STORE);
					return(ERROR_NO_FREE_STORE);
				}
		RETURN(RETURN_OK);
		return(RETURN_OK);
}

#else
  #warning "NEED_NEWREADARGS absence ou compilation inutile"
#endif
