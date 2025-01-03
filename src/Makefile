#################################################
# Bourriquet
# digitally
#################################################
OS = mos
#################################################
# d�finir les commandes communes que nous utilisons
# dans ce makefile. Veuillez noter que chacun d'eux 
# peuvent �tre �cras� par la ligne de commande
#################################################
FLEX    = flex
FC      = flexcat
EXPR    = expr
DATE    = date
RM      = rm -f 
RMDIR   = rm -rf 
MKDIR   = mkdir -p 
CHMOD   = protect FLAGS=rwed
SED     = sed
CP      = copy
CC      = ppc-morphos-gcc-9
STRIP   = ppc-morphos-strip
OBJDUMP = ppc-morphos-objdump
TX      = tx
#################################################
# d�finitions des chemins 
#################################################
CDUP  = /
CDTHIS=
#################################################
# macros sp�ciales pour l'ID de version et les 
# constructions. Il s'agit d'un compilateur crois� sur 
# MorphOS alors nous utilisons expr/gdate 
#################################################
DEVFLAGS = -DDEVWARNING 
BOURRIQUETVERID = -D__BOURRIQUET_VERDATE=\"$(shell $(DATE) +%d.%m.%Y)\" \
             -D__BOURRIQUET_VERDAYS="$(shell $(EXPR) `$(DATE) +%s` / 86400 - 2922)" \
             -D__BOURRIQUET_BUILDDATE=\"$(shell $(DATE) +%Y%m%d)\" \
             -D__BOURRIQUET_BUILDID="$(BUILDID)"

#################################################
# CPU et DEBUG peuvent �tre d�finis en dehors.
# par exemple ... "make DEBUG= CPU=-mcpu=603e" 
# produira une version PPC-603e optimis�e sans debug.
# OPTFLAGS sont annul�s par DEBUG normalement !
# les avertissements ignor�s sont :
# none - parce que nous voulons compiler avec -Wall 
# constamment
#################################################
# Nom de la cible
TARGET  = Bourriquet
# R�pertoires communs
PREFIX    = $(CDTHIS)
MUIDIR    = $(PREFIX)mui
REXXDIR   = $(PREFIX)rexx
MIMEDIR   = $(PREFIX)mime
TCPDIR    = $(PREFIX)tcp
LOCALE    = $(PREFIX)locale
EXTRADIR  = $(PREFIX)extrasrc
TOOLS     = $(PREFIX)tools
LIB       = $(PREFIX)lib/$(OS)
OBJDIR    = .obj_$(OS)
DEPDIR    = .dep_$(OS)
VPATH     = $(OBJDIR)
GCCVER    = 4
# Indicateurs communs du compilateur/lieur 
WARN     = -W -Wall -Wwrite-strings -Wpointer-arith -Wsign-compare #-Wunreachable-code
OPTFLAGS = -O3 -fomit-frame-pointer -fno-strict-aliasing
DEBUG    = -DDEBUG -fno-omit-frame-pointer #-O0
DEBUGSYM = -g -gstabs
CFLAGS   = -I. -I./include $(CPU) $(WARN) $(OPTFLAGS) $(DEVFLAGS) $(DEBUG) $(DEBUGSYM) $(BOURRIQUETVERID)
LDFLAGS  = $(CPU) $(DEBUGSYM)
LDLIBS   = -L$(LIB) -lm 
# Indicateurs du compilateur/lieur pour MorphOS
CPU     = -mcpu=powerpc
CFLAGS  += -noixemul -I./include/netinclude -Wno-pointer-sign
LDFLAGS += -noixemul
LDLIBS  += -lcodesets 

EXTRAOBJS = AllocSysObject.o \
              ExamineDir.o \
              GetHead.o \
              GetPred.o \
              GetSucc.o \
              GetTail.o \
              ItemPoolAlloc.o \
              ItemPoolFree.o \
              MoveList.o \
              NewReadArgs.o \
              SetProcWindow.o \
              asprintf.o \
              getdelim.o \
              memdup.o \
              vasprintf.o
#################################################
# Ici que tout commence 
#################################################

