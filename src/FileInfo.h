/***************************************************************************
Bourriquet  
	digitally
 
***************************************************************************/
#ifndef FILEINFO_H
#define FILEINFO_H 1

#include <exec/types.h>

// Information possible de disponible pour la recherche
enum FileInfo
	{
  		FI_SIZE = 0 ,
  		FI_PROTECTION,
  		FI_COMMENT,
  		FI_DATE,
  		FI_TIME,
  		FI_TYPE,
	};

// valeurs de retour possibles pour FI_TYPE
enum FType
	{
  		FIT_NONEXIST = 0,
  		FIT_UNKNOWN,
  		FIT_FILE,
  		FIT_DRAWER
	};

BOOL ObtainFileInfo(const char *name, enum FileInfo which, void *valuePtr);
BOOL FileExists(const char *filename);

#endif /* FILEINFO_H */
