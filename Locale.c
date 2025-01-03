/***************************************************************************
 Bourriquet 
 	digitally
 
***************************************************************************/
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/codesets.h>

#include "Locale.h"
#include "Bourriquet.h"
#include "Debug.h"

struct fcstr Bourriquet_Strings[182] =
	{
  		{ "Project", NULL, 0 },
  		{ "About Bourriquet...", NULL, 1 },
  		{ "About MUI...", NULL, 2 },
  		{ "About Bourriquet...", NULL, 3 },
  		{ "M\000MUI...", NULL, 4 },
  		{ "Hide", NULL, 5 },
  		{ "Quit", NULL, 6 },
  		{ "\nBourriquet \n (Emule for MorphOS)\n digitally\n", NULL, 7 },
  		{ "Version Bourriquet", NULL, 8 },
  		{ "Compilation date :", NULL, 9 },
  		{ "This program is free software\n", NULL, 10 },
  		{ "This program is free software\n", NULL, 11 },
  		{ "Emule for MorphOS", NULL, 12 },
  		{ "_Ok", NULL, 13 },
  		{ "_Cancel", NULL, 14 },
  		{ "_Yes", NULL, 15 },
  		{ "_No", NULL, 16 },
  		{ "Creating user interface...", NULL, 17 },
  		{ "Initialising libraries...", NULL, 18 },
  		{ "Initialize timer", NULL, 19 },
  		{ "Loading graphics...", NULL, 20 },
  		{ "Loading configuration...", NULL, 21 },
  		{ "Couldnt open MUI custom class %s V%lu.%lu.\nThe class is not installed.", NULL, 22 },
  		{ "Couldnt open MUI custom class %s V%lu.%lu.\n\nCurrently installed is V%lu.%lu, please upgrade!", NULL, 23 },
  		{ "Couldnt open MUI custom class %s V%lu.%lu.\n\nCurrently installed is V%lu.%lu, please upgrade!", NULL, 24 },
  		{ "Try again|Quit", NULL, 25 },
  		{ "Opening user interface...", NULL, 26 },
  		{ "Do you really want to quit Bourriquet ?", NULL, 27 },
  		{ "Confirmation request", NULL, 28 },
  		{ "*_Yes|_No", NULL, 29 },
  		{ "Bourriquet %s", NULL, 30 },
  		{ "Quit", NULL, 31 },
  		{ "Error during startup", NULL, 32 },
  		{ "Cant open %s, version v%lu.%lu", NULL, 33 },
  		{ "Bourriquet cant create a apllication", NULL, 34 },
  		{ "Bourriquet cant create MUI custom classes", NULL, 35 },
  		{ "Bourriquet cant init timer", NULL, 36 },
  		{ "Bourriquet cant init task", NULL, 37 },
  		{ "Bourriquet cant init signal", NULL, 38 },
  		{ "Bourriquet cant init methodtask", NULL, 39 },
  		{ "Bourriquet cant init threads", NULL, 40 },
  		{ "Bourriquet cant find url lib %s, V%lu.%lu %s", NULL, 41 },
  		{ "Bourriquet cant create semaphore", NULL, 42 },
  		{ "Quit home page", NULL, 43 },
  		{ "Current Developers:", NULL, 44 },
  		{ "Contributors:", NULL, 45 },
  		{ "Localization Contributors:", NULL, 46 },
  		{ "This program is free software; youre welcome to\ndistribute and or modify it under the terms of the\nGNU Public License. See the enclosed documents for\nmore details.", NULL, 47 },
  		{ "This program uses the following third party software:", NULL, 48 },
  		{ "The latest news about Bourriquet can be found at the\nBourriquet Support Site at \0335http:///\033n", NULL, 49 },
  		{ "About Bourriquet", NULL, 50 },
  		{ "Shutting down Bourriquet...", NULL, 51 },
  		{ "Bourriquet warning...", NULL, 52 },
  		{ "While trying to cleanup Bourriquet still running sub threads\nhave been found.\nDo you want to retry to quit Bourriquet or ignore the still\nrunning sub thread (which could end up in later crashes)?", NULL, 53 },
  		{ "_Retry|_Ignore", NULL, 54 },
  		{ "Connect", NULL, 55 },
  		{ "Disconnect", NULL, 56 },
  		{ "Server", NULL, 57 },
  		{ "Transfer", NULL, 58 },
  		{ "Research", NULL, 59 },
  		{ "Shared", NULL, 60 },
  		{ "Preferences", NULL, 61 },
  		{ "Server List", NULL, 62 },
  		{ "Our gateway to the Edonkey network", NULL, 63 },
  		{ "Server List", NULL, 64 },
  		{ "Server List (%d)", NULL, 65 },
  		{ "New server", NULL, 66 },
  		{ "IP", NULL, 67 },
  		{ "Port", NULL, 68 },
  		{ "Name", NULL, 69 },
  		{ "Add server", NULL, 70 },
  		{ "Update server.met since URL", NULL, 71 },
  		{ "Update", NULL, 72 },
  		{ "My infos", NULL, 73 },
  		{ "Name", NULL, 74 },
  		{ "IP", NULL, 75 },
  		{ "Description", NULL, 76 },
  		{ "Ping", NULL, 77 },
  		{ "Users", NULL, 78 },
  		{ "Max Users", NULL, 79 },
  		{ "Files", NULL, 80 },
  		{ "Prefs", NULL, 81 },
  		{ "Failed", NULL, 82 },
  		{ "Static", NULL, 83 },
  		{ "SoftFiles", NULL, 84 },
  		{ "HardFiles", NULL, 85 },
  		{ "Version", NULL, 86 },
  		{ "LowIDUsers", NULL, 87 },
  		{ "Server infos", NULL, 88 },
  		{ "Log", NULL, 89 },
  		{ "Low", NULL, 90 },
  		{ "Normal", NULL, 91 },
  		{ "High", NULL, 92 },
  		{ "Connect to this server", NULL, 93 },
  		{ "Priority", NULL, 94 },
  		{ "Add to the list of static servers", NULL, 95 },
  		{ "Remove to the list of static servers", NULL, 96 },
  		{ "Erase this server", NULL, 97 },
  		{ "Research List", NULL, 98 },
  		{ "The Ali Baba cave of the network Edonkey", NULL, 99 },
  		{ "Search parameters", NULL, 100 },
  		{ "File name", NULL, 101 },
  		{ "File type", NULL, 102 },
  		{ "Start", NULL, 103 },
  		{ "Abort", NULL, 104 },
  		{ "Erase all", NULL, 105 },
  		{ "Search results", NULL, 106 },
  		{ "File name", NULL, 107 },
  		{ "Size", NULL, 108 },
  		{ "Availability", NULL, 109 },
  		{ "Full", NULL, 110 },
  		{ "Type", NULL, 111 },
  		{ "Artist", NULL, 112 },
  		{ "Album", NULL, 113 },
  		{ "Title", NULL, 114 },
  		{ "Duration", NULL, 115 },
  		{ "Rate", NULL, 116 },
  		{ "Codec", NULL, 117 },
  		{ "Known", NULL, 118 },
  		{ "List of receipts and emissions", NULL, 119 },
  		{ "Receive and exchange your files freely", NULL, 120 },
  		{ "Downloads", NULL, 121 },
  		{ "Name", NULL, 122 },
  		{ "Size", NULL, 123 },
  		{ "Done", NULL, 124 },
  		{ "Speed", NULL, 125 },
  		{ "Progress", NULL, 126 },
  		{ "Source", NULL, 127 },
  		{ "Priority", NULL, 128 },
  		{ "Status", NULL, 129 },
  		{ "Remaining", NULL, 130 },
  		{ "AddTo", NULL, 131 },
  		{ "Uploads", NULL, 132 },
  		{ "Name User", NULL, 133 },
  		{ "File", NULL, 134 },
  		{ "Speed", NULL, 135 },
  		{ "Transferred", NULL, 136 },
  		{ "Excepted", NULL, 137 },
  		{ "Send Time", NULL, 138 },
  		{ "Status", NULL, 139 },
  		{ "Parts obtained", NULL, 140 },
  		{ "List of shared files", NULL, 141 },
  		{ "Your treasures to share on the Edonkey network", NULL, 142 },
  		{ "File name", NULL, 143 },
  		{ "Size", NULL, 144 },
  		{ "Type", NULL, 145 },
  		{ "Priority", NULL, 146 },
  		{ "Requests", NULL, 147 },
  		{ "Tranferred", NULL, 148 },
  		{ "Shared parts", NULL, 149 },
  		{ "Complete", NULL, 150 },
  		{ "Share EDonkey", NULL, 151 },
  		{ "Configuration", NULL, 152 },
  		{ "Configuration of Bourriquet", NULL, 153 },
  		{ "Save", NULL, 154 },
  		{ "Use", NULL, 155 },
  		{ "Abort", NULL, 156 },
  		{ "General", NULL, 157 },
  		{ "User Name", NULL, 158 },
  		{ "Client Port", NULL, 159 },
  		{ "Directories", NULL, 160 },
  		{ "Paths", NULL, 161 },
  		{ "Incoming directory", NULL, 162 },
  		{ "Temporary directory", NULL, 163 },
  		{ "Shared directory", NULL, 164 },
  		{ "Themes", NULL, 165 },
  		{ "Appearance", NULL, 166 },
  		{ "Activate theme", NULL, 167 },
  		{ "Preview", NULL, 168 },
  		{ "Author", NULL, 169 },
  		{ "Url", NULL, 170 },
  		{ "Information", NULL, 171 },
  		{ "Update", NULL, 172 },
  		{ "Updating the application and its components", NULL, 173 },
  		{ "Update check frequency", NULL, 174 },
  		{ "Never", NULL, 175 },
  		{ "Daily", NULL, 176 },
  		{ "Weekly", NULL, 177 },
  		{ "Monthly", NULL, 178 },
  		{ "Search Now", NULL, 179 },
  		{ "Abort", NULL, 180 },
  		{ "end", NULL, 181 }
	};

