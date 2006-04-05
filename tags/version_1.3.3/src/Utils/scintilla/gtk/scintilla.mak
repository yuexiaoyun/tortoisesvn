# Make file for GTK+/Scintilla on Windows Visual C++
# Borland C++ does not work
# Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
# The License.txt file describes the conditions under which this software may be distributed.
# This makefile is for using Visual C++ with nmake or Borland C++ with make depending on
# the setting of the VENDOR macro. If no VENDOR is defined n the command line then
# the tool used is automatically detected.
# Usage for Microsoft:
#     nmake -f scintilla.mak
# Usage for Borland:
#     make -f scintilla.mak
# For debug versions define DEBUG on the command line, for Borland:
#     make DEBUG=1 -f scintilla.mak
# The main makefile uses mingw32 gcc and may be more current than this file.

.SUFFIXES: .cxx

DIR_O=.
DIR_BIN=..\bin

COMPONENT=$(DIR_BIN)\Scintilla.dll
LEXCOMPONENT=$(DIR_BIN)\SciLexer.dll
STATIC_LIB=$(DIR_BIN)\Scintilla-static.lib

!IFNDEF VENDOR
!IFDEF _NMAKE_VER
#Microsoft nmake so make default VENDOR MICROSOFT
VENDOR=MICROSOFT
!ELSE
VENDOR=BORLAND
!ENDIF
!ENDIF

!IF "$(VENDOR)"=="MICROSOFT"

CC=cl
RC=rc
LD=link

!IFDEF USE_GTK2
MARSHALLER=$(DIR_O)\scintilla-marshal.obj
GTK_TOP= ../../gtk2
TOP = $(GTK_TOP)
!INCLUDE $(TOP)/glib/build/win32/make.msc

GTK_INCLUDES= -I$(GTK_TOP)/gtk+ \
              -I$(GTK_TOP)/gtk+/gdk \
              -I$(GTK_TOP)/glib/glib \
              -I$(GTK_TOP)/glib/gmodule \
              -I$(GTK_TOP)/glib \
              -I$(GTK_TOP)\libiconv\include \
              -I$(GTK_TOP)\pango \
              -I$(GTK_TOP)\atk
ALL_GTK_LIBS=$(GTK2_LIBS) $(GLIB_LIBS) $(LIBICONV_LIBS)
!ELSE
GTK_TOP= ../../win32gtk
GTK_INCLUDES= -I $(GTK_TOP)/gtk+ -I $(GTK_TOP)/gtk+/gdk -I $(GTK_TOP)/glib -I $(GTK_TOP)/glib/gmodule
ALL_GTK_LIBS=$(GTK_TOP)/gtk+/gtk/gtk-1.3.lib \
       $(GTK_TOP)/gtk+/gdk/gdk-1.3.lib \
       $(GTK_TOP)/glib/gmodule/gmodule-1.3.lib \
       $(GTK_TOP)/glib/glib-1.3.lib
!ENDIF

INCLUDEDIRS=-I ../include -I ../src $(GTK_INCLUDES)
CXXFLAGS=/TP /W4 -DGTK
CFLAGS=/W4 -DGTK
# For something scary:/Wp64
CXXDEBUG=/Zi /Od /MDd -DDEBUG
CXXNDEBUG=/Ox /MD -DNDEBUG
NAMEFLAG=-Fo
LDFLAGS=/opt:nowin98
LDDEBUG=/DEBUG
#LIBS=KERNEL32.lib USER32.lib GDI32.lib IMM32.lib OLE32.LIB
LIBS=$(ALL_GTK_LIBS)

!IFDEF QUIET
CC=@$(CC)
CXXDEBUG=$(CXXDEBUG) /nologo
CXXNDEBUG=$(CXXNDEBUG) /nologo
LDFLAGS=$(LDFLAGS) /nologo
!ENDIF

!ELSE
# BORLAND
!error Borland C++ not supported

