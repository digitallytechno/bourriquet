/***************************************************************************
 Bourriquet
  digitally
***************************************************************************/
#ifndef NEWREADARGS_H
#define NEWREADARGS_H

#ifndef WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif
#ifndef WORKBENCH_STARTUP_H
#include <workbench/startup.h>
#endif

struct NewRDArgs
{
    STRPTR Template;        // Modèle de type ReadArgs
    STRPTR ExtHelp;         // Chaîne ExtHelp ou NULL, shell UNIQUEMENT
    STRPTR Window;          // descripteur window WB
    IPTR   *Parameters;     // tableau de stockage des paramètres analysés
    LONG   FileParameter;   // -1 signifie aucun, 0 signifie tous
    LONG   PrgToolTypesOnly;
    struct RDArgs *RDArgs;    // RDArgs que nous donnons à ReadArgs()
    struct RDArgs *FreeArgs;  // RDArgs nous obtenons à ReadArgs()
    STRPTR *Args;
    ULONG MaxArgs;
    STRPTR ToolWindow;
    APTR Pool;
    BPTR WinFH;     // flux de fenêtres d'E/S
    BPTR OldInput;  // anciens flux d'E/S
    BPTR OldOutput;
};

#endif /* NEWREADARGS_H */
