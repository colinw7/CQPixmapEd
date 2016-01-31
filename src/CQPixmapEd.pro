TEMPLATE = lib

TARGET = CQPixmapEd

QT += widgets

DEPENDPATH += .

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

# Input
SOURCES += \
CQPixmapAddColorDialog.cpp \
CQPixmapBgButton.cpp \
CQPixmapCanvas.cpp \
CQPixmapColorButton.cpp \
CQPixmapEd.cpp \
CQPixmapFgButton.cpp \
CQPixmapImage.cpp \
CQPixmapRescaleDialog.cpp \
CQPixmapResizeDialog.cpp \
CQPixmapTextDialog.cpp \
CQThumbnailCanvas.cpp \
CQColorChooser.cpp \
CQColorWheel.cpp \
CQColorWheelCanvas.cpp \
CQToolTip.cpp \
CQImageButton.cpp \
CQIntegerEdit.cpp \
CColorWheel.cpp \

HEADERS += \
../include/CQPixmapEd.h \
../include/CQPixmapImage.h \
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
CQColorChooser.h \
CQColorWheel.h \
CQColorWheelCanvas.h \
CQToolTip.h \
CQImageButton.h \
CQIntegerEdit.h \
CColorWheel.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../CQColorSelector/include \
../../CQUtil/include \
../../CImageLib/include \
../../CFile/include \
../../CStrUtil/include \
../../CMath/include \
../../CFont/include \
../../COS/include \
../../CUndo/include \
../../CUtil/include \