CC=bcc32
RC=brcc32 -r
LD=ilink32

INCLUDEDIRS=-I../include -I../src
CXXFLAGS =  -v
CXXFLAGS=-P -tWM -w -w-prc -w-inl -RT- -x-
# Above turns off warnings for clarfying parentheses and inlines with for not expanded
CXXDEBUG=-v -DDEBUG
CXXNDEBUG=-O1 -DNDEBUG
NAMEFLAG=-o
LDFLAGS=
LDDEBUG=-v
LIBS=import32 cw32mt

!ENDIF

!IFDEF DEBUG
CXXFLAGS=$(CXXFLAGS) $(CXXDEBUG)
LDFLAGS=$(LDDEBUG) $(LDFLAGS)
!ELSE
CXXFLAGS=$(CXXFLAGS) $(CXXNDEBUG)
!ENDIF

#ALL:	$(STATIC_LIB) $(COMPONENT) $(LEXCOMPONENT) $(DIR_O)\ScintillaGTKS.obj $(DIR_O)\WindowAccessor.obj
ALL:	$(STATIC_LIB) $(DIR_O)\ScintillaGTKS.obj $(DIR_O)\WindowAccessor.obj

clean::
	-del /q $(DIR_O)\*.obj $(DIR_O)\*.pdb $(COMPONENT) $(LEXCOMPONENT) $(DIR_O)\*.res $(DIR_BIN)\*.map

SOBJS=\
	$(DIR_O)\AutoComplete.obj \
	$(DIR_O)\CallTip.obj \
	$(DIR_O)\CellBuffer.obj \
	$(DIR_O)\ContractionState.obj \
	$(DIR_O)\Document.obj \
	$(DIR_O)\Editor.obj \
	$(DIR_O)\ExternalLexer.obj \
	$(DIR_O)\Indicator.obj \
	$(DIR_O)\KeyMap.obj \
	$(DIR_O)\LineMarker.obj \
	$(DIR_O)\PlatGTK.obj \
	$(MARSHALLER) \
	$(DIR_O)\RESearch.obj \
	$(DIR_O)\PropSet.obj \
	$(DIR_O)\ScintillaBase.obj \
	$(DIR_O)\ScintillaGTK.obj \
	$(DIR_O)\Style.obj \
	$(DIR_O)\UniConversion.obj \
	$(DIR_O)\ViewStyle.obj \
	$(DIR_O)\XPM.obj

