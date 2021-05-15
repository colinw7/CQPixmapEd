#ifndef CQPixmapEd_H
#define CQPixmapEd_H

#include <accessor.h>

#include <CImageLib.h>
#include <CUndo.h>
#include <COptVal.h>

#include <QMainWindow>
#include <QPushButton>

class QAction;
class QToolBar;
#if 0
class CQColorWheel;
#else
class CQColorSelector;
#endif

class QGridLayout;
class QRadioButton;
class QStackedWidget;
class QPainter;
class QLabel;
class QSpinBox;

class CQPixmap;
class CQPixmapCanvas;
class CQThumbnailCanvas;
class CQPixmapFgButton;
class CQPixmapBgButton;
class CQPixmapColorButton;
class CQPixmapFilenameLabel;
class CQPixmapSizeLabel;
class CQPixmapGridSize;
class CQPixmapPosLabel;
class CQPixmapFgControl;
class CQPixmapBgControl;

class CQPixmapUndoImage : public CUndoData {
 public:
  CQPixmapUndoImage(CQPixmap *pixmap, int x, int y, CImagePtr image) :
   pixmap_(pixmap), x_(x), y_(y), image_(image) {
  }

 ~CQPixmapUndoImage() {
  }

  bool exec();

 private:
  CQPixmap *pixmap_;
  int       x_, y_;
  CImagePtr image_;
};

class CQPixmap : public QMainWindow {
  Q_OBJECT

  Q_PROPERTY(int numColorColumns READ numColorColumns WRITE setNumColorColumns)

 public:
  enum ColorMode {
    COLOR_NONE = 0,
    COLOR_MAP  = 1,
    COLOR_RGB  = 2
  };

  enum Function {
    FUNCTION_COPY,
    FUNCTION_MOVE,
    FUNCTION_MARK,
    FUNCTION_POINT,
    FUNCTION_CURVE,
    FUNCTION_LINE,
    FUNCTION_RECTANGLE,
    FUNCTION_FILLED_RECTANGLE,
    FUNCTION_CIRCLE,
    FUNCTION_FILLED_CIRCLE,
    FUNCTION_FLOOD_FILL,
    FUNCTION_LARGEST_RECT,
    FUNCTION_TEXT,
    FUNCTION_SET_HOT_SPOT
  };

  enum XorMode {
    XOR_NONE,
    XOR_LINE,
    XOR_RECTANGLE,
    XOR_FILLED_RECTANGLE,
    XOR_CIRCLE,
    XOR_FILLED_CIRCLE
  };

 public:
  CQPixmap();

  int numColorColumns() const { return numColorColumns_; }
  void setNumColorColumns(int i);

  bool isColorMap() const { return color_mode_ == COLOR_MAP; }
  bool isColorRGB() const { return color_mode_ == COLOR_RGB; }

  Function getFunction() const { return function_; }

  XorMode getXorMode() const { return xor_mode_; }

  const std::string &filename() const { return filename_; }

  CImagePtr getImage() const { return image_; }

  bool getGrid() const { return grid_; }

  int gridSize() const { return gridSize_; }

  int getBgColorNum() const { return bg_color_num_; }
  int getFgColorNum() const { return fg_color_num_; }

  const QColor &getBgColor() const { return bg_color_; }
  const QColor &getFgColor() const { return fg_color_; }

  void setBgColorNum(int color_num);
  void setFgColorNum(int color_num);

  void setBgColor(const QColor &color);
  void setFgColor(const QColor &color);

  ACCESSOR(TransparentColorNum, int, transparent_color_num)

  ACCESSOR(ColorNum, int   , color_num);
  ACCESSOR(Color   , QColor, color);

  bool isBgActive() const { return ! fg_active_; }
  bool isFgActive() const { return   fg_active_; }

  void setBgActive(bool active);
  void setFgActive(bool active);

  const QRect &getXorRect() const { return xor_rect_; }

  const QRect &getMark() const { return mark_.getValue(); }

  void setChanged();

  QSize sizeHint() const override { return QSize(700, 700); }

  CUndo *getUndo() const { return undo_; }

