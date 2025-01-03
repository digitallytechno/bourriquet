/***************************************************************************
Bourriquet 
	digitally

***************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

extern const char* const months[12];
extern const char* const SecCodes[5];
extern const char* const SigNames[3];
extern const char* const wdays[7];
extern const char* const bourriquetver;
extern const char* const bourriquetversion;
extern const char* const bourriquetversionver;
extern const char* const bourriquetversionstring;
extern const char* const bourriquetuseragent;
extern const char* const bourriquetbuildid;
extern const char* const bourriquetversiondate;
extern const char* const bourriquetcopyright;
extern const char* const bourriquetfullcopyright;
extern const char* const bourriquetcompiler;
extern const char* const bourriqueturl;
extern const unsigned long bourriquetversiondays;
extern struct WBStartup * WBmsg;

// Transforme une définition en chaîne
#define STR(x)  STR2(x)
#define STR2(x) #x

#endif 
