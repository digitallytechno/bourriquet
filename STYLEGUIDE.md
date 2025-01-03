# ENGLISH VERSION *********************************************************************

 Bourriquet
 Copyright (C) digitallytechno

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

***************************************************************************/

This document is meant to be a general coding styleguide for development on
Bourriquet. It carries information on how to structure and layout the source code
during development. All registered developers of Bourriquet should at least have
read once through this guide and stick to the particular rules and
suggestions on how to structure particular code passages. That way future
developers may more easily find a way into the complex source code of Bourriquet
and take it as a reference on how to find out the Bourriquet developers expect
their source code to look like.

So please, any Bourriquet developer, try to stick to the rules listed in this guide
as much as possible so that the code will stay readable even if more than
one developer is working on a certain section.

############################################################################

1. Indentation - Tabulator use (TAB)
------------------------------------

In the past we had too much trouble with excessive tabulator use in the
source code of Bourriquet. Therefore, any use of tabulator characters (0x09) to
structure the source code should be considered FORBIDDEN. Instead, we agree
that for indentation of source code we use TWO (2) spaces (0x20) instead:

Example:

   win = OpenWindowTags(NULL,
                        WA_Title,       "Test",
                        WA_InnerWidth,  width,
                        WA_InnerHeight, height,
                        TAG_DONE);

^^^^^^^^^^^^^^^^^^^^^^^^ - 24 (12x2) spaces / *NO* tabs

############################################################################

2. Blocks
---------

Blocks start on a new line, with the same indentation as the previous line,
and end likewise. Example:

  for(i=0; i < 10; i++)
    {
      ...code...
    }

The braces can be omitted for one-line blocks. E.g.

  for(i=0; i < 10; i++) array[i] += 30;

One exception from this rule is the do-while. These should be formatted like
this:

  do
    {
      ...code...
    }
    while(a < 10);

############################################################################

3. if/then
----------

Normal block rules apply. However, if both branches of an if-else statement
are one line blocks, then they should be formatted like this:

  if(a < 20) Foo();
  else Bar();

If-then-else-if should be merged like this:

  if(a < 20)
    {
      ...code...
    }
  else if(a < 40)
    {
      ...code...
    }

All blocks in a merged if-then-else-if sequence should share the same style
when it comes to have their braces or not, where the latter is then only
possible when all branches are one line blocks.

############################################################################

4. Tuples
---------

A tuple is one parenthesis immediately followed by first member. If more
members are present, they should be separated with ", ". Example:

  (unsigned long first, unsigned long second, unsigned long last)
                       ^                     ^
                       +---------------------+- note the spaces here !

############################################################################

5. Functions
------------

Functions are always proper case, meaning the first letter is a capital
letter. If the function name is made up of several words, they should
also start with a capital letter.

There is one space after the return type, unless this is a pointer
(marked by an asterisk), then the space is between the type and the
asterisk. There is NO space between the function name and the opening
pharatesis

There is exactly one blank line between functions.

Sample functions:

  int AllocateBuffer(unsigned long size)
  {
    ...code...
  }

or

  char *AllocateArray(unsigned long size)
  {
    ...code...
  }

############################################################################

6. return
---------

A function should only have AN exit point at the end of that function.
An exception to this rule is resource allocation or similar sanity checks
placed at the beginning of the function (at outermost scope). Here one can
immediately return a failure. Example:

  BOOL IsLegal(char *str)
  {
    if(str == NULL) return(FALSE);

    ...code...

    return(res);
  }

The rule says �...*AN* exit point at the end� rather than *one*. This allows
to make a function with several exit points all placed at the end, like this:

  BOOL IsWhitespace(char *character)
  {
    if(character == ' ' || ...) return(TRUE);
    else return(FALSE);
  }

Just for the record: return is a flow-control-directive similar to case,
break, continue and goto, and therefore the value provided should not be
enclosed between parenthesis.

############################################################################

7. No "goto" statements
-----------------------

Because general goto usage tend to obfuscate source code and end up in
"spaghetti code" goto statements/use Bourriquet should generally be avoided. Even
if that ends up in more deep if() arrangements, we believe it is a better
practice to restructure your source code instead of being lazy and entering
a goto statements together with a label.

############################################################################

8. No "typedef" statements
--------------------------

As a general "typedef" usage tends to obfuscate source code and make it
unreadable for the highly trained developer, we believe we should avoid
using typedef's at all. There are plenty of other ways to achieve the very
same (especially with enums). So please consider NOT using any own typedef
statements in your code and keep on using the native types.

############################################################################

9. Use "enum" types wherever possible
-------------------------------------

Instead of using a plain "int" type variable, enumeration types (enum)
should be preferred. Even for local status variables enums should be
preferred over plain "int" status variables. So, by no means you should use
an integer to keep the status of a certain operation.

So, DON'T do something like:

  BOOL IsRed(int color)
  {
    if(color == 0) return(TRUE);
  }
    
A proper version of the same function would be:

  BOOL IsRed(enum Color color)
  {
    if(color == CL_Red) return(TRUE);
  }
  
In addition you should also use enum types in function definitions and don't
simply define them as an "int" variables.

So, DON'T do something like:

  BOOL IsRed(int color)
  {
    if(color == CL_Red) return(TRUE);
  }

But use "enum Color color" as shown in the second example above. By using
enum types in your functions you make it more easy for a developer to
understand and track the meaning of the variable. In addition, by using
enum types in functions you will allow the compiler to warn you in case
you may have forgotten to catch a use case (e.g. in switch statements).

############################################################################

10. switch/case
---------------

