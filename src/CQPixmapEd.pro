TEMPLATE = lib

TARGET = CQPixmapEd

QT += widgets

DEPENDPATH += .

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

# Input
SOURCES += \
CQPixmapAddColorDialog.cpp \
CQPixmapBgButton.cpp \
CQPixmapCanvas.cpp \
CQPixmapColorButton.cpp \
CQPixmapEd.cpp \
CQPixmapFgButton.cpp \
CQPixmapRescaleDialog.cpp \
CQPixmapResizeDialog.cpp \
CQPixmapTextDialog.cpp \
CQThumbnailCanvas.cpp \

HEADERS += \
../include/CQPixmapEd.h \
CQPixmapAddColorDialog.h \
CQPixmapBgButton.h \
CQPixmapCanvas.h \
CQPixmapColorButton.h \
CQPixmapColorTip.h \
CQPixmapFgButton.h \
CQPixmapRescaleDialog.h \
CQPixmapResizeDialog.h \
CQPixmapTextDialog.h \
CQPixmapTipSwatch.h \
CQThumbnailCanvas.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../CQColorSelector/include \
../../CColorWheel/qinclude \
../../CColorWheel/include \
../../CQUtil/include \
../../CImageLib/include \
../../CFileUtil/include \
../../CFile/include \
../../CStrUtil/include \
../../CMath/include \
../../CFont/include \
../../COS/include \
../../CUndo/include \
../../CRGBName/include \
../../CUtil/include \
