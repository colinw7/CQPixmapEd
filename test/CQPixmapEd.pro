TEMPLATE = app

TARGET = CQPixmapEd

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += -std=c++14

#CONFIG += debug

# Input
SOURCES += \
CQPixmapEdMain.cpp \

HEADERS += \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

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

unix:LIBS += \
-L$$LIB_DIR \
-L../../CQPixmapEd/lib \
-L../../CQColorSelector/lib \
-L../../CQUtil/lib \
-L../../CColorWheel/lib \
-L../../CUtil/lib \
-L../../CConfig/lib \
-L../../CUndo/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CMath/lib \
-L../../CRGBName/lib \
-L../../CStrUtil/lib \
-L../../CFile/lib \
-L../../CFileUtil/lib \
-L../../COS/lib \
-L../../CRegExp/lib \
-lCQPixmapEd -lCQColorSelector -lCQColorWheel -lCColorWheel \
-lCQUtil -lCConfig -lCUtil -lCImageLib \
-lCRGBName -lCUndo -lCFont -lCFile -lCFileUtil -lCMath -lCOS -lCStrUtil -lCRegExp \
-lpng -ljpeg -ltre 
