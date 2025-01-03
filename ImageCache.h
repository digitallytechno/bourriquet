/***************************************************************************
Bourriquet 
	digitally
  
***************************************************************************/
#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <exec/nodes.h>
#include <mui/TheBar_mcc.h>
#include "HashTable.h"

struct Screen;
struct BitMap;

// Définition de l'imageCacheNode lequel contient
// toutes les informations du fichier image à charger, 
// incluant l'objet dataype de l'image chargé
struct ImageCacheNode
	{
  		struct HashEntryHeader hash; // Entête standard de la table de hachage
  		char *id;                    // pointeur de l'id
  		char *filename;              // pointeur du nom de fichier
  		Object *dt_obj;              // l'objet datatype
  		struct Screen *screen;       // pointeur de lécran de l'image
  		int openCount;               // compte combien de fois l'image est ouverte/utilisée
  		APTR pixelArray;             // pointeur de lecture de la matrice de pixels par PDTM_READPIXELARRAY
  		ULONG pixelFormat;           // le format de pixel de la matrice(PBPAFMT_ARGB)
  		struct BitMap *bitmap;
  		PLANEPTR mask;
  		ULONG width;
  		ULONG height;
  		ULONG depth;
  		ULONG bytesPerPixel;
  		ULONG bytesPerRow;
  		UBYTE masking;
  		BOOL initialLayout;          // avons nous appelé DTM_PROCLAYOUT déjà avant ?
  		BOOL delayedDispose;         // voulons-nous supprimer l'image à partir du cache si openCount atteint zéro ?
	};

// Fonctions publiques
BOOL ImageCacheSetup(void);
void ImageCacheCleanup(void);
struct ImageCacheNode *ObtainImage(const char *id, const char *filename, const struct Screen *scr);
void ReleaseImage(const char *id, BOOL dispose);
BOOL IsImageInCache(const char *id);

#if defined(DEBUG)
void DumpImageCache(void);
#endif

enum TBType  { TBT_ReadWindow, TBT_WriteWindow, TBT_AbookWindow };
enum TBImage { TBI_Normal, TBI_Ghosted, TBI_Selected };

BOOL ToolbarCacheInit(void);
void ToolbarCacheCleanup(void);
struct MUIS_TheBar_Brush **ObtainToolbarImages(const enum TBType toolbar, const enum TBImage image);
BOOL IsToolbarInCache(const enum TBType toolbar);

#endif // IMAGECACHE_H
