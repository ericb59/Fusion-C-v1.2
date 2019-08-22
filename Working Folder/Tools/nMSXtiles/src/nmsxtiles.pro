#--------------------------------------------------------------------------------
#
#                          Project QtCreator for QT5
#
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# TARGET Specifies the name of the target file.
#--------------------------------------------------------------------------------
TARGET = nMSXtiles

#--------------------------------------------------------------------------------
# VERSION Specifies the version number of the application or library.
#--------------------------------------------------------------------------------
DEFINES += VER_NMSXTILES_MAJOR=0
DEFINES += VER_NMSXTILES_MINOR=9
DEFINES += VER_NMSXTILES_PATCH=5
win32:VERSION = 0.9.5.0             # major.minor.patch.build
else:VERSION  = 0.9.5               # major.minor.patch

#--------------------------------------------------------------------------------
# TEMPLATE Specifies the name of the template to use when generating the project.
#   app     Creates a Makefile for building applications (the default).
#   lib     Creates a Makefile for building libraries.
#--------------------------------------------------------------------------------
TEMPLATE = app

#--------------------------------------------------------------------------------
# QT Specifies the Qt modules that are used by your project.
#--------------------------------------------------------------------------------
QT +=	core \      				# Qt5Core.dll
	gui         				# Qt5Gui.dll
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets	# Qt5Widgets.dll

#--------------------------------------------------------------------------------
# CONFIG Specifies project configuration and compiler options.
#   qt      	The target is a Qt application or library.
#   thread  	Thread support is enabled. This is enabled when CONFIG includes qt.
#   x11     	The target is a X11 application or library.	      # libwinpthread-1.dll
#   windows 	The target is a Win32 window application (app only).  # platforms/qwindows.dll
#   console 	The target is a Win32 console application (app only). 
#   shared  	The target is a shared object/DLL. 
#   static  	The target is a static library (lib only). 
#   release | debug
#   warn_on | warn_off
#   stl | stl_off            Standard Template Library
#   rtti | rtti_off          RunTime Type Identification
#   exceptions | exceptions_off
#   c++11 | c++14
#--------------------------------------------------------------------------------
CONFIG += \
	qt \        	# libgcc_s_dw2-1.dll y libstdc++-6.dll
	thread \    	# libwinpthread-1.dll
	stl_off \
	rtti_off \
	exceptions_off \
	warn_on \
	release

#--------------------------------------------------------------------------------
# DEFINES variable as compiler C preprocessor macros (-D option).
#--------------------------------------------------------------------------------
# DEFINES += USAR_POLLO_DE_GOMA

#--------------------------------------------------------------------------------
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#--------------------------------------------------------------------------------
DEFINES += QT_DEPRECATED_WARNINGS

#--------------------------------------------------------------------------------
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#--------------------------------------------------------------------------------
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#--------------------------------------------------------------------------------
# QMAKE_CXXFLAGS Specifies the C++ compiler flags for building a project.
#--------------------------------------------------------------------------------
# QMAKE_CXXFLAGS +=

#--------------------------------------------------------------------------------
# LIBS Specifies a list of libraries to be linked into the project.
#     unix:LIBS += -L/usr/local/lib -lmath      # "-L/home/user/extra libs" -lmath
#    win32:LIBS += C:/mylibs/math.lib           # "C:/mylibs/extra libs/extra.lib"
#--------------------------------------------------------------------------------
# LIBS +=

#--------------------------------------------------------------------------------
# DEPENDPATH Specifies a list of all directories to look in to resolve dependencies.
DEPENDPATH += \
	. \
	../build

#--------------------------------------------------------------------------------
#INCLUDEPATH Specifies the #include directories which should be searched when compiling the project.
#   win32:INCLUDEPATH += "C:/mylibs/extra headers"
#   unix:INCLUDEPATH += "/home/user/extra headers"
INCLUDEPATH += \
	. \
	../build

#--------------------------------------------------------------------------------
# DESTDIR Specifies where to put the target file.
#--------------------------------------------------------------------------------
DESTDIR = ../build

#--------------------------------------------------------------------------------
# OBJECTS_DIR Specifies the directory where all intermediate objects should be placed.
#--------------------------------------------------------------------------------
OBJECTS_DIR = ../obj

#--------------------------------------------------------------------------------
# SOURCES
#--------------------------------------------------------------------------------
SOURCES += \
        main.cpp \
        mainwindow.cpp	\
        cscreenw.cpp \
	labelbtn.cpp \
	spritesw.cpp \
	supportfuncs.cpp \
	tile.cpp \
 	pletter.cpp \
	fdlgloadbiblio.cpp \
	dlgbankrange.cpp \
	dlgsubscreen.cpp \
	changecolors.cpp \
	dlgexportscreen.cpp \
        dlgexchangetiles.cpp \
        changepalette.cpp

#--------------------------------------------------------------------------------
# HEADERS:
#--------------------------------------------------------------------------------
HEADERS += \
        mainwindow.h \
        cscreenw.h \
	labelbtn.h \
	spritesw.h \
	supportfuncs.h \
	tile.h \
	fdlgloadbiblio.h \
	dlgbankrange.h \
	dlgsubscreen.h \
	changecolors.h \
	dlgexportscreen.h \
        dlgexchangetiles.h \
    changepalette.h

#--------------------------------------------------------------------------------
# FORMS:
#--------------------------------------------------------------------------------
FORMS += \
        mainwindow.ui \
        cscreenw.ui \
	fspritesw.ui \
	dlgloadbiblio.ui \
	dlgbankrange.ui \
	dlgsubscreen.ui \
	fchangecolors.ui \
	exportscreen.ui \
        fdlgexchangetiles.ui \
        changepalette.ui

#--------------------------------------------------------------------------------
# RESOURCES Specifies the name of the resource collection files (qrc) for the target.
#--------------------------------------------------------------------------------
RESOURCES += \
	resources.qrc
RC_ICONS = nMSXtiles.ico

#--------------------------------------------------------------------------------
# OTHER FILES:
#--------------------------------------------------------------------------------
OTHER_FILES += \
    todo.txt

#--------------------------------------------------------------------------------
#                                   END
#--------------------------------------------------------------------------------
