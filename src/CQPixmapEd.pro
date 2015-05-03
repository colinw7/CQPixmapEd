TEMPLATE = app

TARGET = CQPixmapEd

DEPENDPATH += .

QT += widgets

#CONFIG += debug

# Input
SOURCES += \
CQPixmapAddColorDialog.cpp \
CQPixmapBgButton.cpp \
CQPixmapCanvas.cpp \
CQPixmapColorButton.cpp \
CQPixmapEd.cpp \
CQPixmapEdMain.cpp \
CQPixmapFgButton.cpp \
CQPixmapImage.cpp \
CQPixmapRescaleDialog.cpp \
CQPixmapResizeDialog.cpp \
CQPixmapTextDialog.cpp \
CQThumbnailCanvas.cpp \
CColorWheel.cpp \
CQColorChooser.cpp \
CQColorWheel.cpp \
CQToolTip.cpp \

HEADERS += \
CQPixmapAddColorDialog.h \
CQPixmapBgButton.h \
CQPixmapCanvas.h \
CQPixmapColorButton.h \
CQPixmapColorTip.h \
CQPixmapEd.h \
CQPixmapFgButton.h \
CQPixmapImage.h \
CQPixmapRescaleDialog.h \
CQPixmapResizeDialog.h \
CQPixmapTextDialog.h \
CQPixmapTipSwatch.h \
CQThumbnailCanvas.h \
CColorWheel.h \
CQColorChooser.h \
CQColorWheel.h \
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
../../CQUtil/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CUndo/lib \
-L../../CQUtil/lib \
-lCQUtil -lCUndo