A switch case looks like this:

  switch(value)
    {
      case ONE:
        {
          ...code...
        }
        break;
      case TWO:
        {
          ...code...
        }
        // continue
      default:
        {
          ...code...
        }
        break;
    }

If a case needs to fall through to the next one, then a comment should be
put instead of the break, as in the example above. The text within this
comment is situation-specific, but generally 'continue' will do.

The case follows normal block rules, but extends the rule about braces
being optional for one-line blocks.

Whenever possible, a switch-case should use symbolic names rather than
numbers. This also applies to state-machines implemented through
if-then-else-if, even if there are only two states.

If a set of cases exists with only one command, then one can write a
block of compact cases. The colons and semi-colons should be aligned
with one space after each and one space in front of each on the longest
line. It is allowed to have a compact block of cases within a normal
switch/case sequence. Example:

  switch(value)
    {
      case ONE:   number = "one";   break;
      case TWO:   number = "two";   break;
      case THREE: number = "three"; break;
      case FOUR:  number = "four";  break;
      case FIVE:  number = "five";  break;
      case FOURTYTWO:
        {
          ...code...
        }
        break;
    }

############################################################################

11. Variables
-------------

Variables should generally have as meaningful names as possible. Avoid
custom abbreviations but do use common ones (like scr, win, obj etc.).
Do not use negating words like 'no' in a boolean value but try instead
to use something like 'is', as in "isUppercase". This applies both to
boolean values and functions which return a boolean.

Local variables and function arguments are either completely lowercase or
camelon notation, where the first letter is lowercase and the first
letter of the following words is uppercase, as shown in the previous
example.

Defines (both constants and macros) are completely uppercase. However, if
the defines are such as like "#define isUppercase" then a mixed case
writing is allowed.

Multiple variable definitions on a single line should be avoided and better
be expanded to multiple lines for better readability.

Example:

  struct CustomEntry
  {
    char *Name;
    char *Address;
  };

rather than

  struct CustomEntry
  {
    char *Name, *Address;
  };

Enumerations, global variables and structure names and members are proper
case as defined under "Functions". Generally static variables should also
comply with this rule (when they are really meant as global, but local not
to clutter the global namespace).

As we want to introduce multithreading to Bourriquet sooner or later please avoid
using static Variables. Instead of that use our "struct Global" which can
be easily locked by semaphores. 

Abbreviations which normally appear in uppercase (e.g. HTML) should still
be uppercase despite the above rules.