/*** fonctions du catalogue ***/
/// InitCatalogue()
void InitCatalogue(void)
	{
  		int i;
  		struct fcstr *fc;

  		ENTER();
  		for(i=0, fc = (struct fcstr *)Bourriquet_Strings;  i < 182;  i++, fc++)
  			{
    			fc->msg = fc->defmsg;
  			}
 	 	LEAVE();
	}

char *GetStr(APTR fcstr)
	{
		char *defaultstr = ((struct fcstr *)fcstr)->defmsg;

		if(LocaleBase)
			{
				return (char *)GetCatalogStr(G->Catalog, ((struct fcstr *)fcstr)->id, (STRPTR)defaultstr);
			}
	  	return defaultstr;
	}

///
/// OpenCatalogue()
BOOL OpenCatalogue(void)
	{
  		BOOL retry = TRUE;
  		BOOL expunged = FALSE;
  		BOOL success = TRUE;
  		struct fcstr *fc = NULL;
  		int i = 0;

  		ENTER();
  		if(G->Locale != NULL && G->Catalog == NULL)
  			{
    			do
    				{
      					if(G->NoCatalogTranslation == FALSE)
      						{
        						G->Catalog = OpenCatalog(NULL,
                                 							(STRPTR)"Bourriquet.catalog",
                                 							OC_BuiltInLanguage, "english",
                                 							OC_BuiltInCodeSet, 4, // 4 = iso-8859-1
                                							TAG_END);
      						}
      					if(G->Catalog != NULL)
      						{
        						if(G->Catalog->cat_Version != 1)
        							{
          								W(DBF_STARTUP, "La version (%d) du catalogue est différente de la version (-1) requise", G->Catalog->cat_Version);
          								CloseCatalog(G->Catalog);
          								G->Catalog = NULL;
								        if(expunged == FALSE)
          									{
            									RemLibrary((struct Library *)LocaleBase);
            									//D(DBF_STARTUP, "expunged locale.library");
            									expunged = TRUE;
            									retry = TRUE;
          									}
          								else
          									{
            									struct EasyStruct es;

            									DisplayBeep(NULL);
            									es.es_StructSize = sizeof(es);
            									es.es_Flags      = 0;
									            es.es_Title        = (STRPTR)"Version de Bourriquet.catalog incompatible!";
            									es.es_TextFormat   = (STRPTR)"Bourriquet a trouvé un fichier catalogue de traduction dans votre langue\n"
                                                                                  		"Il ne peut pas être activé car cette version est trop vieille\n"
																						"avec la version nécessaire de Bourriquet\n";
            									es.es_GadgetFormat = (STRPTR)"Retry|Continue|Quit";
									            switch(EasyRequestArgs(NULL, &es, NULL, NULL))
            										{
              											default:
              												case 0: 
                												retry = FALSE;
                												success = FALSE;
              													break;
											              case 1: 
                												retry = TRUE;
                												expunged = FALSE;
              													break;
											              case 2: // continue
                												retry = FALSE;
                												success = TRUE;
              													break;
            										}
          									}
        							}
        						else
        							{
          								D(DBF_STARTUP, "Bourriquet.catalog v%d.%d (language: '%s', codeset: %d) ouvert avec succés", G->Catalog->cat_Version, G->Catalog->cat_Revision, G->Catalog->cat_Language, G->Catalog->cat_CodeSet);
          								retry = FALSE;
          								success = TRUE;
        							}
      						}
      					else if(G->NoCatalogTranslation == FALSE)
      						{
        						if(IoErr() != 0) W(DBF_STARTUP, "Bourriquet ne peut pas ouvrir 'Bourriquet.catalog': IoErr: %d", IoErr());
        						else W(DBF_STARTUP, "Bourriquet ne pourra pas ouvrir 'Bourriquet.catalog': les phrases prédéfinies seront utilisées à la place");
						        retry = FALSE;
      						}
    				}
    			while(retry == TRUE);
  			}
  		else W(DBF_STARTUP, "La locale.library n'était pas ouverte ou le catalogue est déjà ouvert.");
  		if(G->Locale != NULL && G->Catalog != NULL)
  			{
    			for(i=0, fc = (struct fcstr *)Bourriquet_Strings;  i < 182;  i++, fc++)
    				{
      					const char *cs;

      					cs = GetCatalogStr(G->Catalog, fc->id, (STRPTR)fc->defmsg);
      					fc->msg = CodesetsUTF8ToStr(CSA_Source, cs, TAG_DONE);
    				}
  			}
  		RETURN(success);
  		return success;
	}

///
/// CloseCatalogue()
void CloseCatalogue(void)
	{
  		int i;
  		struct fcstr *fc;

  		ENTER();
  		for(i=0, fc = (struct fcstr *)Bourriquet_Strings;  i < 182;  i++, fc++)
  			{
    			if(fc->msg != NULL && fc->msg != fc->defmsg)
    				{
      					CodesetsFreeA((STRPTR)fc->msg, NULL);
      					fc->msg = fc->defmsg;
   		 			}
  			}
  		if(G->Locale != NULL && G->Catalog != NULL)
  			{
    			CloseCatalog(G->Catalog);
    			G->Catalog = NULL;
 	 		}
  		LEAVE();
	}

///