#++Autogenerated -- run src/LexGen.py to regenerate
#**LEXOBJS=\\\n\(\t$(DIR_O)\\\*.obj \\\n\)
LEXOBJS=\
	$(DIR_O)\LexAda.obj \
	$(DIR_O)\LexAPDL.obj \
	$(DIR_O)\LexAsm.obj \
	$(DIR_O)\LexAsn1.obj \
	$(DIR_O)\LexAU3.obj \
	$(DIR_O)\LexAVE.obj \
	$(DIR_O)\LexBaan.obj \
	$(DIR_O)\LexBash.obj \
	$(DIR_O)\LexBasic.obj \
	$(DIR_O)\LexBullant.obj \
	$(DIR_O)\LexCaml.obj \
	$(DIR_O)\LexCLW.obj \
	$(DIR_O)\LexConf.obj \
	$(DIR_O)\LexCPP.obj \
	$(DIR_O)\LexCrontab.obj \
	$(DIR_O)\LexCsound.obj \
	$(DIR_O)\LexCSS.obj \
	$(DIR_O)\LexEiffel.obj \
	$(DIR_O)\LexErlang.obj \
	$(DIR_O)\LexEScript.obj \
	$(DIR_O)\LexFlagship.obj \
	$(DIR_O)\LexForth.obj \
	$(DIR_O)\LexFortran.obj \
	$(DIR_O)\LexGui4Cli.obj \
	$(DIR_O)\LexHaskell.obj \
	$(DIR_O)\LexHTML.obj \
	$(DIR_O)\LexKix.obj \
	$(DIR_O)\LexLisp.obj \
	$(DIR_O)\LexLout.obj \
	$(DIR_O)\LexLua.obj \
	$(DIR_O)\LexMatlab.obj \
	$(DIR_O)\LexMetapost.obj \
	$(DIR_O)\LexMMIXAL.obj \
	$(DIR_O)\LexMPT.obj \
	$(DIR_O)\LexMSSQL.obj \
	$(DIR_O)\LexNsis.obj \
	$(DIR_O)\LexOthers.obj \
	$(DIR_O)\LexPascal.obj \
	$(DIR_O)\LexPB.obj \
	$(DIR_O)\LexPerl.obj \
	$(DIR_O)\LexPOV.obj \
	$(DIR_O)\LexPS.obj \
	$(DIR_O)\LexPython.obj \
	$(DIR_O)\LexRebol.obj \
	$(DIR_O)\LexRuby.obj \
	$(DIR_O)\LexScriptol.obj \
	$(DIR_O)\LexSmalltalk.obj \
	$(DIR_O)\LexSpecman.obj \
	$(DIR_O)\LexSQL.obj \
	$(DIR_O)\LexTADS3.obj \
	$(DIR_O)\LexTeX.obj \
	$(DIR_O)\LexVB.obj \
	$(DIR_O)\LexVerilog.obj \
	$(DIR_O)\LexVHDL.obj \
	$(DIR_O)\LexYAML.obj \

#--Autogenerated -- end of automatically generated section

LOBJS=\
	$(DIR_O)\AutoComplete.obj \
	$(DIR_O)\CallTip.obj \
	$(DIR_O)\CellBuffer.obj \
	$(DIR_O)\ContractionState.obj \
	$(DIR_O)\Document.obj \
	$(DIR_O)\DocumentAccessor.obj \
	$(DIR_O)\Editor.obj \
	$(DIR_O)\Indicator.obj \
	$(DIR_O)\KeyMap.obj \
	$(DIR_O)\KeyWords.obj \
	$(DIR_O)\LineMarker.obj \
	$(DIR_O)\PlatGTK.obj \
	$(MARSHALLER) \
	$(DIR_O)\RESearch.obj \
	$(DIR_O)\PropSet.obj \
	$(DIR_O)\ScintillaBaseL.obj \
	$(DIR_O)\ScintillaGTKL.obj \
	$(DIR_O)\Style.obj \
	$(DIR_O)\StyleContext.obj \
	$(DIR_O)\UniConversion.obj \
	$(DIR_O)\ViewStyle.obj \
	$(DIR_O)\XPM.obj \
	$(DIR_O)\ExternalLexer.obj \
	$(LEXOBJS)

!IF "$(VENDOR)"=="MICROSOFT"

$(STATIC_LIB): $(LOBJS) #$(DIR_O)\ScintRes.res
	lib.exe /OUT:$@ $(LOBJS) $(LIBS)

$(COMPONENT): $(SOBJS) #$(DIR_O)\ScintRes.res
	$(LD) $(LDFLAGS) /DLL /OUT:$@ $(SOBJS) $(LIBS)

$(DIR_O)\ScintRes.res : ScintRes.rc
	$(RC) /fo$@ $(*B).rc

$(LEXCOMPONENT): $(LOBJS) #$(DIR_O)\ScintRes.res
	$(LD) $(LDFLAGS) /DLL /OUT:$@ $(LOBJS) $(LIBS)

!ELSE

$(STATIC_LIB): $(LOBJS) #$(DIR_O)\ScintRes.res
	$(LD) /OUT:$@ $(LOBJS) $(LIBS)

$(COMPONENT): $(SOBJS) ScintRes.res
	$(LD) $(LDFLAGS) -Tpd -c c0d32 $(SOBJS), $@, , $(LIBS), , ScintRes.res