Some examples:

  #define ABS(x) ((x) < 0 ? -(x) : (x))
  #define PI     3.14159265358979323846

  Object *GlobalConfig = DataspaceObject, ..., End;

  struct CustomEntry *CreateEntry(char *name, char *address)
  {
    struct CustomEntry *res;

    if((res = malloc(sizeof(struct CustomEntry)))
      {
        res->Name = name;
        res->Address = address;
      }
    return(res);
  }

############################################################################

12. Use "const" on variables where ever possible
-----------------------------------------------

By using "const" on variable definitions you give the compiler an additional
hint on the usage of these variables. For modern compilers, this allows to
tune the optimization and not only make faster code but also provides
a more secure environment. While this will allow the compiler to warn you
(as the developer) on cases where you might have accidentally accessed the
variable in write mode, it will also allow the compiler to move the data
section of that variable into the read-only part of the executable and thus
make it secure for external modifications.

Therefore, where ever possible you should use "const" on variables from which
you know will never change their content.

So, instead of doing something like:

  BOOL IsRed(enum Color color)
  {
    if(color == CL_Red) return(TRUE);
  }
 
You should define the function like:

  BOOL IsRed(const enum Color color)
  {
    if(color == CL_Red) return(TRUE);
  }
 
This will allow the compiler to warn you in case you are trying to
do some local modifications to the "color" variable.

############################################################################

13. Custom Class naming
-----------------------

Since we manage our own MUI custom classes in the 'src/classes' directory
of Bourriquet, we try to make things clear as much as possible. One thing is the
naming of the Bourriquet internal custom classes.

We have the convention that internal MUI classes of Bourriquet should be put into
the 'classes' subdirectory and together with the file naming, the internal
class should use the same 'camelon notation' like we use for variables.

Example: "TextEditor" instead of "Texteditor".

In addition, we believe the class/file-name itself should clearly show from
which superclass a particular class was inherited. For example, if you
generate a new class which is inherited from MUIC_Group, we highly suggest
to use a name ending with "Group".

############################################################################

14. Types
---------

We mostly use standard C-types wherever possible. This is 'char *' for
STRPTR, 'char' for TEXT, 'void' for VOID and 'void *' for APTR. However,
for certain Amiga specific tasks we stick to the special Amiga types. For
Example, for defining MUI objects we use "Object *" rather than 'void *',
but also rather than APTR.

Further, although Bourriquet is currently ANSI-C, one should always use proper
types, so that we may later compile the project with a C++ compiler.
This means that if you e.g. have an IntuiMessage pointer that must be
supplied to 'ReplyMsg()', you should use the format
'ReplyMsg(&imsg->ExecMessage)'.

For strings embedded in structures, pass a pointer to first character,
as this also serves as extra information for the person who reads the
source. For example,

  struct Server
  {
    char IP[SIZE_DEFAULT];
    char Name[SIZE_NAME];
  };

  ...

  struct Server *se = SomeServer();
  printf("Letter came from %s\n", &se->IP[0]);

as opposed to:

  printf("Letter came from %s\n", se);

############################################################################

15. Taglists
------------

When providing taglists, one can either put all tags on the same line
(if only a few tags are supplied), or put one tag on each line, which
is then indented with as many spaces until the opening pharatesis of the
taglist function. When putting one tag on its own line, the first line that
contain the 'receiver' should NOT contain a tag.

The comma should come immediately after the tag ID, and at least one
space should follow. More spaces are allowed for padding (but no tabs -
see rule about Tabs, which btw also contains a taglist example). For
example:

  SetAttrs(obj,
           MUIA_Window_LeftEdge,   left,
           MUIA_Window_TopEdge,    top,
           MUIA_Window_Width,      _width(msg->other),
           TAG_DONE);

^^^^^^^^^^^ - 11 spaces - NO tabs

############################################################################

16. ANSI-C vs. AmigaOS
----------------------

As a general rule: Use the ANSI-C functions like memcpy, malloc etc.

The exception is when the Amiga versions handle locale specific things
which the ANSI-C functions lack. As of this writing, only ToUpper,
ToLower, Stricmp and Strnicmp are cases where the Amiga versions should
be used. The same applies to variable types as mentioned in section 9 of
this styleguide.

############################################################################

17. Comments
------------

A general rule for commenting is: Comment as much as possible while you
are developing your algorithms!

For comment style, we highly prefer using the C++ single line commenting
variant. Even for multiples lines we prefer to use the C++ style (//)
comment characters as we than can more easily comment out things temporarly
with the "/* ... */" comments. So please use the "//" C++ style comments
wherever possible.

############################################################################

18. Debugging statements
------------------------

As we recently implemented a very flexible runtime debugging functions in
Bourriquet, all developers are advised to make use of the provided debugging macros
as much as possible! Especially for the function entry and exit positions
the special macros 'ENTER()', 'LEAVE()' and 'RETURN()' should be considered
mandatory for every new function introduced in Bourriquet.

Example:

  BOOL Function(char *text)
  {
    BOOL result = FALSE;
    ENTER();

    ... code ...

    RETURN(result);
    return(result);
  }

Another mandatory/important debugging macro is the so-called 'ASSERT()'.
It allows to check for a certain condition and if that condition isn't true
anymore, it will immediately output an error string and exit the application.
It should be used whenever possible and where ultimative conditions should
be checked.

Example:

  BOOL Function(char *text)
  {
    BOOL result = FALSE;
    ENTER();

    ASSERT(text != NULL);

    text[10] = 'h'; // if text == NULL a crash would occur!

    ... code ...

    RETURN(result);
    return(result);
  }

In addition, the macros like 'SHOWVALUE()' and 'SHOWSTRING()' also can come
handy during debugging a certain situation. So please check the "debug.h" file
in the 'src' directory of Bourriquet. It contains all currently supported debugging
macros including the printf-like macros "D()", "W()" and "E()" for outputting
variable texts for debugging, warning or even error conditions.

Please note that in case Bourriquet will be compiled without the "DEBUG" define, all
code normally emitted by these macros will be omitted. They are really plainly
meant to be useful during debugging.

############################################################################

19. Obsolete string functions
-----------------------------

A simple rules is: In any event, try to avoid to use one of the following
functions as they are known to cause buffer overflows or other known issues.
Use their replacement functions listed here instead:

  OBSOLETE    REPLACEMENT
  --------    -----------
  strcpy      strlcpy
  strncpy     strlcpy
  strcat      strlcat
  strncat     strlcat
  stccpy      strlcpy
  sprintf     snprintf
  vsprintf    vsnprintf
  strtok      strpbrk

So please try to avoid the use of one of the above listed functions as their
use should be really considered obsolete within Bourriquet. And if we try to stick
to their much safer variants listed here, we really hope to be a bit more
on the safe side and highly reduce the probability of unwanted crashes due to
buffer overflows.

############################################################################

20. Avoid the use of MUIA_ShowMe
--------------------------------

It is known, that the use of MUIA_ShowMe may cause trouble with older MUI
versions (<= 3.8) but also may screw up the window layout with newer MUI
versions, under certain conditions. 

Whereas we don't want to generally ban the use of MUIA_ShowMe
in the sources of bourriquet, we highly suggest to try to avoid it. Where ever
easily possible, a MUIA_ShowMe use should be replaced by the corresponding
MUIM_Group_InitChange/ExitChange combo so that an object will be dynamically
removed (hided) and added (shown) by using there Group method calls instead

############################################################################

21. Do NOT use GetAttr() or SetAttrs()
---------------------------------------

A common mistake when programming BOOPSI classes is to either forget to
properly terminate a SetAttrs() call or that a GetAttr() call might end up
in an uninitialized variables. Both of these problems should, however, be
solved by using the more convenient and secure xget() and xset() macros.

These macros will make sure that the GetAttr() and SetAttrs() calls are
properly terminated and that a GetAttr() call will always return 0 in
case an attribute isn't available.

So to be on the safe side, the direct use of GetAttrs() or SetAttrs()
should be considered obsolete and all calls should be replaced by their
equivalent xget/xset calls.

############################################################################

22. Prefer AllocSysObject(), AllocDosObject() like functions
------------------------------------------------------------

As with newer operating system versions certain new functionality regarding
resource tracking has been added (especially AmigaOS4), the use of the
AllocSysObject() and AllocDosObject() functions should be considered the
preferred way to allocate system resources.

That means, e.g. instead of doing things like CreateIORequest() you are
requested to use AllocSysObject() to create an IORequest. The same applies
for all kind of system structures where these structures are normally
allocated via AllocVec(). For such structures you are advised to use the
corresponding AllocXXXXObject() functions if they exist and do the same
job!

############################################################################

23. Use forward declarations - keep the header files (*.h) clean!
-----------------------------------------------------------------

To keep the header files - and especially the way '#include' statements are
performed - clean, the use of forward declarations should considered
mandatory. To clear things up, the following rules should apply:

  - If a variable in a header file (*.h) is to be defined as a pointer and
    its type is a custom type (e.g. struct), then a forward declaration
    of that type should be put at the top of the header file rather than
    including the header where this type is defined.

  - No forward declarations for 'enum' types should be defined. While some
    compilers might support it, it is not part of any standard.

  - put your forward declarations at the top of each header file, right
    after the last #include statement. They should always look like:

    --- cut here ---
    #include <intuition/classusr.h> // Object

    // forward declarations
    struct Server;
    --- cut here ---

    whereas "struct Server" is the forward declaration.

The main reason why we prefer the use of forward declarations rather than
including every necessary header file is, because it keeps the header files
clean and reduces the possibility of circular include statements. Even more
important, it keeps the dependency list (which is generated via 'make
depend') clean and speed up compilation as well.

############################################################################

24. Comment your structure elements
-----------------------------------

When implementing a new structure via "struct XXXX" it always helps if the
individual structure elements are commented at the point where the structure
will be defined. This should also be done if the element name itself might
be self-explanatory. In practice, this means instead of doing something
like:

  struct Server
  {
    char IP[SIZE_DEFAULT];
    char Port[SIZE_DEFAULT];
    char Name[SIZE_DEFAULT];
  };

you SHOULD do something like:

  struct Server
  {
    char IP[SIZE_DEFAULT];  // IP of server
    char Port[SIZE_DEFAULT];   // Port of server 
    char Name[SIZE_DEFAULT];     // Name of server
  };

By adding line comments (//) to each structure element you help developers
to understand what the individual elements stands for. In addition, you can
give additional information about implications or warnings.

############################################################################


# VERSION FRANÇAISE ########################################### 

Bourriquet
 Copyright (C) digitallytechno

 Ce programme est un logiciel libre ; vous pouvez le redistribuer et/ou le modifier selon les termes de la licence publique générale GNU (GNU General Public License) telle qu'elle est publiée
 et/ou le modifier selon les termes de la Licence Publique Générale GNU telle que publiée par la
 la Free Software Foundation; soit la version 2 de la licence, soit (à votre choix) toute version ultérieure.
 (à votre choix) toute version ultérieure.

 Ce programme est distribué dans l'espoir qu'il sera utile,
 mais SANS AUCUNE GARANTIE ; sans même la garantie implicite de
 MERCHANTABILITÉ ou d'ADAPTATION À UN USAGE PARTICULIER.  Voir la
 Licence publique générale GNU pour plus de détails.

 Vous devriez avoir reçu une copie de la licence publique générale GNU
 avec ce programme ; si ce n'est pas le cas, écrivez à la Free Software
 Foundation, Inc, 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

 ***************************************************************************/

Ce document est destiné à servir de guide général de codage pour le développement sur
Bourriquet. Il contient des informations sur la manière de structurer et de présenter le code source
pendant le développement. Tous les développeurs enregistrés de Bourriquet devraient au moins avoir
lu une fois ce guide et s'en tenir aux règles particulières et aux
suggestions sur la manière de structurer certains passages du code. De cette manière, les futurs
développeurs pourront plus facilement se frayer un chemin dans le code source de Bourriquet
et le prendre comme référence pour savoir à quoi les développeurs de Bourriquet attendent que leur code source ressemble.

Alors, s'il vous plaît, chers développeurs de Bourriquet, essayez de respecter les règles énumérées dans ce guide
afin que le code reste lisible même si plus d'un développeur travaille sur une certaine section.

############################################################################

1. Indentation - Utilisation de la tabulation (TAB)
------------------------------------

Dans le passé, nous avons eu trop de problèmes avec l'utilisation excessive de la tabulation dans le code source de Bourriquet.
Par conséquent, toute utilisation des caractères de tabulation (0x09) pour structurer le code source doit être considérée comme INTERDITE. Au lieu de cela, nous convenons
que pour l'indentation du code source, nous utilisons plutôt DEUX (2) espaces (0x20) :

Exemple :

   win = OpenWindowTags(NULL,
                        WA_Title, « Test »,
                        WA_InnerWidth, width,
                        WA_InnerHeight, hauteur,
                        TAG_DONE) ;

^^^^^^^^^^^^^^^^^^^^^^^^ - 24 (12x2) espaces / *NO* tabulations

2. Blocs
---------

Les blocs commencent sur une nouvelle ligne, avec la même indentation que la ligne précédente,
et se terminent de la même manière. Exemple :

  for(i=0 ; i < 10 ; i++)
    {
      ...code...
    }

Les accolades peuvent être omises pour les blocs d'une ligne. Par exemple

  for(i=0 ; i < 10 ; i++) array[i] += 30 ;

Une exception à cette règle est le « do-while ». Ceux-ci doivent être formatés comme
ceci :

  do
    {
      ...code...
    }
    while(a < 10) ;

############################################################################

3. if/then
----------

Les règles normales des blocs s'appliquent. Toutefois, si les deux branches d'une instruction if-else
sont des blocs d'une ligne, elles doivent être formatées comme suit :

  if(a < 20) Foo() ;
  else Bar() ;

Les instructions if-then-else-if doivent être fusionnées comme suit :

  if(a < 20)
    {
      ...code...
    }
  else if(a < 40)
    {
      ...code...
    }

Tous les blocs d'une séquence if-then-else-if fusionnée doivent partager le même style
lorsqu'il s'agit d'avoir leurs accolades ou non, ce dernier cas n'étant alors
possible que lorsque toutes les branches sont des blocs d'une ligne.

############################################################################

4. Tuples
---------

Un tuple est constitué d'une parenthèse immédiatement suivie du premier membre. Si plusieurs
membres sont présents, ils doivent être séparés par », ». Exemple :

  (unsigned long first, unsigned long second, unsigned long last)
                       ^ ^
                       +---------------------+- notez les espaces ici !

############################################################################

5. Les fonctions
------------

Les fonctions sont toujours en majuscules, c'est-à-dire que la première lettre est une majuscule. Si le nom de la fonction est composé de plusieurs mots, ceux-ci doivent également commencer par une majuscule.

Il y a un espace après le type de retour, sauf s'il s'agit d'un pointeur
(marqué par un astérisque), auquel cas l'espace se trouve entre le type et l'astérisque.
l'astérisque. Il n'y a PAS d'espace entre le nom de la fonction et l'ouverture
de la parenthèse.

Il y a exactement une ligne vide entre les fonctions.

Exemples de fonctions :

  int AllocateBuffer(unsigned long size)
  {
    ...code...
  }

ou

  char *AllocateArray(unsigned long size)
  {
    ...code...
  }

############################################################################

6. return
---------

Une fonction ne doit avoir qu'un point de sortie à la fin de cette fonction.
Une exception à cette règle est l'allocation de ressources ou des contrôles de santé similaires placés au début de la fonction (à l'extérieur). Ici, on peut
renvoyer immédiatement un échec. Exemple :

  BOOL IsLegal(char *str)
  {
    if(str == NULL) return(FALSE) ;

    ...code...

    return(res) ;
  }

La règle dit �...*AN* point de sortie à la fin� plutôt que *one*. Cela permet
de faire une fonction avec plusieurs points de sortie tous placés à la fin, comme ceci :

  BOOL IsWhitespace(char *character)
  {
    if(character == ' ' || ...) return(TRUE) ;
    else return(FALSE) ;
  }

Pour mémoire : return est une directive de contrôle de flux similaire à case,
break, continue et goto, et la valeur fournie ne doit donc pas être placée entre
entre parenthèses.

############################################################################

7.  goto
-----------------------

Parce que l'utilisation générale de goto tend à obscurcir le code source et à aboutir à un « code spaghetti », les instructions goto dans Bourriquet doivent être évitées.
Même si cela aboutit à des arrangements plus profonds de if(), nous pensons qu'il est préférable de restructurer votre code source plutôt que d'être paresseux et d'entrer 
une instruction goto accompagnée d'une étiquette.

############################################################################

8. typedef
--------------------------

L'utilisation générale de « typedef » tend à obscurcir le code source et à le rendre illisible pour les développeurs hautement qualifiés, nous pensons que nous devrions éviter
d'utiliser les typedef. Il existe de nombreuses autres façons d'obtenir le même résultat (en particulier avec les enums). Veuillez donc envisager de NE PAS utiliser de déclarations typedef dans votre code et continuez à utiliser les types natifs.

############################################################################

9. enum
-------------------------------------

Au lieu d'utiliser une simple variable de type « int », il convient de préférer les types d'énumération (enum). Ils doivent être. Même pour les variables d'état locales, les enums doivent être préférées que les variables d'état de type « int ». Ainsi, vous ne devez en aucun cas utiliser un entier pour conserver l'état d'une certaine opération.

Ne faites donc pas quelque chose comme

  BOOL IsRed(int color)
  {
    if(color == 0) return(TRUE) ;
  }
    
Une version correcte de la même fonction serait :

  BOOL IsRed(enum Color color)
  {
    if(color == CL_Red) return(TRUE) ;
  }
  
En outre, vous devez également utiliser les types enum dans les définitions de fonctions et ne pas les définir simplement comme des variables « int ».

Ainsi, ne faites pas quelque chose comme

  BOOL IsRed(int color)
  {
    if(color == CL_Red) return(TRUE) ;
  }

Utilisez plutôt « enum Color color », comme indiqué dans le deuxième exemple ci-dessus. En utilisant des enum dans vos fonctions, vous facilitez la compréhension et le suivi de la signification de la variable par le développeur. En outre, l'utilisation de types enum dans les fonctions, permettra au compilateur de vous avertir au cas où vous auriez oublié un cas d'utilisation (par exemple dans les instructions de commutation).

############################################################################

10. switch/case
---------------

Un cas de switch ressemble à ceci :

  switch(valeur)
    {
      case ONE :
        {
          ...code...
        }
        break ;
      case TWO :
        {
          ...code...
        }
        // continue
      par défaut :
        {
          ...code...
        }
        break ;
    }

Si un cas doit passer au suivant, il convient de mettre un commentaire à la place du break, comme dans l'exemple ci-dessus. Le texte est spécifique à la situation, mais en général, « continue » suffit.

L'exemple suit les règles normales des blocs, mais étend la règle selon laquelle les accolades sont facultatives pour les blocs d'une ligne.

Dans la mesure du possible, un cas de switch doit utiliser des noms symboliques plutôt que des nombres. Cela s'applique également aux machines d'état mises en œuvre par l'intermédiaire de if-then-else-if, même s'il n'y a que deux états.

Si un ensemble de cas existe avec une seule commande, on peut écrire un bloc de cas compact. Les deux points et les points-virgules doivent être alignés
avec un espace après chacun et un espace devant chacun sur la ligne la plus longue.
ligne la plus longue. Il est permis d'avoir un bloc compact de cas à l'intérieur d'une séquence normale switch/case.
séquence normale de case et switch. Exemple :

  switch(valeur)
    {
      case ONE : nombre = « un » ; break ;
      case TWO : nombre = « deux » ; break ;
      case THREE : nombre = « trois » ; break ;
      case FOUR : nombre = « four » ; break ;
      case FIVE : nombre = « cinq » ; break ;
      case FOURTYTWO :
        {
          ...code...
        }
        break ;
    }

##############################################################

11. Variables
-------------

Les variables doivent généralement avoir des noms aussi significatifs que possible. Évitez les abréviations, mais utilisez les abréviations courantes (comme scr, win, obj, etc.).
N'utilisez pas de mots négatifs comme « no » dans une valeur booléenne, mais essayez plutôt d'utiliser quelque chose comme « is », comme dans « isUppercase ». Cela s'applique à la fois aux valeurs booléennes et aux fonctions qui renvoient des valeurs booléennes. 

Les variables locales et les arguments de fonctions sont soit entièrement en minuscules, soit en la notation camélon, où la première lettre est en minuscule et la première lettre des mots suivants est en majuscule, comme le montre l'exemple précédent.
précédent.

Les définitions (constantes et macros) sont entièrement en majuscules. Cependant, si
les définitions sont telles que « #define isUppercase », alors une écriture en
est autorisée.

Les définitions de variables multiples sur une seule ligne doivent être évitées et il est préférable de les étendre sur plusieurs lignes pour une meilleure lecture.

Exemple :

  struct CustomEntry
  {
    char *Nom ;
    char *Adresse ;
  } ;

plutôt que

  struct CustomEntry
  {
    char *Name, *Address ;
  } ;

Les énumérations, les variables globales et les noms et membres de structures sont des cas propres tels que définis dans la section « Fonctions ». En règle générale, les variables statiques doivent également se conformer à cette règle (lorsqu'elles sont réellement censées être globales, mais locales pour ne pas encombrer l'espace de noms global).

Comme nous voulons introduire le multithreading dans Bourriquet tôt ou tard, évitez d'utiliser des variables statiques. Au lieu de cela, utilisez notre « struct Global » qui peut être facilement verrouillée par des sémaphores. 

Les abréviations qui apparaissent normalement en majuscules (par exemple HTML) doivent toujours être en majuscules malgré les règles ci-dessus

Voici quelques exemples :

  #define ABS(x) ((x) < 0 ? -(x) : (x))
  #define PI 3.14159265358979323846

  Object *GlobalConfig = DataspaceObject, ..., End ;

  struct CustomEntry *CreateEntry(char *name, char *address)
  {
    struct CustomEntry *res ;

    if((res = malloc(sizeof(struct CustomEntry)))
      {
        res->Nom = nom ;
        res->Address = address ;
      }
    return(res) ;
  }

############################################################################

12. Utiliser « const » sur les variables dans la mesure du possible
-----------------------------------------------

En utilisant « const » sur les définitions de variables, vous donnez au compilateur un indice supplémentaire sur l'utilisation de ces variables. Pour les compilateurs modernes, cela permet d'optimiser le code et de le rendre plus rapide, mais aussi de fournir un environnement plus sûr. Bien que cela permette au compilateur de vous avertir
(en tant que développeur) dans les cas où vous avez accidentellement accédé à la variable en mode écriture. Il permettra également au compilateur de déplacer la section de données de cette variable dans la zone de lecture seule, de cette variable dans la partie en lecture seule de l'exécutable et donc de la sécuriser contre les modifications externes.

Par conséquent, dans la mesure du possible, vous devez utiliser « const » pour les variables dont vous savez qu'elles ne changeront jamais de contenu.

Ainsi, au lieu de faire quelque chose comme

  BOOL IsRed(enum Color color)
  {
    if(color == CL_Red) return(TRUE) ;
  }
 
vous devriez définir la fonction comme suit

  BOOL IsRed(const enum Color color)
  {
    if(color == CL_Red) return(TRUE) ;
  }
 
Cela permettra au compilateur de vous avertir si vous essayez de modifier localement la variable « color ». 

############################################################################

13. Nommage personnalisé des classes
-----------------------

Comme nous gérons nos propres classes personnalisées MUI dans le répertoire 'src/classes' de Bourriquet, nous essayons de rendre les choses aussi claires que possible. Une chose est le nom des classes personnalisées internes de Bourriquet.

Nous avons pour convention que les classes MUI internes de Bourriquet doivent être placées dans le sous-répertoire 'classes' et, en plus du nom du fichier, la classe interne doit utiliser le même nom de notation que celle que nous utilisons pour les variables.

Exemple : « TextEditor » au lieu de « TextEditor ».

En outre, nous pensons que le nom de la classe/du fichier lui-même doit clairement indiquer de quelle superclasse une classe particulière a été héritée. Par exemple, si vous
une nouvelle classe héritée de MUIC_Group, nous vous conseillons vivement d'utiliser un nom se terminant par « Groupe ».

############################################################################

14. Types
---------

Dans la mesure du possible, nous utilisons des types C standard. Il s'agit de 'char *' pour
STRPTR, 'char' pour TEXT, 'void' pour VOID et 'void *' pour APTR. Cependant, pour certaines tâches spécifiques à l'Amiga, nous nous en tenons aux types C standard,
Par exemple, pour définir les objets MUI, nous utilisons « Object * » plutôt que 'void *',
mais aussi plutôt que APTR.

De plus, bien que Bourriquet soit actuellement en ANSI-C, il faut toujours utiliser les bons
types afin de pouvoir compiler ultérieurement le projet avec un compilateur C++.
Cela signifie que si vous avez par exemple un pointeur IntuiMessage qui doit être
être fourni à 'ReplyMsg()', vous devez utiliser le format ReplyMsg(&imsg->ExecMessage).

Pour les chaînes de caractères intégrées dans des structures, passez un pointeur sur le premier caractère, car il s'agit d'une information supplémentaire pour la personne qui lit la
source. 
Voici un exemple,

  struct Server
  {
    char IP[SIZE_DEFAULT] ;
    char Name[SIZE_NAME] ;
  } ;

  ...

  struct Server *se = SomeServer() ;
  printf(« Letter came from %s\n », &se->IP[0]) ;

par opposition à :

  printf(« Letter came from %s\n », se) ;

############################################################################

15. Listes d'étiquettes
------------

Lorsque l'on fournit des listes de balises, on peut soit mettre toutes les balises sur la même ligne (si seulement quelques balises sont fournies), soit mettre une balise sur chaque ligne, qui est alors indentée avec autant d'espaces jusqu'à la parenthèse d'ouverture de la fonction taglist. Lorsqu'une balise est placée sur sa propre ligne, la première ligne qui contient le « récepteur » ne doit PAS contenir de balise.

La virgule doit venir immédiatement après l'identifiant de la balise, et au moins un espace doit suivre. Des espaces supplémentaires sont autorisés pour le remplissage (mais pas de tabulations - voir la règle sur les tabulations, qui contient également un exemple de liste de balises). Par exemple :

  SetAttrs(obj,
           MUIA_Window_LeftEdge, left,
           MUIA_Window_TopEdge, top,
           MUIA_Window_Width, _width(msg->autre),
           TAG_DONE) ;

^^^^^^^^^^^ - 11 espaces - PAS de tabulations

############################################################################

16. ANSI-C vs. AmigaOS
----------------------

En règle générale : Utilisez les fonctions ANSI-C comme memcpy, malloc etc.

L'exception est quand les versions Amiga gèrent des choses locales spécifiques que les fonctions ANSI-C n'ont pas. A ce jour, seuls ToUpper, ToLower, Stricmp et Strnicmp sont des cas où les versions Amiga doivent être utilisées.
La même chose s'applique aux types de variables comme mentionné dans la section 9 de
ce guide de style.

############################################################################

17. Commentaires
------------

La règle générale en matière de commentaires est la suivante : Commentez autant que possible pendant que vous développer vos algorithmes !

En ce qui concerne le style des commentaires, nous préférons utiliser la variante C++ de commentaire sur une seule ligne de C++. Même pour les lignes multiples, nous préférons utiliser le style C++ (//) car il est plus facile de commenter temporairement des choses avec le « /* ...
avec les commentaires « /* ... */ ». Veuillez donc utiliser les commentaires de style C++ « // » dans la mesure du possible.

############################################################################

18. Débogage des déclarations
------------------------

Comme nous avons récemment mis en place des fonctions de débogage très flexibles dans Bourriquet, il est conseillé à tous les développeurs d'utiliser les macros de débogage fournies autant que possible ! En particulier pour les positions d'entrée et de sortie des fonctions les macros spéciales 'ENTER()', 'LEAVE()' et 'RETURN()' doivent être considérées comme obligatoires pour chaque nouvelle fonction introduite dans Bourriquet.

Exemple :

  BOOL Function(char *text)
  {
    BOOL result = FALSE ;
    ENTER() ;

    ... code ...

    RETURN(résultat) ;
    return(result) ;
  }

Une autre macro de débogage obligatoire/importante est ce que l'on appelle « ASSERT() ». Elle permet de vérifier une certaine condition et si cette condition n'est plus vraie, une chaîne d'erreur est immédiatement émise et l'application est quittée.
Elle doit être utilisée chaque fois que possible et lorsque des conditions ultimes doivent être vérifiées.

Exemple :

  BOOL Function(char *text)
  {
    BOOL result = FALSE ;
    ENTER() ;

    ASSERT(texte != NULL) ;

    text[10] = 'h' ; // si text == NULL un crash se produirait !

    ... code ...

    RETURN(résultat) ;
    return(result) ;
  }

En outre, les macros telles que 'SHOWVALUE()' et 'SHOWSTRING()' peuvent également s'avérer utiles lors du débogage d'une situation donnée. Veuillez donc vérifier le fichier « debug.h » qui se trouve dans le répertoire 'src' de Bourriquet. Il contient toutes les macros de débogage actuellement, y compris les macros de type printf « D() », « W() » et « E() » pour sortir des textes variables pour le débogage, l'avertissement ou même les conditions d'erreur.

Veuillez noter que si Bourriquet est compilé sans la définition « DEBUG », tout le code normalement émis par ces macros sera omis. Elles sont tout simplement destinées à être utiles lors du débogage.

############################################################################

19. Fonctions de chaîne obsolètes
-----------------------------

Une règle simple est la suivante : Dans tous les cas, essayez d'éviter d'utiliser l'une des fonctions suivantes, car elles sont connues pour provoquer des débordements de mémoire tampon ou d'autres problèmes connus.
Utilisez plutôt les fonctions de remplacement listées ici :

  REMPLACEMENT OBSOLÈTE
  -------- -----------
  strcpy strlcpy
  strncpy strlcpy
  strcat strlcat
  strncat strlcat
  stccpy strlcpy
  sprintf snprintf
  vsprintf vsnprintf
  strtok strpbrk

Essayez donc d'éviter d'utiliser l'une de ces fonctions car leur utilisation doit être considérée comme obsolète dans Bourriquet. Et si nous essayons de nous en tenir
à leurs variantes beaucoup plus sûres listées ici, nous espérons vraiment être un peu plus
du côté de la sécurité et réduire fortement la probabilité de plantages indésirables dus à des débordements de tampon.

############################################################################

20. Éviter l'utilisation de MUIA_ShowMe
--------------------------------

Il est connu que l'utilisation de MUIA_ShowMe peut causer des problèmes avec les anciennes versions de MUI (<= 3.8) mais peut également perturber la disposition des fenêtres avec les versions plus récentes de MUI, dans certaines conditions. 

Alors que nous ne voulons pas interdire de manière générale l'utilisation de MUIA_ShowMe dans les sources de bourriquet, nous suggérons fortement d'essayer de l'éviter. Lorsque cela est facilement possible, l'utilisation de MUIA_ShowMe doit être remplacée par l'expression correspondante MUIM_Group_InitChange/ExitChange correspondant, de sorte qu'un objet soit dynamiquement supprimé (caché) et ajouté (affiché) en utilisant ces appels de méthode de groupe à la place.

############################################################################

21. Ne PAS utiliser GetAttr() ou SetAttrs()
---------------------------------------

Une erreur fréquente dans la programmation des classes BOOPSI est d'oublier de terminer correctement une méthode SetAttrs() ou qu'un appel à GetAttr() peut aboutir à une variable non initialisée. Ces deux problèmes devraient cependant être
résolus en utilisant les macros xget() et xset(), plus pratiques et plus sûres.

Ces macros s'assureront que les appels GetAttr() et SetAttrs() sont
correctement terminés et qu'un appel GetAttr() renvoie toujours 0 au cas où un attribut ne serait pas disponible.

Par sécurité, l'utilisation directe de GetAttrs() ou SetAttrs() doit être considérée comme obsolète et tous les appels doivent être remplacés par leurs appels équivalents xget/xset.

############################################################################

22. Préférer les fonctions de type AllocSysObject(), AllocDosObject()
------------------------------------------------------------

Comme avec les nouvelles versions des systèmes d'exploitation, certaines nouvelles fonctionnalités concernant le suivi des ressources ont été ajoutées. L'utilisation des fonctions AllocSysObject() et AllocDosObject() doit être considérée comme la meilleure façon d'allouer des ressources système.

Cela signifie, par exemple, qu'au lieu de faire des choses comme CreateIORequest(), on vous demande d'utiliser AllocSysObject() pour créer un IORequest. Il en va de même
pour toutes les structures du système, qui sont normalement allouées via AllocVec(). Pour ces structures, il est conseillé d'utiliser la fonction AllocXXXXObject() correspondantes si elles existent et font le même travail !

############################################################################

23. Utilisez les déclarations avancées - gardez les fichiers d'en-tête (*.h) propres !
-----------------------------------------------------------------

Pour que les fichiers d'en-tête - et en particulier la façon dont les instructions « #include » sont exécutées - restent propres, l'utilisation des déclarations forward doit être considérée comme un élément essentiel de l'architecture.
Pour clarifier les choses, les règles suivantes devraient s'appliquer :

  - Si une variable dans un fichier d'en-tête (*.h) doit être définie comme un pointeur et que       son type est un type personnalisé (par exemple struct), une déclaration de ce type doit être placée en tête du fichier d'en-tête.

  - Il n'y a pas lieu de définir de déclaration directe pour les types « enum ». Bien que certains compilateurs le supportent, cela ne fait partie d'aucune norme.

  - Placez vos déclarations avancées au début de chaque fichier d'en-tête, juste après la dernière déclaration #include. Elles doivent toujours ressembler à :

    --- couper ici ---
    #include <intuition/classusr.h> // Objet

    // Déclarations avancées
    struct Server ;
    --- coupé ici ---

    alors que « struct Server » est la déclaration avancée.

La raison principale pour laquelle nous préférons utiliser des déclarations avancées plutôt que d'inclure tous les fichiers d'en-tête nécessaires est la suivante :
c'est parce que cela permet de garder les fichiers d'en-tête et réduit la possibilité d'inclure des déclarations circulaires. Plus important encore , cela permet de conserver la liste des dépendances (générée par 'make depend') propre et accélère également la compilation.

############################################################

24. Commentez vos éléments de structure
-----------------------------------

Lors de l'implémentation d'une nouvelle structure via "struct XXXX", il est toujours utile que le
les différents éléments de structure sont commentés à l'endroit où la structure
sera défini. Cela devrait également être fait si le nom de l'élément lui-même pourrait
être explicite. En pratique, cela signifie qu'au lieu de faire quelque chose
comme:

  structure du serveur
  {
    char IP[SIZE_DEFAULT];
    char Port[SIZE_DEFAULT];
    nom du caractère[SIZE_DEFAULT] ;
  } ;

vous DEVRIEZ faire quelque chose comme :

  structure du serveur
  {
    char IP[SIZE_DEFAULT];  // IP du serveur
    char Port[SIZE_DEFAULT];   // Port du serveur 
    nom du caractère[SIZE_DEFAULT] ;     // Nom du serveur
  } ;

En ajoutant des commentaires de ligne (//) à chaque élément de structure, vous aidez les développeurs pour comprendre ce que représentent les différents éléments. De plus, vous pouvez
donner des informations supplémentaires sur les implications ou les avertissements.

################################################# ##########################