#################################################
# Outils/biblioth�ques tiers
#################################################
GENCLASSES = $(TOOLS)/genclasses
#################################################
# Nos nouvelles classes r��crites
#################################################
MUIOBJS = \
		Classes.o \
		SplashWindow.o \
        BourriquetApplication.o \
        ImageArea.o \
		Toolbar.o \
		AboutWindow.o \
		ShutdownWindow.o \
		ServerList.o \
		GenericRequestWindow.o \
		MainWindow.o \
		Log.o \
		InfosServeur.o \
		ResearchList.o \
		DownloadList.o \
		UploadList.o \
		SharedList.o	\
		ConnectionWindow.o \
		ConfigPage.o \
		ThemeList.o \
		ThemeListGroup.o \
		SharedPage.o \
		ResearchPage.o \
		TransfertPage.o \
		ServerPage.o
		
#################################################
# Les sources principales 
#################################################
BOURRIQUETOBJS = \
		Global.o \
		Bourriquet.o \
		Locale.o \
		Debug.o \
		MUIObjects.o \
        Utilities.o \
        Timer.o \
        MethodStack.o \
        Threads.o \
        Busy.o \
        HashTable.o \
        ImageCache.o \
        FileInfo.o \
		Requesters.o \
		Themes.o \
		ServerMet.o \
		DashBoard.o \
		Config.o \
		Error.o \
		ServerList.o \
		hmac_md5.o \
		md5.o
		
TCPOBJS = \
		Connection.o \
		ConnectionProtocole.o \
		Socket.o
		
OBJS = \
	$(BOURRIQUETOBJS) \
	$(addprefix $(MUIDIR)/,$(MUIOBJS)) \
	$(addprefix $(EXTRADIR)/,$(EXTRAOBJS)) \
	$(addprefix $(TCPDIR)/,$(TCPOBJS))