$(DIR_O)\ScintRes.res: ScintRes.rc
	$(RC) $*.rc

$(LEXCOMPONENT): $(LOBJS)
	$(LD) $(LDFLAGS) -Tpd -c c0d32 $(LOBJS), $@, , $(LIBS), , ScintRes.res

!ENDIF

# Define how to build all the objects and what they depend on

# Most of the source is in ..\src with a couple in this directory
{..\src}.cxx{$(DIR_O)}.obj:
	$(CC) $(INCLUDEDIRS) $(CXXFLAGS) -c $(NAMEFLAG)$@ $<
{.}.cxx{$(DIR_O)}.obj:
	$(CC) $(INCLUDEDIRS) $(CXXFLAGS) -c $(NAMEFLAG)$@ $<
{.}.c{$(DIR_O)}.obj:
	$(CC) $(INCLUDEDIRS) $(CFLAGS) -c $(NAMEFLAG)$@ $<

# Some source files are compiled into more than one object because of different conditional compilation
$(DIR_O)\ScintillaBaseL.obj: ..\src\ScintillaBase.cxx
	$(CC) $(INCLUDEDIRS) $(CXXFLAGS) -DSCI_LEXER -c $(NAMEFLAG)$@ ..\src\ScintillaBase.cxx

$(DIR_O)\ScintillaGTKL.obj: ScintillaGTK.cxx
	$(CC) $(INCLUDEDIRS) $(CXXFLAGS) -DSCI_LEXER -c $(NAMEFLAG)$@ ScintillaGTK.cxx

$(DIR_O)\ScintillaGTKS.obj: ScintillaGTK.cxx
	$(CC) $(INCLUDEDIRS) $(CXXFLAGS) -DSTATIC_BUILD -c $(NAMEFLAG)$@ ScintillaGTK.cxx

# Dependencies

# All lexers depend on this set of headers
LEX_HEADERS=..\include\Platform.h ..\include\PropSet.h \
 ..\include\SString.h ..\include\Accessor.h ..\include\KeyWords.h \
 ..\include\Scintilla.h ..\include\SciLexer.h ..\src\StyleContext.h

$(DIR_O)\AutoComplete.obj: ..\src\AutoComplete.cxx ..\include\Platform.h ..\src\AutoComplete.h

$(DIR_O)\CallTip.obj: ..\src\CallTip.cxx ..\include\Platform.h ..\src\CallTip.h

$(DIR_O)\CellBuffer.obj: ..\src\CellBuffer.cxx ..\include\Platform.h ..\include\Scintilla.h ..\src\CellBuffer.h

$(DIR_O)\ContractionState.obj: ..\src\ContractionState.cxx ..\include\Platform.h ..\src\ContractionState.h

$(DIR_O)\Document.obj: ..\src\Document.cxx ..\include\Platform.h ..\include\Scintilla.h ..\src\RESearch.h \
 ..\src\CellBuffer.h ..\src\Document.h

$(DIR_O)\DocumentAccessor.obj: ..\src\DocumentAccessor.cxx ..\include\Platform.h ..\include\PropSet.h ..\include\Accessor.h ..\src\DocumentAccessor.h ..\include\Scintilla.h

$(DIR_O)\Editor.obj: ..\src\Editor.cxx ..\include\Platform.h ..\include\Scintilla.h ..\src\ContractionState.h \
 ..\src\CellBuffer.h ..\src\KeyMap.h ..\src\Indicator.h ..\src\LineMarker.h ..\src\Style.h ..\src\ViewStyle.h \
 ..\src\Document.h ..\src\Editor.h

$(DIR_O)\ExternalLexer.obj: ..\src\ExternalLexer.cxx ..\include\Platform.h ..\include\Scintilla.h ..\include\SciLexer.h \
 ..\include\PropSet.h ..\include\Accessor.h ..\src\DocumentAccessor.h ..\include\Keywords.h ..\src\ExternalLexer.h

