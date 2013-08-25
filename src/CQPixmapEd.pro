######################################################################
# Automatically generated by qmake (2.01a) Sat May 25 12:00:10 2013
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .

#CONFIG += debug

# Input
SOURCES += \
CQPixmapEdMain.cpp \
CQPixmapEd.cpp \
CQPixmapAddColorDialog.cpp \
CQPixmapBgButton.cpp \
CQPixmapCanvas.cpp \
CQPixmapColorButton.cpp \
CQPixmapFgButton.cpp \
CQPixmapRescaleDialog.cpp \
CQPixmapResizeDialog.cpp \
CQPixmapTextDialog.cpp \
CQPixmapImage.cpp \
CQThumbnailCanvas.cpp \
CQColorWheel.cpp \
CQColorChooser.cpp \
CColorWheel.cpp \
CQToolTip.cpp \

HEADERS += \
CQPixmapAddColorDialog.h \
CQPixmapBgButton.h \
CQPixmapCanvas.h \
CQPixmapColorButton.h \
CQPixmapEd.h \
CQPixmapFgButton.h \
CQPixmapRescaleDialog.h \
CQPixmapResizeDialog.h \
CQPixmapTextDialog.h \
CQPixmapImage.h \
CQThumbnailCanvas.h \
CQColorWheel.h \
CQColorChooser.h \
CColorWheel.h \
CQToolTip.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../include \
../../CMath/include \
../../COS/include \
../../CUndo/include \
../../CUtil/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CUndo/lib \
-lCUndo