  void init();

  void createMenus();
  void createToolBars();

  QWidget *createCentralWidget();

  void createStatusBar();

  void updateStatusMessage();

  void updatePosition(int x, int y);

  void newImage();

  void loadImage(const std::string &fileName);
  void loadImage(CImagePtr image);

  void insertImage(const std::string &fileName);

  void initImage();
  void setCanvasSize();
  void initColors();

  void addColorButton(int i);
  void addColorSpacer(int i);

  QColor getColor(int x, int y) const;

  CImagePtr getImage(int x, int y, int width, int height);

  void drawImage(int x, int y, CImagePtr image);

  void rotate(int angle);
  void floodFill(int x, int y);
  void largestRect(int x, int y);
  void clear(int left, int bottom, int right, int top);

  void drawText(int x, int y);

  void setHotSpot(int x, int y);

  void setColorMode(ColorMode mode);

  void redraw();

  QPixmap *getPixmap();

  void xorStart(XorMode mode);
  void xorEnd();

  void setXorRectStart(int x, int y);
  void setXorRectEnd(int x, int y);
  void moveXorRect(int dx, int dy);
  void setXorRectFromMark();

  void setRectMark();

  void copyArea(const QRect &from, const QRect &to);
  void moveArea(const QRect &from, const QRect &to);

  void drawRectLine();
  void drawLine(int x1, int y1, int x2, int y2);
  void drawRectRect();
  void fillRectRect();
  void drawRectCircle();
  void fillRectCircle();

  void drawRectangle(int x, int y, int w, int h);
  void fillRectangle(int x, int y, int w, int h);

  void drawPoint(int x, int y);

  void drawCanvasPoint(int x, int y);
  void drawCanvasRect(int x, int y, const QColor &c);
  void drawCanvasRect(int x, int y, Qt::BrushStyle brushStyle);
  void drawXorCanvasPoint(int x, int y);

  void startPainter();
  void endPainter();

  void startUndoGroup();
  void endUndoGroup();

  void addUndoImage();
  void addUndoImage(const QRect &rect);

  void updateUndoButtons();

  bool pixelToWindow(int px, int py, int *wx, int *wy);
  bool windowToPixel(int wx, int wy, int *px, int *py);

 public slots:
  void promptLoadImage();
  void promptInsertImage();
  void saveImage();
  void promptSaveImage();

  void promptResizeImage();
  void resizeImage(int width, int height);

  void promptRescaleImage();
  void rescaleImage(int width, int height, bool keep_aspect);

  void displayControl(bool);
  void setGrid(bool);
  void zoomIn();
  void zoomOut();
  void zoomFull();
  void setGridSize(int);
  void setTextDlg();
  void setDrawText(const QString &);
  void setFontDlg();

  void setColorMap(bool);
  void setColorRGB(bool);
  void addColorDialog();

  void addColor(const QString &);
  void addColor(const QColor &);

  void undo();
  void redo();

  void clear();
  void set();
  void invert();

  void unmark();

  void flipH();
  void flipV();
  void flipHV();
  void scrollLeft();
  void scrollRight();
  void scrollDown();
  void scrollUp();
  void rotateLeft();
  void rotateRight();

  void setCopyMode();
  void setMoveMode();
  void setMarkMode();
  void setPointMode();
  void setCurveMode();
  void setLineMode();
  void setRectangleMode();
  void setFilledRectangleMode();
  void setCircleMode();
  void setFilledCircleMode();
  void setFloodFillMode();
  void setLargestRectMode();
  void setTextMode();
  void setHotSpotMode();

  void clearHotSpot();

#if 0
  void wheelBgColorChanged();
  void wheelFgColorChanged();
#else
  void wheelColorChanged(const QColor &c);
#endif

  void statusMessageChanged(const QString &msg);

 private:
  void updateFunction(Function function);

 private:
  typedef std::vector<CQPixmapColorButton *> ColorButtonList;

  // File Menu
  QAction *loadItem_;
  QAction *insertItem_;
  QAction *saveItem_;
  QAction *saveAsItem_;
  QAction *resizeItem_;
  QAction *rescaleItem_;
  QAction *quitItem_;