$(DIR_O)\Indicator.obj: ..\src\Indicator.cxx ..\include\Platform.h ..\include\Scintilla.h ..\src\Indicator.h

$(DIR_O)\KeyMap.obj: ..\src\KeyMap.cxx ..\include\Platform.h ..\include\Scintilla.h ..\src\KeyMap.h

$(DIR_O)\KeyWords.obj: ..\src\KeyWords.cxx ..\include\Platform.h ..\include\PropSet.h ..\include\Accessor.h ..\include\KeyWords.h \
 ..\include\Scintilla.h ..\include\SciLexer.h

#++Autogenerated -- run src/LexGen.py to regenerate
#**\n\($(DIR_O)\\\*.obj: ..\\src\\\*.cxx $(LEX_HEADERS)\n\n\)

$(DIR_O)\LexAda.obj: ..\src\LexAda.cxx $(LEX_HEADERS)

$(DIR_O)\LexAPDL.obj: ..\src\LexAPDL.cxx $(LEX_HEADERS)

$(DIR_O)\LexAsm.obj: ..\src\LexAsm.cxx $(LEX_HEADERS)

$(DIR_O)\LexAsn1.obj: ..\src\LexAsn1.cxx $(LEX_HEADERS)

$(DIR_O)\LexAU3.obj: ..\src\LexAU3.cxx $(LEX_HEADERS)

$(DIR_O)\LexAVE.obj: ..\src\LexAVE.cxx $(LEX_HEADERS)

$(DIR_O)\LexBaan.obj: ..\src\LexBaan.cxx $(LEX_HEADERS)

$(DIR_O)\LexBash.obj: ..\src\LexBash.cxx $(LEX_HEADERS)

$(DIR_O)\LexBasic.obj: ..\src\LexBasic.cxx $(LEX_HEADERS)

$(DIR_O)\LexBullant.obj: ..\src\LexBullant.cxx $(LEX_HEADERS)

$(DIR_O)\LexCaml.obj: ..\src\LexCaml.cxx $(LEX_HEADERS)

$(DIR_O)\LexCLW.obj: ..\src\LexCLW.cxx $(LEX_HEADERS)

$(DIR_O)\LexConf.obj: ..\src\LexConf.cxx $(LEX_HEADERS)

$(DIR_O)\LexCPP.obj: ..\src\LexCPP.cxx $(LEX_HEADERS)

$(DIR_O)\LexCrontab.obj: ..\src\LexCrontab.cxx $(LEX_HEADERS)

$(DIR_O)\LexCsound.obj: ..\src\LexCsound.cxx $(LEX_HEADERS)

$(DIR_O)\LexCSS.obj: ..\src\LexCSS.cxx $(LEX_HEADERS)

$(DIR_O)\LexEiffel.obj: ..\src\LexEiffel.cxx $(LEX_HEADERS)

$(DIR_O)\LexErlang.obj: ..\src\LexErlang.cxx $(LEX_HEADERS)

$(DIR_O)\LexEScript.obj: ..\src\LexEScript.cxx $(LEX_HEADERS)

$(DIR_O)\LexFlagship.obj: ..\src\LexFlagship.cxx $(LEX_HEADERS)

$(DIR_O)\LexForth.obj: ..\src\LexForth.cxx $(LEX_HEADERS)

$(DIR_O)\LexFortran.obj: ..\src\LexFortran.cxx $(LEX_HEADERS)

$(DIR_O)\LexGui4Cli.obj: ..\src\LexGui4Cli.cxx $(LEX_HEADERS)

$(DIR_O)\LexHaskell.obj: ..\src\LexHaskell.cxx $(LEX_HEADERS)

$(DIR_O)\LexHTML.obj: ..\src\LexHTML.cxx $(LEX_HEADERS)

$(DIR_O)\LexKix.obj: ..\src\LexKix.cxx $(LEX_HEADERS)

