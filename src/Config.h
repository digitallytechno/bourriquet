/***************************************************************************
Bourriquet 
	digitally
 
***************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#include <exec/lists.h>
#include <libraries/mui.h>
#include "Stringsizes.h"

struct Config
	{
  		int version; // la version du fichier de configuration chargé
  		struct MinList DownloadList;   // liste des fichiers du répertoire de fichiers téléchargés
  		struct MinList TempList;   // liste des fichiers temporaires
  		struct MinList ShareList;       // liste des fichiers à partager de disponible
 		char UserName[SIZE_REALNAME];
    	int ClientPort;
		BOOL ConfigIsSaved;
    	char DownloadDir[SIZE_PATH];  // Répertoire des fichiers téléchargés (download)
    	char TempDir[SIZE_PATH];  // Répertoire des fichiers temporaires (temp)
    	char ShareDir[SIZE_PATH]; // Répertoire des fichiers à partager (upload)
		char ThemeName[SIZE_FILE];
};

extern struct Config *C;
extern struct Config *CE;

struct Config *AllocConfig(void);
void FreeConfig(struct Config *co);
void ClearConfig(struct Config *co);
int LoadConfig(struct Config *co, const char *fname);
BOOL SaveConfig(struct Config *co, const char *fname);
BOOL IsValidConfig(const struct Config *co);
void ValidateConfig(struct Config *co, BOOL update, BOOL saveChanges);

#endif /* CONFIG_H */