  // Edit Menu
  QAction *undoItem_;
  QAction *redoItem_;
  QAction *clearItem_;
  QAction *setItem_;
  QAction *invertItem_;
  QAction *setTextItem_;
  QAction *setFontItem_;

  // Function Menu
  QAction *copyItem_;
  QAction *moveItem_;
  QAction *markItem_;
  QAction *unmarkItem_;
  QAction *pointItem_;
  QAction *curveItem_;
  QAction *lineItem_;
  QAction *rectItem_;
  QAction *fillRectItem_;
  QAction *circleItem_;
  QAction *fillCircleItem_;
  QAction *floodFillItem_;
  QAction *largestRectItem_;
  QAction *textItem_;
  QAction *hotSpotItem_;

  // Move Menu
  QAction *leftItem_;
  QAction *rightItem_;
  QAction *upItem_;
  QAction *downItem_;
  QAction *flipXItem_;
  QAction *flipYItem_;
  QAction *foldItem_;
  QAction *rotateRItem_;
  QAction *rotateLItem_;

  // View Menu
  QAction *controlItem_;
  QAction *gridItem_;
  QAction *zoomInItem_;
  QAction *zoomOutItem_;
  QAction *zoomFullItem_;

  // Color Menu
  QAction *colorMapItem_;
  QAction *colorRGBItem_;
  QAction *addColorItem_;

  // Help Menu
  QAction *aboutItem_;

  // Toolbars
  QToolBar *editToolBar_;
  QToolBar *functionToolBar_;
  QToolBar *moveToolBar_;

  QPushButton  *undoButton_;
  QPushButton  *redoButton_;
  QRadioButton *copyFunction_;
  QRadioButton *moveFunction_;
  QRadioButton *markFunction_;
  QRadioButton *pointFunction_;
  QRadioButton *curveFunction_;
  QRadioButton *lineFunction_;
  QRadioButton *rectangleFunction_;
  QRadioButton *filledRectangleFunction_;
  QRadioButton *circleFunction_;
  QRadioButton *filledCircleFunction_;
  QRadioButton *floodFillFunction_;
  QRadioButton *largestRectFunction_;
  QRadioButton *textFunction_;
  QRadioButton *setHotSpotFunction_;

  QWidget *controlArea_;

  CQPixmapPosLabel *positionLabel_   { 0 };
  CQPixmapGridSize *gridSizeControl_ { 0 };

  ColorMode            color_mode_;
  Function             function_;
  XorMode              xor_mode_;
  std::string          filename_;
  CImagePtr            image_;
  QPixmap             *pixmap_;
  QPainter            *pixmap_painter_;
  QPixmap              xor_pixmap_;
  QPainter            *painter_;
  int                  painter_depth_;
  bool                 changed_;
  QFont                drawFont_;
  QString              drawText_;
  bool                 grid_;
  int                  gridSize_;
  int                  bg_color_num_;
  int                  fg_color_num_;
  int                  transparent_color_num_;
  QColor               bg_color_;
  QColor               fg_color_;
  int                  color_num_;
  QColor               color_;
  bool                 fg_active_;
  CQPixmapCanvas      *canvas_;
  CQThumbnailCanvas   *thumbnail_canvas_;
#if 0
  CQColorWheel        *color_wheel_;
#else
  CQColorSelector     *color_selector_;
#endif
  QStackedWidget        *color_stack_;
  QGridLayout           *colormap_widget_layout_;
  CQPixmapFilenameLabel *filenameLabel_ { 0 };
  CQPixmapSizeLabel     *sizeLabel_ { 0 };
  CQPixmapFgControl     *fgControl_ { 0 };
  CQPixmapBgControl     *bgControl_ { 0 };
  ColorButtonList        color_buttons_;
  int                    numColorColumns_;
  QWidget               *color_spacer_;
  QRect                  xor_rect_;
  COptValT<QRect>        mark_;
  int                    x_hot_, y_hot_;
  CUndo                 *undo_;
};

#endif