$(DIR_O)\LexLisp.obj: ..\src\LexLisp.cxx $(LEX_HEADERS)

$(DIR_O)\LexLout.obj: ..\src\LexLout.cxx $(LEX_HEADERS)

$(DIR_O)\LexLua.obj: ..\src\LexLua.cxx $(LEX_HEADERS)

$(DIR_O)\LexMatlab.obj: ..\src\LexMatlab.cxx $(LEX_HEADERS)

$(DIR_O)\LexMetapost.obj: ..\src\LexMetapost.cxx $(LEX_HEADERS)

$(DIR_O)\LexMMIXAL.obj: ..\src\LexMMIXAL.cxx $(LEX_HEADERS)

$(DIR_O)\LexMPT.obj: ..\src\LexMPT.cxx $(LEX_HEADERS)

$(DIR_O)\LexMSSQL.obj: ..\src\LexMSSQL.cxx $(LEX_HEADERS)

$(DIR_O)\LexNsis.obj: ..\src\LexNsis.cxx $(LEX_HEADERS)

$(DIR_O)\LexOthers.obj: ..\src\LexOthers.cxx $(LEX_HEADERS)

$(DIR_O)\LexPascal.obj: ..\src\LexPascal.cxx $(LEX_HEADERS)

$(DIR_O)\LexPB.obj: ..\src\LexPB.cxx $(LEX_HEADERS)

$(DIR_O)\LexPerl.obj: ..\src\LexPerl.cxx $(LEX_HEADERS)

$(DIR_O)\LexPOV.obj: ..\src\LexPOV.cxx $(LEX_HEADERS)

$(DIR_O)\LexPS.obj: ..\src\LexPS.cxx $(LEX_HEADERS)

$(DIR_O)\LexPython.obj: ..\src\LexPython.cxx $(LEX_HEADERS)

$(DIR_O)\LexRebol.obj: ..\src\LexRebol.cxx $(LEX_HEADERS)

$(DIR_O)\LexRuby.obj: ..\src\LexRuby.cxx $(LEX_HEADERS)

$(DIR_O)\LexScriptol.obj: ..\src\LexScriptol.cxx $(LEX_HEADERS)

$(DIR_O)\LexSmalltalk.obj: ..\src\LexSmalltalk.cxx $(LEX_HEADERS)

$(DIR_O)\LexSpecman.obj: ..\src\LexSpecman.cxx $(LEX_HEADERS)

$(DIR_O)\LexSQL.obj: ..\src\LexSQL.cxx $(LEX_HEADERS)

$(DIR_O)\LexTADS3.obj: ..\src\LexTADS3.cxx $(LEX_HEADERS)

$(DIR_O)\LexTeX.obj: ..\src\LexTeX.cxx $(LEX_HEADERS)

$(DIR_O)\LexVB.obj: ..\src\LexVB.cxx $(LEX_HEADERS)

$(DIR_O)\LexVerilog.obj: ..\src\LexVerilog.cxx $(LEX_HEADERS)

$(DIR_O)\LexVHDL.obj: ..\src\LexVHDL.cxx $(LEX_HEADERS)

$(DIR_O)\LexYAML.obj: ..\src\LexYAML.cxx $(LEX_HEADERS)


#--Autogenerated -- end of automatically generated section

$(DIR_O)\LineMarker.obj: ..\src\LineMarker.cxx ..\include\Platform.h ..\include\Scintilla.h ..\src\LineMarker.h

$(DIR_O)\PlatWin.obj: PlatWin.cxx ..\include\Platform.h PlatformRes.h ..\src\UniConversion.h

$(DIR_O)\PropSet.obj: ..\src\PropSet.cxx ..\include\Platform.h ..\include\PropSet.h \
 ..\include\SString.h

$(DIR_O)\RESearch.obj: ..\src\RESearch.cxx ..\src\RESearch.h