#################################################
# Traductions disponibles dans le catalogue (po files)
#################################################
CATALOGS = \
	$(patsubst %.po,%.catalog,$(wildcard $(LOCALE)/*.po))
#################################################
# Cible principale
#################################################
.PHONY: all
all: $(OBJDIR) $(OBJDIR)/$(MUIDIR) $(OBJDIR)/$(EXTRADIR) $(DEPDIR) $(DEPDIR)/$(MUIDIR)  $(OBJDIR)/$(TCPDIR) $(DEPDIR)/$(EXTRADIR) $(MUIDIR)/Classes.crc $(TARGET)
#################################################
# Faire les r�pertoires objets 
#################################################
$(OBJDIR):
	@echo "  MK $@"
	@$(MKDIR) $(OBJDIR)

$(OBJDIR)/$(MUIDIR): $(OBJDIR)
	@$(MKDIR) $(OBJDIR)/$(MUIDIR)

$(OBJDIR)/$(EXTRADIR): $(OBJDIR)
	@$(MKDIR) $(OBJDIR)/$(EXTRADIR)

$(OBJDIR)/$(TCPDIR): $(OBJDIR)
	@$(MKDIR) $(OBJDIR)/$(TCPDIR)
#################################################
# Faire les r�pertoires de d�pendances 
#################################################
$(DEPDIR):
	@echo "  MK $@"
	@$(MKDIR) $(DEPDIR)

$(DEPDIR)/$(MUIDIR): $(DEPDIR)
	@$(MKDIR) $(DEPDIR)/$(MUIDIR)

$(DEPDIR)/$(EXTRADIR): $(DEPDIR)
	@$(MKDIR) $(DEPDIR)/$(EXTRADIR)

$(DEPDIR)/$(TCPDIR): $(DEPDIR)
	@$(MKDIR) $(DEPDIR)/$(TCPDIR)
#################################################
# Pour compiler les fichiers simples .c
#################################################
$(OBJDIR)/%.o: %.c
	@echo "  CC $<"
ifeq ($(GCCVER), 2)
	@$(CC) -MM -MG -o $(DEPDIR)/$(subst .c,.d,$<) $(CFLAGS) $<
	@$(SED) -i 's,^.*\.o:,$@:,g' $(DEPDIR)/$(subst .c,.d,$<)
else
	@$(CC) -MM -MP -MT '$@' -MF $(DEPDIR)/$(subst .c,.d,$<) $(CFLAGS) $<
endif
	@$(CC) $(CFLAGS) -c $< -o $@
#################################################
# Pour lier la cible
#################################################
$(TARGET): $(addprefix $(OBJDIR)/,$(OBJS))
	@echo "  LD $@"
	@$(CC) $(LDFLAGS) -o $@ $(addprefix $(OBJDIR)/,$(OBJS)) $(LDLIBS) -Wl,--cref,-M,-Map=$@.map
#################################################
# Pour cr�er un fichier .dump
#################################################
.PHONY: dump
dump:
	-$(OBJDUMP) --section-headers --all-headers --reloc --disassemble-all $(TARGET).debug > $(TARGET).dump
#################################################
# Nettoyage de la cible
#################################################
.PHONY: clean
clean:
	@echo "  CLEAN"
	@$(RM) $(TARGET) $(TARGET) $(TARGET).map $(addprefix $(OBJDIR)/,$(OBJS)) gitrev.h
#################################################
# Nettoyage de tout y compris le r�pertoire .obj
#################################################
.PHONY: cleanall
cleanall: clean
	@echo "  CLEANALL"
	@$(RM) Locale.? $(MUIDIR)/Classes.* $(MUIDIR)/*_cl.h $(MUIDIR)/*.crc
	@$(RM) $(LOCALE)/*.catalog
	@$(RMDIR) $(OBJDIR)
	@$(RMDIR) $(DEPDIR)
#################################################
# Nettoyage de tout, y compris nos outils automatiques
#################################################
.PHONY: distclean
distclean: cleanall
	@echo "  DISTCLEAN"
	@$(MAKE) -C $(GENCLASSES) clean
#################################################
# Ajouter des indicateurs � la cible
#################################################
$(OBJDIR)/Global.o: gitrev.h
$(OBJDIR)/Global.o: CFLAGS += $(BOURRIQUETVERID)
#################################################
## Outil automatique de classes 
#################################################
$(MUIDIR)/Classes.h: $(MUIDIR)/Classes.crc
$(MUIDIR)/Classes.c: $(MUIDIR)/Classes.crc

$(MUIDIR)/Classes.crc: $(GENCLASSES)/GenClasses $(MUIDIR)/ClassesExtra.h Locale.h $(filter-out $(MUIDIR)/Classes.c, $(addprefix $(MUIDIR)/, $(subst .o,.c, $(MUIOBJS))))
	@echo "GN GenClasses"
	@$(GENCLASSES)/GenClasses mui -bBourriquet -gpl -storm -iClassesExtra.h,ClassesSetup.h -qdl

$(GENCLASSES)/GenClasses: $(GENCLASSES)/gc.c $(GENCLASSES)/gc.h \
	$(GENCLASSES)/lists.c $(GENCLASSES)/lists.h \
	$(GENCLASSES)/crc32.c $(GENCLASSES)/crc32.h
	@$(MAKE) -C $(GENCLASSES)
#################################################
# Outil g�n�rateur de gitrev
#################################################
gitrev.h: $(TOOLS)/gitrev.sh
	@echo "  GN $@"
	@$(TOOLS)/gitrev.sh header >$@
#################################################
# Outil de g�n�ration de catalogue 
#################################################
$(LOCALE)/%.catalog: $(LOCALE)/%.po
	@echo "  FC $@"
	@$(FC) REVISION $(shell git rev-list --all --count $<) POFILE $< CATALOG $@ CODESET UTF-8

.IGNORE: $(CATALOGS)

catalogs: $(CATALOGS)
#################################################
# Notre c�l�bre Flexcat 
#################################################
Locale.h: Locale.c
Locale.c: $(LOCALE)/Bourriquet.cd Locale_h.sd Locale_c.sd
	@echo "  FC $@"
	@$(FC) $(LOCALE)/Bourriquet.cd Locale.h=Locale_h.sd Locale.c=Locale_c.sd CODESET ISO-8859-1
#################################################
# Pour cr�er des archives de diffusion
#################################################
.PHONY: release
release: distclean
	@cd ..; scripts/mkdist.sh release mos
#################################################
## Pour cr�er une archive instantan�e 
#################################################
.PHONY: snapshot
snapshot: BUILDID = $(shell $(TOOLS)/gitrev.sh)
snapshot: distclean all
	@lha a BOURRIQUET_$(OS)_$(shell $(TOOLS)/gitrev.sh).lha $(TARGET)*
#################################################
# Inclure des d�pendances 
#################################################
-include $(BOURRIQUETOBJS:%.o=$(DEPDIR)/%.d)
-include $(MUIOBJS:%.o=$(DEPDIR)/$(MUIDIR)/%.d)
-include $(EXTRAOBJS:%.o=$(DEPDIR)/$(EXTRADIR)/%.d)
-include $(TCPOBJS:%.o=$(DEPDIR)/$(TCPDIR)/%.d)
