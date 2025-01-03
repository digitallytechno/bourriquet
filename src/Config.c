/***************************************************************************
Bourriquet 
	digitally

***************************************************************************/
#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/diskfont.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "Bourriquet.h"
#include "Error.h"
#include "Global.h"
#include "Busy.h"
#include "Config.h"
#include "FileInfo.h"
#include "Locale.h"
#include "MUIObjects.h"
#include "Requesters.h"
#include "Debug.h"

struct Config *C = NULL;
struct Config *CE = NULL;

#define LATEST_CFG_VERSION 1

/// InitConfig
// initialise une structure de configuration
static void InitConfig(struct Config *co)
{
 	ENTER();
  	NewMinList(&co->DownloadList);
  	NewMinList(&co->TempList);
  	NewMinList(&co->ShareList);
   	LEAVE();
}

///
/// AllocConfig
// alloue et initialise une structure de configuration
struct Config *AllocConfig(void)
{
 	struct Config *co;

  	ENTER();
  	if((co = calloc(1, sizeof(*co))) != NULL) InitConfig(co);
  	RETURN(co);
  	return co;
}

///
/// FreeConfig
// Lib�re une structure de configuration
void FreeConfig(struct Config *co)
{
 	ENTER();
  	if(co != NULL)
  		{
    		ClearConfig(co);
    		free(co);
  		}
  	LEAVE();
}

///
/// ClearConfig
// Nettoie le contenu d'une struture de configuration 
void ClearConfig(struct Config *co)
{
 	ENTER();
  	// Nettoie et initialise une configuration
  	memset(co, 0, sizeof(*co));
  	InitConfig(co);
  	LEAVE();
}

///
/// LoadConfig
// Charge une configuration � partir d'un fichier. renvoie 1 en cas de succ�s, 0 en cas d'erreur et -1 si le fichier de configuration valide n'est pas trouv�
int LoadConfig(struct Config *co, const char *fname)
{
 	int result = -1;
  	FILE *fh;

  	ENTER();
	// signaler cette configuration comme n'�tant pas (correctement) enregistr�e par avance, 
	// juste au cas o� quelque chose se passerait mal.
  	co->ConfigIsSaved = FALSE;
  	if((fh = fopen(fname, "r")) != NULL)
  		{
    		char *buf = NULL;
    		size_t buflen = 0;

			D(DBF_CONFIG, "Charger configuration � partir de '%s'", fname);
    		setvbuf(fh, NULL, _IOFBF, SIZE_FILEBUF);
    		if(getline(&buf, &buflen, fh) >= 3 && strnicmp(buf, "BCO", 3) == 0)
    			{
      				int version = atoi(&buf[3]);

      				// se souvenir de la version charg�e comme r�f�rence ult�rieure
      				co->version = version;
      				while(getline(&buf, &buflen, fh) > 0)
      					{
        					char *p;
        					char *value;
        					const char *value2 = "";

        					// Trouver le s�parateur "="
        					if((value = strchr(buf, '=')) != NULL)
        						{
									for(value2 = (++value)+1; isspace(*value); value++)	;
								}
        					// rechercher de a \r ou \n et terminer la cha�ne ici
        					if((p = strpbrk(buf, "\n")) != NULL) *p = '\0';
							// rechercher au d�but de buf jusqu'� = ou un espace et
							// termine plus t�t avec NULL la cha�ne afin que buf[]
							// ne contienne que l'identifiant de configuration
							for(p = buf; *p != '\0' && *p != '=' && !isspace(*p); p++) ; *p = '\0';
							// maintenant nous passons en revue nos options de configurations potentielles
							// et nous v�rifions si le nom correspond � celui stock� dans buf
        					if(IsStrEmpty(buf) == FALSE && value != NULL)
        						{
									/* G�n�ral */
									if(stricmp(buf, "UserName") == 0)  
										{
											strlcpy(co->UserName, value, sizeof(co->UserName));
											D(DBF_CONFIG, "UserName '%s'", co->UserName);
										}
									else if(stricmp(buf, "ClientPort") == 0)  
										{
											co->ClientPort = atoi(value);
											D(DBF_CONFIG, "ClientPort '%d'", co->ClientPort);
										}
									/* Mixte */
          							else if(stricmp(buf, "TempDir") == 0)                  
										{
											strlcpy(co->TempDir, value, sizeof(co->TempDir));
											D(DBF_CONFIG, "tempdir '%s'", co->TempDir);
										}
          							else if(stricmp(buf, "DownloadDir") == 0)                
										{
											strlcpy(co->DownloadDir, value, sizeof(co->DownloadDir));
											D(DBF_CONFIG, "DownloadDir '%s'", co->DownloadDir);
										}
          							else if(stricmp(buf, "ShareDir") == 0)                
										{
											strlcpy(co->ShareDir, value, sizeof(co->ShareDir));
											D(DBF_CONFIG, "ShareDir '%s'", co->ShareDir);
										}
									/* Look&Feel */
          							else if(stricmp(buf, "Theme") == 0)  strlcpy(co->ThemeName, value, sizeof(co->ThemeName));
									/*Mise � jour*/
          							//else if(stricmp(buf, "UpdateInterval") == 0)  co->UpdateInterval = atoi(value);
        						}
						}
				}
    		else E(DBF_CONFIG, "Ne peut pas trouver l'en-t�te de configuration typique de Bourriquet dans la premi�re ligne");
    		fclose(fh);
    		free(buf);
  		}
  	RETURN(result);
  	return result;
}		

///
/// SaveConfig
// Sauvegarde la configuration dans un fichier
BOOL SaveConfig(struct Config *co, const char *fname)
{
 	BOOL result = FALSE;
 	FILE *fh;

 	ENTER();
 	D(DBF_CONFIG, "Sauvegarde la configuration dans '%s'", fname);
 	co->ConfigIsSaved = FALSE;
 	if((fh = fopen(fname, "w")) != NULL)
  		{
			setvbuf(fh, NULL, _IOFBF, SIZE_FILEBUF);
   			fprintf(fh, "BCO%d - Bourriquet configuration\n", LATEST_CFG_VERSION);
   			fprintf(fh, "# ['%s (%s)']\n", bourriquetversion, bourriquetversiondate);
   			fprintf(fh, "\n[Mixed]\n");
   			fprintf(fh, "TempDir            = %s\n", co->TempDir);
   			fprintf(fh, "DownloadDir          = %s\n", co->DownloadDir);
   			fprintf(fh, "ShareDir          = %s\n", co->ShareDir);
   			// analyse si nous ne rencontrons r�ellement aucunes erreurs durant les diff�rentes op�rations d'�critures
   			if(ferror(fh) == 0)
 				{
      				result = TRUE;
      				// se souvenir que c'est cette configuration qui a �t� sauvegard� 
      				co->ConfigIsSaved = TRUE;
      				D(DBF_CONFIG, "La configuration a �t� sauvegard� avec succ�s");
      				// ajouter quelque chose au fichier journal
      				//AppendToLogfile(LF_VERBOSE, 60, GetStr(MSG_LOG_SavingConfig), fname);
    			}
    		else E(DBF_CONFIG, "Erreur durant l'op�ration de sauvegarde de la configuration");
    		fclose(fh);
  		}
  	//else ER_NewError(GeStr(MSG_ER_CantCreateFile), fname);
  	RETURN(result);
  	return result;
}

/// IsValidConfig
// V�rifie si les r�glages ont �t� effectu�s
BOOL IsValidConfig(const struct Config *co)
{
 	BOOL valid = TRUE;

  	ENTER();
	RETURN(valid);
  	return(valid);
}