$(DIR_O)\ScintillaBase.obj: ..\src\ScintillaBase.cxx ..\include\Platform.h ..\include\Scintilla.h \
 ..\src\ContractionState.h ..\src\CellBuffer.h ..\src\CallTip.h ..\src\KeyMap.h ..\src\Indicator.h \
 ..\src\LineMarker.h ..\src\Style.h ..\src\ViewStyle.h ..\src\AutoComplete.h ..\src\Document.h ..\src\Editor.h \
 ..\src\ScintillaBase.h

$(DIR_O)\ScintillaBaseL.obj: ..\src\ScintillaBase.cxx ..\include\Platform.h ..\include\Scintilla.h ..\include\SciLexer.h \
 ..\src\ContractionState.h ..\src\CellBuffer.h ..\src\CallTip.h ..\src\KeyMap.h ..\src\Indicator.h \
 ..\src\LineMarker.h ..\src\Style.h ..\src\AutoComplete.h ..\src\ViewStyle.h ..\src\Document.h ..\src\Editor.h \
 ..\src\ScintillaBase.h ..\include\PropSet.h ..\include\Accessor.h ..\src\DocumentAccessor.h ..\include\KeyWords.h

$(DIR_O)\ScintillaWin.obj: ScintillaWin.cxx ..\include\Platform.h ..\include\Scintilla.h \
 ..\src\ContractionState.h ..\src\CellBuffer.h ..\src\CallTip.h ..\src\KeyMap.h ..\src\Indicator.h \
 ..\src\LineMarker.h ..\src\Style.h ..\src\AutoComplete.h ..\src\ViewStyle.h ..\src\Document.h ..\src\Editor.h \
 ..\src\ScintillaBase.h ..\src\UniConversion.h

$(DIR_O)\ScintillaWinL.obj: ScintillaWin.cxx ..\include\Platform.h ..\include\Scintilla.h ..\include\SciLexer.h \
 ..\src\ContractionState.h ..\src\CellBuffer.h ..\src\CallTip.h ..\src\KeyMap.h ..\src\Indicator.h \
 ..\src\LineMarker.h ..\src\Style.h ..\src\AutoComplete.h ..\src\ViewStyle.h ..\src\Document.h ..\src\Editor.h \
 ..\src\ScintillaBase.h ..\include\PropSet.h ..\include\Accessor.h ..\include\KeyWords.h ..\src\UniConversion.h

$(DIR_O)\ScintillaWinS.obj: ScintillaWin.cxx ..\include\Platform.h ..\include\Scintilla.h \
 ..\src\ContractionState.h ..\src\CellBuffer.h ..\src\CallTip.h ..\src\KeyMap.h ..\src\Indicator.h \
 ..\src\LineMarker.h ..\src\Style.h ..\src\AutoComplete.h ..\src\ViewStyle.h ..\src\Document.h ..\src\Editor.h \
 ..\src\ScintillaBase.h ..\src\UniConversion.h

$(DIR_O)\Style.obj: ..\src\Style.cxx ..\include\Platform.h ..\src\Style.h

$(DIR_O)\StyleContext.obj: ..\src\StyleContext.cxx ..\include\Platform.h ..\include\Accessor.h ..\include\PropSet.h ..\src\StyleContext.h

$(DIR_O)\ViewStyle.obj: ..\src\ViewStyle.cxx ..\include\Platform.h ..\include\Scintilla.h ..\src\Indicator.h \
 ..\src\LineMarker.h ..\src\Style.h ..\src\ViewStyle.h

$(DIR_O)\UniConversion.obj: ..\src\UniConversion.cxx ..\src\UniConversion.h

$(DIR_O)\WindowAccessor.obj: ..\src\WindowAccessor.cxx ..\include\Platform.h ..\include\PropSet.h \
 ..\include\Accessor.h ..\include\WindowAccessor.h ..\include\Scintilla.h
