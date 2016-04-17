#include <CQPixmapEd.h>
#include <CQPixmapCanvas.h>
#include <CQThumbnailCanvas.h>
#include <CQPixmapBgButton.h>
#include <CQPixmapFgButton.h>
#include <CQPixmapColorButton.h>
#include <CQPixmapResizeDialog.h>
#include <CQPixmapRescaleDialog.h>
#include <CQPixmapTextDialog.h>
#include <CQPixmapAddColorDialog.h>
#include <CQPixmapCache.h>

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QScrollArea>
#include <QPainter>
#include <QFileDialog>
#include <QFontDialog>
#include <QStatusBar>
#include <QStackedWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <CQImageButton.h>
#include <CQUtil.h>
#include <CQFont.h>
#include <CQIntegerEdit.h>

#include <CUndo.h>
#include <CFileUtil.h>
#include <CImageXPM.h>
#include <CBresenham.h>
#if 0
#include <CQColorWheel.h>
#else
#include <CQColorSelector.h>
#endif
#include <CQColorChooser.h>
#include <CRGBName.h>

#include <svg/flip_x_svg.h>
#include <svg/flip_y_svg.h>
#include <svg/fold_svg.h>
#include <svg/up_svg.h>
#include <svg/down_svg.h>
#include <svg/left_svg.h>
#include <svg/right_svg.h>
#include <svg/rotate_left_svg.h>
#include <svg/rotate_right_svg.h>

#include <svg/clear_svg.h>
#include <svg/set_svg.h>
#include <svg/invert_svg.h>
#include <svg/copy_svg.h>
#include <svg/move_svg.h>
#include <svg/mark_svg.h>
#include <svg/unmark_svg.h>
#include <svg/point_svg.h>
#include <svg/curve_svg.h>
#include <svg/line_svg.h>
#include <svg/rect_svg.h>
#include <svg/fill_rect_svg.h>
#include <svg/circle_svg.h>
#include <svg/fill_circle_svg.h>
#include <svg/fill_svg.h>
#include <svg/largest_rect_svg.h>
#include <svg/text_svg.h>
#include <svg/hot_spot_svg.h>
#include <svg/undo_svg.h>
#include <svg/redo_svg.h>

//-----

class CQPixmapStatusLabel : public QLabel {
 public:
  CQPixmapStatusLabel(const QString &str);
};

//-----

class CQPixmapGridSize : public QWidget {
 public:
  CQPixmapGridSize(CQPixmap *pixmap);

  void update();

 private:
  CQPixmap *pixmap_;
  QLabel   *label_;
  QSpinBox *spin_;
};

//-----

class CQPixmapFilenameLabel : public QWidget {
 public:
  CQPixmapFilenameLabel(CQPixmap *pixmap);

  void update();

 private:
  CQPixmap *pixmap_;
  QLabel   *label_;
  QLabel   *fileName_;
};

//-----

class CQPixmapSizeLabel : public QWidget {
 public:
  CQPixmapSizeLabel(CQPixmap *pixmap);

  void update();

 private:
  CQPixmap *pixmap_;
  QLabel   *label_;
  QLabel   *size_;
};

//-----

class CQPixmapPosLabel : public QWidget {
 public:
  CQPixmapPosLabel(CQPixmap *pixmap);

  void update(int x, int y);

 private:
  CQPixmap *pixmap_;
  QLabel   *label_;
  QLabel   *pos_;
};

//-----

class CQPixmapFgControl : public QWidget {
 public:
  CQPixmapFgControl(CQPixmap *pixmap);

  void update();

 private:
  CQPixmap         *pixmap_;
  QLabel           *label_;
  CQPixmapFgButton *button_;
};

//-----

class CQPixmapBgControl : public QWidget {
 public:
  CQPixmapBgControl(CQPixmap *pixmap);

  void update();

 private:
  CQPixmap         *pixmap_;
  QLabel           *label_;
  CQPixmapBgButton *button_;
};

//-----

CQPixmap::
CQPixmap() :
 color_mode_           (COLOR_MAP),
 function_             (FUNCTION_POINT),
 xor_mode_             (XOR_NONE),
 filename_             ("scratch.xpm"),
 pixmap_               (0),
 pixmap_painter_       (0),
 painter_              (0),
 painter_depth_        (0),
 changed_              (true),
 grid_                 (true),
 gridSize_             (1),
 bg_color_num_         (0),
 fg_color_num_         (1),
 transparent_color_num_(2),
 bg_color_             (QColor(255,255,255)),
 fg_color_             (QColor(0,0,0)),
 color_num_            (0),
 color_                (QColor(255,255,255)),
 fg_active_            (true),
 numColorColumns_      (32),
 color_spacer_         (0),
 x_hot_                (-1),
 y_hot_                (-1)
{
  setObjectName("CQPixmap");

  undo_ = new CUndo;

  drawFont_ = font();
  drawText_ = "Abc";
}

void
CQPixmap::
setBgColorNum(int color_num)
{
  if (color_num != bg_color_num_) {
    bg_color_num_ = color_num;

    if (bgControl_)
      bgControl_->update();

    thumbnail_canvas_->update();

    updateStatusMessage();
  }
}

void
CQPixmap::
setNumColorColumns(int i)
{
  numColorColumns_ = i;

  initColors();
}

void
CQPixmap::
setFgColorNum(int color_num)
{
  if (color_num != fg_color_num_) {
    fg_color_num_ = color_num;

    if (fgControl_)
      fgControl_->update();

    updateStatusMessage();
  }
}

void
CQPixmap::
setBgColor(const QColor &c)
{
  if (c != bg_color_) {
    bg_color_ = c;

    if (bgControl_)
      bgControl_->update();

    if (! fg_active_) {
#if 0
    color_wheel_->setRGB(CQUtil::colorToRGBA(bg_color_));
#else
    color_selector_->setColor(bg_color_);
#endif
    }

    thumbnail_canvas_->update();

    updateStatusMessage();
  }
}

void
CQPixmap::
setFgColor(const QColor &c)
{
  if (c != fg_color_) {
    fg_color_ = c;

    if (fg_active_) {
#if 0
      color_wheel_->setRGB(CQUtil::colorToRGBA(fg_color_));
#else
      color_selector_->setColor(fg_color_);
#endif
    }

    if (fgControl_)
      fgControl_->update();

    updateStatusMessage();
  }
}

void
CQPixmap::
setBgActive(bool active)
{
  setFgActive(! active);
}

void
CQPixmap::
setFgActive(bool active)
{
  fg_active_ = active;

  if (fgControl_)
    fgControl_->update();

  if (bgControl_)
    bgControl_->update();

#if 0
  if (fg_active_)
    color_wheel_->setRGB(CQUtil::colorToRGBA(getFgColor()));
  else
    color_wheel_->setRGB(CQUtil::colorToRGBA(getBgColor()));
#else
  if (fg_active_)
    color_selector_->setColor(getFgColor());
  else
    color_selector_->setColor(getBgColor());
#endif
}

void
CQPixmap::
setChanged()
{
  changed_ = true;
}

void
CQPixmap::
init()
{
  createCentralWidget();

  createMenus    ();
  createToolBars ();
  createStatusBar();
}

void
CQPixmap::
createMenus()
{
  QMenu *fileMenu = new QMenu("&File", this);

  menuBar()->addMenu(fileMenu);

  //----

  loadItem_ = new QAction("&Load ...", this); fileMenu->addAction(loadItem_);

  loadItem_->setShortcut(QString("Ctrl+L"));
  loadItem_->setStatusTip("Load image");

  connect(loadItem_, SIGNAL(triggered()), this, SLOT(promptLoadImage()));

  //----

  insertItem_ = new QAction("&Insert ...", this); fileMenu->addAction(insertItem_);

  insertItem_->setStatusTip("Insert image");

  connect(insertItem_, SIGNAL(triggered()), this, SLOT(promptInsertImage()));

  //----

  saveItem_ = new QAction("&Save", this); fileMenu->addAction(saveItem_);

  saveItem_->setStatusTip("Save image");

  connect(saveItem_, SIGNAL(triggered()), this, SLOT(saveImage()));

  //----

  saveAsItem_ = new QAction("Save &As ...", this); fileMenu->addAction(saveAsItem_);

  saveAsItem_->setShortcut(QString("Ctrl+S"));
  saveAsItem_->setStatusTip("Save image as new name");

  connect(saveAsItem_, SIGNAL(triggered()), this, SLOT(promptSaveImage()));

  //----

  fileMenu->addSeparator();

  //----

  resizeItem_ = new QAction("Resize ...", this); fileMenu->addAction(resizeItem_);

  resizeItem_->setStatusTip("Resize image");

  connect(resizeItem_, SIGNAL(triggered()), this, SLOT(promptResizeImage()));

  //----

  rescaleItem_ = new QAction("Rescale ...", this); fileMenu->addAction(rescaleItem_);

  rescaleItem_->setStatusTip("Rescale image");

  connect(rescaleItem_, SIGNAL(triggered()), this, SLOT(promptRescaleImage()));

  //----

  fileMenu->addSeparator();

  //----

  quitItem_ = new QAction("&Quit", this); fileMenu->addAction(quitItem_);

  quitItem_->setShortcut(QString("Ctrl+Q"));
  quitItem_->setStatusTip("Quit the application");

  connect(quitItem_, SIGNAL(triggered()), this, SLOT(close()));

  //--------

  QMenu *editMenu = new QMenu("&Edit", this);

  menuBar()->addMenu(editMenu);

  undoItem_ = new QAction("&Undo", this); editMenu->addAction(undoItem_);

  undoItem_->setShortcut(QString("Ctrl+Z"));
  undoItem_->setStatusTip("Undo last change");
  undoItem_->setIcon(CQPixmapCacheInst->getIcon("UNDO"));

  connect(undoItem_, SIGNAL(triggered()), this, SLOT(undo()));

  redoItem_ = new QAction("&Redo", this); editMenu->addAction(redoItem_);

  redoItem_->setShortcut(QString("Ctrl+Y"));
  redoItem_->setStatusTip("Redo last undo");
  redoItem_->setIcon(CQPixmapCacheInst->getIcon("REDO"));

  connect(redoItem_, SIGNAL(triggered()), this, SLOT(redo()));

  clearItem_ = new QAction("&Clear", this); editMenu->addAction(clearItem_);

  clearItem_->setStatusTip("Clear all pixels in image");
  clearItem_->setIcon(CQPixmapCacheInst->getIcon("CLEAR"));

  connect(clearItem_, SIGNAL(triggered()), this, SLOT(clear()));

  setItem_ = new QAction("&Set", this); editMenu->addAction(setItem_);

  setItem_->setStatusTip("Set all pixels in image");
  setItem_->setIcon(CQPixmapCacheInst->getIcon("SET"));

  connect(setItem_, SIGNAL(triggered()), this, SLOT(set()));

  invertItem_ = new QAction("&Invert", this); editMenu->addAction(invertItem_);

  invertItem_->setStatusTip("Invert image");
  invertItem_->setIcon(CQPixmapCacheInst->getIcon("INVERT"));

  connect(invertItem_, SIGNAL(triggered()), this, SLOT(invert()));

  //----

  editMenu->addSeparator();

  //----

  setTextItem_ = new QAction("Set Text ...", this); editMenu->addAction(setTextItem_);

  setTextItem_->setStatusTip("Set text to draw");

  connect(setTextItem_, SIGNAL(triggered()), this, SLOT(setTextDlg()));

  //----

  setFontItem_ = new QAction("Set Font ...", this); editMenu->addAction(setFontItem_);

  setFontItem_->setStatusTip("Set current font");

  connect(setFontItem_, SIGNAL(triggered()), this, SLOT(setFontDlg()));

  //--------

  QMenu *functionMenu = new QMenu("&Function", this);

  menuBar()->addMenu(functionMenu);

  copyItem_ = new QAction("&Copy", this); functionMenu->addAction(copyItem_);
  copyItem_->setCheckable(true);

  copyItem_->setStatusTip("Copy mode");
  copyItem_->setIcon(CQPixmapCacheInst->getIcon("COPY"));

  connect(copyItem_, SIGNAL(toggled(bool)), this, SLOT(setCopyMode()));

  //----

  moveItem_ = new QAction("&Move", this); functionMenu->addAction(moveItem_);
  moveItem_->setCheckable(true);

  moveItem_->setStatusTip("Move mode");
  moveItem_->setIcon(CQPixmapCacheInst->getIcon("MOVE"));

  connect(moveItem_, SIGNAL(toggled(bool)), this, SLOT(setMoveMode()));

  //----

  markItem_ = new QAction("&Mark", this); functionMenu->addAction(markItem_);
  markItem_->setCheckable(true);

  markItem_->setStatusTip("Mark mode");
  markItem_->setIcon(CQPixmapCacheInst->getIcon("MARK"));

  connect(markItem_, SIGNAL(toggled(bool)), this, SLOT(setMarkMode()));

  //----

  unmarkItem_ = new QAction("&Unmark", this); functionMenu->addAction(unmarkItem_);

  unmarkItem_->setStatusTip("Unmark mode");
  unmarkItem_->setIcon(CQPixmapCacheInst->getIcon("UNMARK"));

  connect(unmarkItem_, SIGNAL(triggered()), this, SLOT(unmark()));

  //----

  pointItem_ = new QAction("&Point", this); functionMenu->addAction(pointItem_);
  pointItem_->setCheckable(true); pointItem_->setChecked(true);

  pointItem_->setStatusTip("Point mode");
  pointItem_->setIcon(CQPixmapCacheInst->getIcon("POINT"));

  connect(pointItem_, SIGNAL(toggled(bool)), this, SLOT(setPointMode()));

  //----

  curveItem_ = new QAction("&Curve", this); functionMenu->addAction(curveItem_);
  curveItem_->setCheckable(true);

  curveItem_->setStatusTip("Curve mode");
  curveItem_->setIcon(CQPixmapCacheInst->getIcon("CURVE"));

  connect(curveItem_, SIGNAL(toggled(bool)), this, SLOT(setCurveMode()));

  //----

  lineItem_ = new QAction("&Line", this); functionMenu->addAction(lineItem_);
  lineItem_->setCheckable(true);

  lineItem_->setStatusTip("Line mode");
  lineItem_->setIcon(CQPixmapCacheInst->getIcon("LINE"));

  connect(lineItem_, SIGNAL(toggled(bool)), this, SLOT(setLineMode()));

  //----

  rectItem_ = new QAction("&Rectangle", this); functionMenu->addAction(rectItem_);
  rectItem_->setCheckable(true);

  rectItem_->setStatusTip("Rectangle mode");
  rectItem_->setIcon(CQPixmapCacheInst->getIcon("RECT"));

  connect(rectItem_, SIGNAL(toggled(bool)), this, SLOT(setRectangleMode()));

  //----

  fillRectItem_ = new QAction("&Filled Rectangle", this); functionMenu->addAction(fillRectItem_);
  fillRectItem_->setCheckable(true);

  fillRectItem_->setStatusTip("Filled rectangle mode");
  fillRectItem_->setIcon(CQPixmapCacheInst->getIcon("FILL_RECT"));

  connect(fillRectItem_, SIGNAL(toggled(bool)), this, SLOT(setFilledRectangleMode()));

  //----

  circleItem_ = new QAction("&Circle", this); functionMenu->addAction(circleItem_);
  circleItem_->setCheckable(true);

  circleItem_->setStatusTip("Circle mode");
  circleItem_->setIcon(CQPixmapCacheInst->getIcon("CIRCLE"));

  connect(circleItem_, SIGNAL(toggled(bool)), this, SLOT(setCircleMode()));

  //----

  fillCircleItem_ = new QAction("&Filled Circle", this); functionMenu->addAction(fillCircleItem_);
  fillCircleItem_->setCheckable(true);

  fillCircleItem_->setStatusTip("Filled circle mode");
  fillCircleItem_->setIcon(CQPixmapCacheInst->getIcon("FILL_CIRCLE"));

  connect(fillCircleItem_, SIGNAL(toggled(bool)), this, SLOT(setFilledCircleMode()));

  //----

  floodFillItem_ = new QAction("&Flood Fill", this); functionMenu->addAction(floodFillItem_);
  floodFillItem_->setCheckable(true);

  floodFillItem_->setStatusTip("Flood fill");
  floodFillItem_->setIcon(CQPixmapCacheInst->getIcon("FILL"));

  connect(floodFillItem_, SIGNAL(toggled(bool)), this, SLOT(setFloodFillMode()));

  //----

  largestRectItem_ = new QAction("&Largest Rect", this); functionMenu->addAction(largestRectItem_);
  largestRectItem_->setCheckable(true);

  largestRectItem_->setStatusTip("Largest Rectangle");
  largestRectItem_->setIcon(CQPixmapCacheInst->getIcon("LARGEST_RECT"));

  connect(largestRectItem_, SIGNAL(toggled(bool)), this, SLOT(setLargestRectMode()));

  //----

  textItem_ = new QAction("&Text", this); functionMenu->addAction(textItem_);
  textItem_->setCheckable(true);

  textItem_->setStatusTip("Text mode");
  textItem_->setIcon(CQPixmapCacheInst->getIcon("TEXT"));

  connect(textItem_, SIGNAL(toggled(bool)), this, SLOT(setTextMode()));

  //----

  hotSpotItem_ = new QAction("&Set Hot Spot", this); functionMenu->addAction(hotSpotItem_);
  hotSpotItem_->setCheckable(true);

  hotSpotItem_->setStatusTip("Set Hot Spot mode");
  hotSpotItem_->setIcon(CQPixmapCacheInst->getIcon("HOT_SPOT"));

  connect(hotSpotItem_, SIGNAL(toggled(bool)), this, SLOT(setHotSpotMode()));

  //--------

  QMenu *moveMenu = new QMenu("&Move", this);

  menuBar()->addMenu(moveMenu);

  leftItem_ = new QAction("&Left", this); moveMenu->addAction(leftItem_);

  leftItem_->setStatusTip("Scroll image left");
  leftItem_->setIcon(CQPixmapCacheInst->getIcon("LEFT"));

  connect(leftItem_, SIGNAL(triggered()), this, SLOT(scrollLeft()));

  //----

  rightItem_ = new QAction("&Right", this); moveMenu->addAction(rightItem_);

  rightItem_->setStatusTip("Scroll image right");
  rightItem_->setIcon(CQPixmapCacheInst->getIcon("RIGHT"));

  connect(rightItem_, SIGNAL(triggered()), this, SLOT(scrollRight()));

  //----

  upItem_ = new QAction("&Up", this); moveMenu->addAction(upItem_);

  upItem_->setStatusTip("Scroll image up");
  upItem_->setIcon(CQPixmapCacheInst->getIcon("UP"));

  connect(upItem_, SIGNAL(triggered()), this, SLOT(scrollUp()));

  //----

  downItem_ = new QAction("&Down", this); moveMenu->addAction(downItem_);

  downItem_->setStatusTip("Scroll image down");
  downItem_->setIcon(CQPixmapCacheInst->getIcon("DOWN"));

  connect(downItem_, SIGNAL(triggered()), this, SLOT(scrollDown()));

  //----

  flipXItem_ = new QAction("&Flip X", this); moveMenu->addAction(flipXItem_);

  flipXItem_->setStatusTip("Flip image in X direction");
  flipXItem_->setIcon(CQPixmapCacheInst->getIcon("FLIP_X"));

  connect(flipXItem_, SIGNAL(triggered()), this, SLOT(flipH()));

  //----

  flipYItem_ = new QAction("&Flip Y", this); moveMenu->addAction(flipYItem_);

  flipYItem_->setStatusTip("Flip image in Y direction");
  flipYItem_->setIcon(CQPixmapCacheInst->getIcon("FLIP_Y"));

  connect(flipYItem_, SIGNAL(triggered()), this, SLOT(flipV()));

  //----

  foldItem_ = new QAction("&Fold", this); moveMenu->addAction(foldItem_);

  foldItem_->setStatusTip("Flip image in both directions");
  foldItem_->setIcon(CQPixmapCacheInst->getIcon("FOLD"));

  connect(foldItem_, SIGNAL(triggered()), this, SLOT(flipHV()));

  //----

  rotateRItem_ = new QAction("&Rotate Right", this); moveMenu->addAction(rotateRItem_);

  rotateRItem_->setStatusTip("Rotate image right");
  rotateRItem_->setIcon(CQPixmapCacheInst->getIcon("ROTATE_RIGHT"));

  connect(rotateRItem_, SIGNAL(triggered()), this, SLOT(rotateRight()));

  //----

  rotateLItem_ = new QAction("&Rotate Left", this); moveMenu->addAction(rotateLItem_);

  rotateLItem_->setStatusTip("Rotate image left");
  rotateLItem_->setIcon(CQPixmapCacheInst->getIcon("ROTATE_LEFT"));

  connect(rotateLItem_, SIGNAL(triggered()), this, SLOT(rotateLeft()));

  //--------

  QMenu *viewMenu = new QMenu("&View", this);

  menuBar()->addMenu(viewMenu);

  controlItem_ = new QAction("&Toggle Controls", this); viewMenu->addAction(controlItem_);
  controlItem_->setCheckable(true);

  controlItem_->setStatusTip("Toggle old/new controls");

  connect(controlItem_, SIGNAL(toggled(bool)), this, SLOT(displayControl(bool)));

  //----

  gridItem_ = new QAction("&Grid", this); viewMenu->addAction(gridItem_);
  gridItem_->setCheckable(true); gridItem_->setChecked(true);

  gridItem_->setShortcut(QKeySequence(Qt::Key_G));
  gridItem_->setStatusTip("Turn on/off grid");

  connect(gridItem_, SIGNAL(toggled(bool)), this, SLOT(setGrid(bool)));

  //----

  viewMenu->addSeparator();

  //----

  zoomInItem_ = new QAction("Zoom In", this); viewMenu->addAction(zoomInItem_);

  zoomInItem_->setShortcut(QKeySequence(Qt::Key_Plus));
  zoomInItem_->setStatusTip("Zoom In");

  connect(zoomInItem_, SIGNAL(triggered()), this, SLOT(zoomIn()));

  //----

  zoomOutItem_ = new QAction("Zoom Out", this); viewMenu->addAction(zoomOutItem_);

  zoomOutItem_->setShortcut(QKeySequence(Qt::Key_Minus));
  zoomOutItem_->setStatusTip("Zoom Out");

  connect(zoomOutItem_, SIGNAL(triggered()), this, SLOT(zoomOut()));

  //----

  zoomFullItem_ = new QAction("Zoom Full", this); viewMenu->addAction(zoomFullItem_);

  zoomFullItem_->setStatusTip("Zoom Full");

  connect(zoomFullItem_, SIGNAL(triggered()), this, SLOT(zoomFull()));

  //----

  QMenu *colorMenu = new QMenu("&Color", this);

  menuBar()->addMenu(colorMenu);

  colorMapItem_ = new QAction("Color &Map", this); colorMenu->addAction(colorMapItem_);
  colorMapItem_->setCheckable(true); colorMapItem_->setChecked(true);

  colorMapItem_->setStatusTip("Colormapped Image (Max 256 colors)");

  connect(colorMapItem_, SIGNAL(toggled(bool)), this, SLOT(setColorMap(bool)));

  colorRGBItem_ = new QAction("Color &RGB", this); colorMenu->addAction(colorRGBItem_);
  colorRGBItem_->setCheckable(true); colorRGBItem_->setChecked(false);

  colorRGBItem_->setStatusTip("RGB Image (Any Color)");

  connect(colorRGBItem_, SIGNAL(toggled(bool)), this, SLOT(setColorRGB(bool)));

  addColorItem_ = new QAction("&Add Color ...", this); colorMenu->addAction(addColorItem_);

  addColorItem_->setStatusTip("Add new color");

  connect(addColorItem_, SIGNAL(triggered()), this, SLOT(addColorDialog()));

  //--------

  //QMenu *sepMenu = new QMenu("|", this);

  //menuBar()->addMenu(sepMenu);

  //--------

  QMenu *helpMenu = new QMenu("&Help", this);

  menuBar()->addMenu(helpMenu);

  //----

  aboutItem_ = new QAction("&About", this); helpMenu->addAction(aboutItem_);

  aboutItem_->setStatusTip("Show the application's About box");
}

void
CQPixmap::
createToolBars()
{
  editToolBar_ = new QToolBar("&Edit", this);

  addToolBar(Qt::TopToolBarArea, editToolBar_);

  int is = style()->pixelMetric(QStyle::PM_SmallIconSize);

  editToolBar_->setIconSize(QSize(is, is));

  editToolBar_->addAction(undoItem_);
  editToolBar_->addAction(redoItem_);
  editToolBar_->addAction(clearItem_);
  editToolBar_->addAction(setItem_);
  editToolBar_->addAction(invertItem_);

  //------

  functionToolBar_ = new QToolBar("&Function", this);

  addToolBar(Qt::LeftToolBarArea, functionToolBar_);

  functionToolBar_->setIconSize(QSize(is, is));

  functionToolBar_->addAction(copyItem_);
  functionToolBar_->addAction(moveItem_);
  functionToolBar_->addAction(markItem_);
  functionToolBar_->addAction(unmarkItem_);
  functionToolBar_->addAction(pointItem_);
  functionToolBar_->addAction(curveItem_);
  functionToolBar_->addAction(lineItem_);
  functionToolBar_->addAction(rectItem_);
  functionToolBar_->addAction(fillRectItem_);
  functionToolBar_->addAction(circleItem_);
  functionToolBar_->addAction(fillCircleItem_);
  functionToolBar_->addAction(floodFillItem_);
  functionToolBar_->addAction(largestRectItem_);
  functionToolBar_->addAction(textItem_);
  functionToolBar_->addAction(hotSpotItem_);

  //------

  moveToolBar_ = new QToolBar("&Move", this);

  addToolBar(Qt::TopToolBarArea, moveToolBar_);

  moveToolBar_->setIconSize(QSize(is, is));

  moveToolBar_->addAction(leftItem_);
  moveToolBar_->addAction(rightItem_);
  moveToolBar_->addAction(upItem_);
  moveToolBar_->addAction(downItem_);
  moveToolBar_->addAction(flipXItem_);
  moveToolBar_->addAction(flipYItem_);
  moveToolBar_->addAction(foldItem_);
  moveToolBar_->addAction(rotateRItem_);
  moveToolBar_->addAction(rotateLItem_);

  //------

  updateUndoButtons();
}

QWidget *
CQPixmap::
createCentralWidget()
{
  QWidget *widget = new QWidget;
  widget->setObjectName("centralWidget");

  QHBoxLayout *layout = new QHBoxLayout(widget);
  layout->setMargin(2); layout->setSpacing(2);

  //-------

  controlArea_ = new QWidget;
  controlArea_->setObjectName("controlArea");

  layout->addWidget(controlArea_);

  QVBoxLayout *controlLayout = new QVBoxLayout(controlArea_);
  controlLayout->setMargin(2); controlLayout->setSpacing(2);

  undoButton_ = new QPushButton("Undo");

  connect(undoButton_, SIGNAL(clicked()), this, SLOT(undo()));

  controlLayout->addWidget(undoButton_);

  redoButton_ = new QPushButton("Redo");

  connect(redoButton_, SIGNAL(clicked()), this, SLOT(redo()));

  controlLayout->addWidget(redoButton_);

  QPushButton *clear = new QPushButton("Clear");

  connect(clear, SIGNAL(clicked()), this, SLOT(clear()));

  controlLayout->addWidget(clear);

  QPushButton *set = new QPushButton("Set");

  connect(set, SIGNAL(clicked()), this, SLOT(set()));

  controlLayout->addWidget(set);

  QPushButton *invert = new QPushButton("Invert");

  connect(invert, SIGNAL(clicked()), this, SLOT(invert()));

  controlLayout->addWidget(invert);

  copyFunction_ = new QRadioButton("Copy");

  connect(copyFunction_, SIGNAL(clicked()), this, SLOT(setCopyMode()));

  controlLayout->addWidget(copyFunction_);

  moveFunction_ = new QRadioButton("Move");

  connect(moveFunction_, SIGNAL(clicked()), this, SLOT(setMoveMode()));

  controlLayout->addWidget(moveFunction_);

  markFunction_ = new QRadioButton("Mark");

  connect(markFunction_, SIGNAL(clicked()), this, SLOT(setMarkMode()));

  controlLayout->addWidget(markFunction_);

  QPushButton *unmark = new QPushButton("Unmark");

  connect(unmark, SIGNAL(clicked()), this, SLOT(unmark()));

  controlLayout->addWidget(unmark);

  //------

  QWidget *scroll_buttons = new QWidget;
  scroll_buttons->setObjectName("scroll_buttons");

  controlLayout->addWidget(scroll_buttons);

  QGridLayout *scroll_buttons_layout = new QGridLayout(scroll_buttons);
  scroll_buttons_layout->setMargin(2); scroll_buttons_layout->setSpacing(2);

  CQImageButton *flip_x_button  = new CQImageButton(CQPixmapCacheInst->getIcon("FLIP_X"));
  CQImageButton *flip_y_button  = new CQImageButton(CQPixmapCacheInst->getIcon("FLIP_Y"));
  CQImageButton *fold_button    = new CQImageButton(CQPixmapCacheInst->getIcon("FOLD"));
  CQImageButton *up_button      = new CQImageButton(CQPixmapCacheInst->getIcon("UP"));
  CQImageButton *down_button    = new CQImageButton(CQPixmapCacheInst->getIcon("DOWN"));
  CQImageButton *left_button    = new CQImageButton(CQPixmapCacheInst->getIcon("LEFT"));
  CQImageButton *right_button   = new CQImageButton(CQPixmapCacheInst->getIcon("RIGHT"));
  CQImageButton *rrotate_button = new CQImageButton(CQPixmapCacheInst->getIcon("ROTATE_RIGHT"));
  CQImageButton *lrotate_button = new CQImageButton(CQPixmapCacheInst->getIcon("ROTATE_LEFT"));

  scroll_buttons_layout->addWidget(flip_x_button , 0, 0);
  scroll_buttons_layout->addWidget(flip_y_button , 0, 2);
  scroll_buttons_layout->addWidget(fold_button   , 1, 1);
  scroll_buttons_layout->addWidget(up_button     , 0, 1);
  scroll_buttons_layout->addWidget(down_button   , 2, 1);
  scroll_buttons_layout->addWidget(left_button   , 1, 0);
  scroll_buttons_layout->addWidget(right_button  , 1, 2);
  scroll_buttons_layout->addWidget(rrotate_button, 2, 2);
  scroll_buttons_layout->addWidget(lrotate_button, 2, 0);

  connect(flip_x_button , SIGNAL(clicked()), this, SLOT(flipH()));
  connect(flip_y_button , SIGNAL(clicked()), this, SLOT(flipV()));
  connect(fold_button   , SIGNAL(clicked()), this, SLOT(flipHV()));
  connect(up_button     , SIGNAL(clicked()), this, SLOT(scrollUp()));
  connect(down_button   , SIGNAL(clicked()), this, SLOT(scrollDown()));
  connect(left_button   , SIGNAL(clicked()), this, SLOT(scrollLeft()));
  connect(right_button  , SIGNAL(clicked()), this, SLOT(scrollRight()));
  connect(rrotate_button, SIGNAL(clicked()), this, SLOT(rotateRight()));
  connect(lrotate_button, SIGNAL(clicked()), this, SLOT(rotateLeft()));

  //------

  pointFunction_ = new QRadioButton("Point");

  connect(pointFunction_, SIGNAL(clicked()), this, SLOT(setPointMode()));

  controlLayout->addWidget(pointFunction_);

  curveFunction_ = new QRadioButton("Curve");

  connect(curveFunction_, SIGNAL(clicked()), this, SLOT(setCurveMode()));

  controlLayout->addWidget(curveFunction_);

  lineFunction_ = new QRadioButton("Line");

  connect(lineFunction_, SIGNAL(clicked()), this, SLOT(setLineMode()));

  controlLayout->addWidget(lineFunction_);

  rectangleFunction_ = new QRadioButton("Rectangle");

  connect(rectangleFunction_, SIGNAL(clicked()), this, SLOT(setRectangleMode()));

  controlLayout->addWidget(rectangleFunction_);

  filledRectangleFunction_ = new QRadioButton("Filled Rectangle");

  connect(filledRectangleFunction_, SIGNAL(clicked()), this, SLOT(setFilledRectangleMode()));

  controlLayout->addWidget(filledRectangleFunction_);

  circleFunction_ = new QRadioButton("Circle");

  connect(circleFunction_, SIGNAL(clicked()), this, SLOT(setCircleMode()));

  controlLayout->addWidget(circleFunction_);

  filledCircleFunction_ = new QRadioButton("Filled Circle");

  connect(filledCircleFunction_, SIGNAL(clicked()), this, SLOT(setFilledCircleMode()));

  controlLayout->addWidget(filledCircleFunction_);

  floodFillFunction_ = new QRadioButton("Flood Fill");

  connect(floodFillFunction_, SIGNAL(clicked()), this, SLOT(setFloodFillMode()));

  controlLayout->addWidget(floodFillFunction_);

  largestRectFunction_ = new QRadioButton("largestRectl");

  connect(largestRectFunction_, SIGNAL(clicked()), this, SLOT(setLargestRectMode()));

  controlLayout->addWidget(largestRectFunction_);

  textFunction_ = new QRadioButton("Text");

  connect(textFunction_, SIGNAL(clicked()), this, SLOT(setTextMode()));

  controlLayout->addWidget(textFunction_);

  setHotSpotFunction_ = new QRadioButton("Set Hot Spot");

  connect(setHotSpotFunction_, SIGNAL(clicked()), this, SLOT(setHotSpotMode()));

  controlLayout->addWidget(setHotSpotFunction_);

  QPushButton *clear_hot_spot = new QPushButton("Clear Hot Spot");

  connect(clear_hot_spot, SIGNAL(clicked()), this, SLOT(clearHotSpot()));

  controlLayout->addWidget(clear_hot_spot);

  controlLayout->addStretch();

  //-------

  QWidget *color_canvas = new QWidget;
  color_canvas->setObjectName("color_canvas");

  layout->addWidget(color_canvas);

  QVBoxLayout *color_canvas_layout = new QVBoxLayout(color_canvas);
  color_canvas_layout->setMargin(2); color_canvas_layout->setSpacing(2);

  //-------

  QWidget *colorControl = new QWidget;
  colorControl->setObjectName("colorControl");

  colorControl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  color_canvas_layout->addWidget(colorControl);

  QHBoxLayout *colorControlLayout = new QHBoxLayout(colorControl);
  colorControlLayout->setMargin(2); colorControlLayout->setSpacing(2);

  //---

#if 0
  QFrame *fg_bg_control = new QFrame;
  fg_bg_control->setObjectName("fg_bg_control");

  fg_bg_control->setFrameShape (QFrame::Panel);
  fg_bg_control->setFrameShadow(QFrame::Sunken);

  colorControlLayout->addWidget(fg_bg_control);

  QVBoxLayout *fg_bg_controlLayout = new QVBoxLayout(fg_bg_control);
  fg_bg_controlLayout->setMargin(2); fg_bg_controlLayout->setSpacing(2);

  fgControl_ = new CQPixmapFgControl(this);
  bgControl_ = new CQPixmapBgControl(this);

  fg_bg_controlLayout->addWidget(fgControl_);
  fg_bg_controlLayout->addWidget(bgControl_);

  fg_bg_controlLayout->addStretch();
#endif

  //---

  color_stack_ = new QStackedWidget;
  color_stack_->setObjectName("color_stack");

  color_stack_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  colorControlLayout->addWidget(color_stack_);

  QFrame *colormap_widget = new QFrame;
  colormap_widget->setObjectName("colormap_widget");

  colormap_widget->setFrameShape (QFrame::Panel);
  colormap_widget->setFrameShadow(QFrame::Sunken);

  colormap_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  colormap_widget_layout_ = new QGridLayout(colormap_widget);
  colormap_widget_layout_->setMargin(2); colormap_widget_layout_->setSpacing(2);

  color_stack_->insertWidget(0, colormap_widget);

  //---

  QFrame *colorrgb_widget = new QFrame;
  colorrgb_widget->setObjectName("colorrgb_widget");

  colorrgb_widget->setFrameShape (QFrame::Panel);
  colorrgb_widget->setFrameShadow(QFrame::Sunken);

  QHBoxLayout *colorrgb_widget_layout = new QHBoxLayout(colorrgb_widget);
  colorrgb_widget_layout->setMargin(2); colorrgb_widget_layout->setSpacing(2);

#if 0
  color_wheel_ = new CQColorWheel(0, Qt::Horizontal, 100, false);

  connect(color_wheel_, SIGNAL(colorBgChanged()), this, SLOT(wheelBgColorChanged()));
  connect(color_wheel_, SIGNAL(colorFgChanged()), this, SLOT(wheelFgColorChanged()));

  colorrgb_widget_layout->addWidget(color_wheel_);

  color_stack_->insertWidget(1, colorrgb_widget);
#else
  CQColorSelector::Config selConfig;

  selConfig.colorButton = false;
  selConfig.colorEdit   = false;

  color_selector_ = new CQColorSelector(0, selConfig);

  color_selector_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(color_selector_, SIGNAL(colorChanged(const QColor &)),
          this, SLOT(wheelColorChanged(const QColor &)));

  color_stack_->insertWidget(1, color_selector_);
#endif

  //---

  colorControlLayout->addStretch();

  //---

  QFrame *thumbnail_frame = new QFrame;
  thumbnail_frame->setObjectName("thumbnail_frame");

  thumbnail_frame->setFrameShape (QFrame::Panel);
  thumbnail_frame->setFrameShadow(QFrame::Sunken);

  colorControlLayout->addWidget(thumbnail_frame);

  QHBoxLayout *thumbnail_layout = new QHBoxLayout(thumbnail_frame);
  thumbnail_layout->setMargin(2); thumbnail_layout->setSpacing(2);

  thumbnail_canvas_ = new CQThumbnailCanvas(this);

  thumbnail_layout->addWidget(thumbnail_canvas_);

  //-------

  QFrame *canvas_frame = new QFrame;
  canvas_frame->setObjectName("canvas_frame");

  canvas_frame->setFrameShape (QFrame::Panel);
  canvas_frame->setFrameShadow(QFrame::Sunken);

  color_canvas_layout->addWidget(canvas_frame);

  QVBoxLayout *scrolled_canvas_layout = new QVBoxLayout(canvas_frame);

  scrolled_canvas_layout->setMargin(2); scrolled_canvas_layout->setSpacing(2);

  QScrollArea *scrolled_canvas = new QScrollArea;

  canvas_ = new CQPixmapCanvas(this);

  canvas_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  scrolled_canvas->setWidget(canvas_);

  scrolled_canvas_layout->addWidget(scrolled_canvas);

  //-------

  pointFunction_->setChecked(true);

  newImage();

  //-------

  controlArea_->setVisible(false);

  //-------

  setCentralWidget(widget);

  return widget;
}

void
CQPixmap::
createStatusBar()
{
  QStatusBar *sbar = statusBar();

  filenameLabel_ = new CQPixmapFilenameLabel(this);

  sbar->insertPermanentWidget(0, filenameLabel_);

  //---

  QWidget *spacer = new QWidget;

  sbar->insertPermanentWidget(1, spacer, 1);

  //---

  sizeLabel_ = new CQPixmapSizeLabel(this);

  sbar->insertPermanentWidget(2, sizeLabel_);

  //---

  fgControl_ = new CQPixmapFgControl(this);

  sbar->insertPermanentWidget(3, fgControl_);

  bgControl_ = new CQPixmapBgControl(this);

  sbar->insertPermanentWidget(4, bgControl_);

  //---

  gridSizeControl_ = new CQPixmapGridSize(this);

  sbar->insertPermanentWidget(5, gridSizeControl_);

  //---

  positionLabel_ = new CQPixmapPosLabel(this);

  sbar->insertPermanentWidget(6, positionLabel_);

  //---

  sbar->showMessage(" ");

  //---

  connect(sbar, SIGNAL(messageChanged(const QString &)),
          this, SLOT(statusMessageChanged(const QString &)));
}

void
CQPixmap::
statusMessageChanged(const QString &msg)
{
  if (msg == "")
    updateStatusMessage();
}

void
CQPixmap::
updateStatusMessage()
{
#if 0
  QString msg;

  QString bg_str, fg_str;

  if (isColorMap()) {
    //bg = image_->getColor(getBgColorNum());
    //fg = image_->getColor(getFgColorNum());

    int bg_num = getBgColorNum();

    bg_str = QString("%1").arg(bg_num) + "(";

    if (bg_num < 0)
      bg_str += "#0000000";
    else {
      QColor bg = CQUtil::rgbaToColor(image_->getColor(bg_num));

      bg_str += bg.name();
    }

    bg_str += ")";

    int fg_num = getFgColorNum();

    fg_str = QString("%1").arg(fg_num) + "(";

    if (fg_num < 0)
      fg_str += "#0000000";
    else {
      QColor fg = CQUtil::rgbaToColor(image_->getColor(fg_num));

      fg_str += fg.name();
    }

    fg_str += ")";
  }
  else {
    bg_str = bg_color_.name();
    fg_str = fg_color_.name();
  }

  msg = QString("Filename: %1, Size %2x%3").
                arg(filename_.c_str()).arg(image_->getWidth()).arg(image_->getHeight());

  statusBar()->showMessage(msg);
#endif
}

void
CQPixmap::
updatePosition(int x, int y)
{
  positionLabel_->update(x, y);
}

void
CQPixmap::
promptLoadImage()
{
  QString fileName = QFileDialog::getOpenFileName(this,
    "Load Image", "", "Image Files (*.bmp *.gif *.jpg *.png *.xpm)");

  if (fileName.length())
    loadImage(fileName.toStdString());
}

void
CQPixmap::
promptInsertImage()
{
  QString fileName = QFileDialog::getOpenFileName(this,
    "Insert Image", "", "Image Files (*.bmp *.gif *.jpg *.png *.xpm)");

  if (fileName.length())
    insertImage(fileName.toStdString());
}

void
CQPixmap::
saveImage()
{
  CFileType type = CFileUtil::getImageTypeFromName(filename_);

  if (type == CFILE_TYPE_NONE)
    type = CFILE_TYPE_IMAGE_XPM;

  image_->write(filename_, type);
}

void
CQPixmap::
promptSaveImage()
{
  QString fileName = QFileDialog::getSaveFileName(this,
    "Save Image", "", "Image Files (*.bmp *.gif *.jpg *.png *.xpm)");

  if (! fileName.length())
    return;

  filename_ = fileName.toStdString();

  saveImage();

  if (filenameLabel_)
    filenameLabel_->update();

  updateStatusMessage();
}

void
CQPixmap::
promptResizeImage()
{
  CQPixmapResizeDialog *dialog = new CQPixmapResizeDialog(this);

  dialog->init(image_->getWidth(), image_->getHeight());

  connect(dialog, SIGNAL(resize(int, int)), this, SLOT(resizeImage(int, int)));

  dialog->show();
}

void
CQPixmap::
promptRescaleImage()
{
  CQPixmapRescaleDialog *dialog = new CQPixmapRescaleDialog(this);

  dialog->init(image_->getWidth(), image_->getHeight(), false);

  connect(dialog, SIGNAL(rescale(int, int, bool)), this, SLOT(rescaleImage(int, int, bool)));

  dialog->show();
}

void
CQPixmap::
displayControl(bool display)
{
  controlArea_->setVisible(display);

  editToolBar_    ->setVisible(! display);
  functionToolBar_->setVisible(! display);
  moveToolBar_    ->setVisible(! display);
}

void
CQPixmap::
setGrid(bool grid)
{
  grid_ = grid;

  setChanged();

  redraw();
}

void
CQPixmap::
zoomIn()
{
  setGridSize(gridSize() + 1);
}

void
CQPixmap::
zoomOut()
{
  setGridSize(gridSize() - 1);
}

void
CQPixmap::
zoomFull()
{
  setGridSize(1);
}

void
CQPixmap::
setGridSize(int s)
{
  gridSize_ = std::min(std::max(s, 1), 64);

  if (gridSizeControl_)
    gridSizeControl_->update();

  setCanvasSize();
}

void
CQPixmap::
setTextDlg()
{
  CQPixmapTextDialog *dialog = new CQPixmapTextDialog(this);

  dialog->init(drawText_);

  connect(dialog, SIGNAL(setText(const QString &)), this, SLOT(setDrawText(const QString &)));

  dialog->show();
}

void
CQPixmap::
setDrawText(const QString &str)
{
  drawText_ = str;
}

void
CQPixmap::
setFontDlg()
{
  bool ok;

  QFont font = QFontDialog::getFont(&ok, drawFont_, this);

  if (! ok) return;

  drawFont_ = font;
}

void
CQPixmap::
setColorMap(bool active)
{
  setColorMode(active ? COLOR_MAP : COLOR_RGB);
}

void
CQPixmap::
setColorRGB(bool active)
{
  setColorMode(active ? COLOR_RGB : COLOR_MAP);
}

void
CQPixmap::
setColorMode(ColorMode mode)
{
  if (mode == color_mode_)
    return;

  color_mode_ = mode;

  color_stack_->setCurrentIndex(color_mode_ == COLOR_MAP ? 0 : 1);

  if (isColorMap())
    image_->convertToColorIndex();
  else
    image_->convertToRGB();

  colorMapItem_->setChecked(color_mode_ == COLOR_MAP);
  colorRGBItem_->setChecked(color_mode_ == COLOR_RGB);

  addColorItem_->setEnabled(color_mode_ == COLOR_MAP);

  setChanged();

  initColors();

  redraw();
}

void
CQPixmap::
addColorDialog()
{
  if (! isColorMap())
    return;

  CQPixmapAddColorDialog *dialog = new CQPixmapAddColorDialog(this);

#if 0
  connect(dialog, SIGNAL(addColor(const QString &)), this, SLOT(addColor(const QString &)));
#else
  connect(dialog, SIGNAL(addColor(const QColor &)), this, SLOT(addColor(const QColor &)));
#endif

  dialog->show();
}

void
CQPixmap::
addColor(const QString &name)
{
  CRGBA rgba;

  CRGBName::toRGBA(name.toStdString(), rgba);

  addColor(CQUtil::rgbaToColor(rgba));
}

void
CQPixmap::
addColor(const QColor &c)
{
  if (! isColorMap())
    return;

  image_->addColor(CQUtil::colorToRGBA(c));

  initColors();
}

void
CQPixmap::
undo()
{
  undo_->undo();

  updateUndoButtons();

  setChanged();

  redraw();
}

void
CQPixmap::
redo()
{
  undo_->redo();

  updateUndoButtons();

  setChanged();

  redraw();
}

void
CQPixmap::
clear()
{
  addUndoImage();

  if (isColorMap())
    image_->setColorIndexData(getBgColorNum());
  else
    image_->setRGBAData(CQUtil::colorToRGBA(getBgColor()));

  setChanged();

  redraw();
}

void
CQPixmap::
set()
{
  addUndoImage();

  if (isColorMap())
    image_->setColorIndexData(getFgColorNum());
  else
    image_->setRGBAData(CQUtil::colorToRGBA(getFgColor()));

  setChanged();

  redraw();
}

void
CQPixmap::
invert()
{
  addUndoImage();

  image_->invert();

  initColors();

  setChanged();

  redraw();
}

void
CQPixmap::
unmark()
{
  mark_.setInvalid();

  setChanged();

  redraw();

  image_->resetWindow();
}

void
CQPixmap::
flipH()
{
  addUndoImage();

  image_->flipH();

  setChanged();

  redraw();
}

void
CQPixmap::
flipV()
{
  addUndoImage();

  image_->flipV();

  setChanged();

  redraw();
}

void
CQPixmap::
flipHV()
{
  addUndoImage();

  image_->flipHV();

  setChanged();

  redraw();
}

void
CQPixmap::
scrollLeft()
{
  addUndoImage();

  image_->scrollX(-1);

  setChanged();

  redraw();
}

void
CQPixmap::
scrollRight()
{
  addUndoImage();

  image_->scrollX(1);

  setChanged();

  redraw();
}

void
CQPixmap::
scrollDown()
{
  addUndoImage();

  image_->scrollY(-1);

  setChanged();

  redraw();
}

void
CQPixmap::
scrollUp()
{
  addUndoImage();

  image_->scrollY(1);

  setChanged();

  redraw();
}

void
CQPixmap::
rotateLeft()
{
  addUndoImage();

  rotate(90);
}

void
CQPixmap::
rotateRight()
{
  addUndoImage();

  rotate(-90);
}

void
CQPixmap::
setCopyMode()
{
  updateFunction(FUNCTION_COPY);
}

void
CQPixmap::
setMoveMode()
{
  updateFunction(FUNCTION_MOVE);
}

void
CQPixmap::
setMarkMode()
{
  updateFunction(FUNCTION_MARK);
}

void
CQPixmap::
setPointMode()
{
  updateFunction(FUNCTION_POINT);
}

void
CQPixmap::
setCurveMode()
{
  updateFunction(FUNCTION_CURVE);
}

void
CQPixmap::
setLineMode()
{
  updateFunction(FUNCTION_LINE);
}

void
CQPixmap::
setRectangleMode()
{
  updateFunction(FUNCTION_RECTANGLE);
}

void
CQPixmap::
setFilledRectangleMode()
{
  updateFunction(FUNCTION_FILLED_RECTANGLE);
}

void
CQPixmap::
setCircleMode()
{
  updateFunction(FUNCTION_CIRCLE);
}

void
CQPixmap::
setFilledCircleMode()
{
  updateFunction(FUNCTION_FILLED_CIRCLE);
}

void
CQPixmap::
setFloodFillMode()
{
  updateFunction(FUNCTION_FLOOD_FILL);
}

void
CQPixmap::
setLargestRectMode()
{
  updateFunction(FUNCTION_LARGEST_RECT);
}

void
CQPixmap::
setTextMode()
{
  updateFunction(FUNCTION_TEXT);
}

void
CQPixmap::
setHotSpotMode()
{
  updateFunction(FUNCTION_SET_HOT_SPOT);
}

void
CQPixmap::
updateFunction(Function function)
{
  static bool guard;

  if (guard) return;

  guard = true;

  function_ = function;

  switch (function_) {
    case FUNCTION_COPY            : copyFunction_           ->setChecked(true); break;
    case FUNCTION_MOVE            : moveFunction_           ->setChecked(true); break;
    case FUNCTION_MARK            : markFunction_           ->setChecked(true); break;
    case FUNCTION_POINT           : pointFunction_          ->setChecked(true); break;
    case FUNCTION_CURVE           : curveFunction_          ->setChecked(true); break;
    case FUNCTION_LINE            : lineFunction_           ->setChecked(true); break;
    case FUNCTION_RECTANGLE       : rectangleFunction_      ->setChecked(true); break;
    case FUNCTION_FILLED_RECTANGLE: filledRectangleFunction_->setChecked(true); break;
    case FUNCTION_CIRCLE          : circleFunction_         ->setChecked(true); break;
    case FUNCTION_FILLED_CIRCLE   : filledCircleFunction_   ->setChecked(true); break;
    case FUNCTION_FLOOD_FILL      : floodFillFunction_      ->setChecked(true); break;
    case FUNCTION_LARGEST_RECT    : largestRectFunction_    ->setChecked(true); break;
    case FUNCTION_TEXT            : textFunction_           ->setChecked(true); break;
    case FUNCTION_SET_HOT_SPOT    : setHotSpotFunction_     ->setChecked(true); break;
    default                       :                                             break;
  }

  copyItem_       ->setChecked(function_ == FUNCTION_COPY);
  moveItem_       ->setChecked(function_ == FUNCTION_MOVE);
  markItem_       ->setChecked(function_ == FUNCTION_MARK);
  pointItem_      ->setChecked(function_ == FUNCTION_POINT);
  curveItem_      ->setChecked(function_ == FUNCTION_CURVE);
  lineItem_       ->setChecked(function_ == FUNCTION_LINE);
  rectItem_       ->setChecked(function_ == FUNCTION_RECTANGLE);
  fillRectItem_   ->setChecked(function_ == FUNCTION_FILLED_RECTANGLE);
  circleItem_     ->setChecked(function_ == FUNCTION_CIRCLE);
  fillCircleItem_ ->setChecked(function_ == FUNCTION_FILLED_CIRCLE);
  floodFillItem_  ->setChecked(function_ == FUNCTION_FLOOD_FILL);
  largestRectItem_->setChecked(function_ == FUNCTION_LARGEST_RECT);
  textItem_       ->setChecked(function_ == FUNCTION_TEXT);
  hotSpotItem_    ->setChecked(function_ == FUNCTION_SET_HOT_SPOT);

  guard = false;
}

void
CQPixmap::
clearHotSpot()
{
  setHotSpot(-1, -1);
}

void
CQPixmap::
setHotSpot(int x, int y)
{
  x_hot_ = x;
  y_hot_ = y;

  CImageXPMInst->setHotSpot(std::min(x, 0), std::min(y, 0));

  setChanged();

  redraw();
}

#if 0
void
CQPixmap::
wheelBgColorChanged()
{
  setBgColor(CQUtil::rgbaToColor(color_wheel_->getRGB()));
}

void
CQPixmap::
wheelFgColorChanged()
{
  setFgColor(CQUtil::rgbaToColor(color_wheel_->getRGB()));
}
#else
void
CQPixmap::
wheelColorChanged(const QColor &c)
{
  if (fg_active_)
    setFgColor(c);
  else
    setBgColor(c);
}
#endif

void
CQPixmap::
drawText(int x, int y)
{
  CFontPtr font = CQFontMgrInst->lookupFont(drawFont_);

  CImagePtr image = font.cast<CQFont>()->getStringImage(drawText_.toStdString());

  int x1 = std::min(x + image->getWidth () - 1, image_->getWidth () - 1);
  int y1 = std::min(y + image->getHeight() - 1, image_->getHeight() - 1);

  addUndoImage(QRect(x, y, x1 - x + 1, y1 - y + 1));

  if (isColorMap()) {
    CRGBA bg = image_->getColor(getTransparentColorNum());
    CRGBA fg = image_->getColor(getColorNum());

    image->twoColor(bg, fg);

    CImageCopyType old_type = CImage::setCopyType(CIMAGE_COPY_SKIP_TRANSPARENT);

    drawImage(x, y, image);

    CImage::setCopyType(old_type);
  }
  else
    drawImage(x, y, image);

  setChanged();

  redraw();
}

void
CQPixmap::
newImage()
{
  CImageNameSrc src("CQPixmap::new_image");

  image_ = CImageMgrInst->createImage(src);

  image_->setDataSize(32, 32);

  if (isColorMap()) {
    image_->addColor(CRGBA(0,0,0,1)); // black
    image_->addColor(CRGBA(1,1,1,1)); // white
    image_->addColor(CRGBA(0,0,0,0)); // transparent

    image_->setColorIndexData(1U); // white
  }
  else
    image_->setRGBAData(CRGBA(1,1,1));

  initImage();
}

void
CQPixmap::
loadImage(const std::string &fileName)
{
  CImageFileSrc src(fileName);

  CImagePtr image = CImageMgrInst->createImage(src);

  if (! image.isValid() || image->getWidth() == 0 || image->getHeight() == 0) {
    image->setDataSize(32, 32);

    if (isColorMap()) {
      image->addColor(CRGBA(0,0,0,1)); // black
      image->addColor(CRGBA(1,1,1,1)); // white
      image->addColor(CRGBA(0,0,0,0)); // transparent

      image->setColorIndexData(1U); // white
    }
    else
      image->setRGBAData(CRGBA(1,1,1));
  }

  loadImage(image);

  filename_ = fileName;

  if (filenameLabel_)
    filenameLabel_->update();

  updateStatusMessage();
}

void
CQPixmap::
loadImage(CImagePtr image)
{
  if (! image.isValid())
    return;

  filename_ = "";

  image_ = image;

  if (image_->hasColormap())
    setColorMode(COLOR_MAP);
  else
    setColorMode(COLOR_RGB);

  initImage();

  if (filenameLabel_)
    filenameLabel_->update();

  updateStatusMessage();
}

void
CQPixmap::
insertImage(const std::string &fileName)
{
  CImageFileSrc src(fileName);

  CImagePtr image = CImageMgrInst->createImage(src);

  if (! image.isValid())
    return;

  if (isColorMap()) {
    image->convertToColorIndex();

    int num_colors = image->getNumColors();

    for (int i = 0; i < num_colors; ++i)
      image_->addColor(image->getColor(i));
  }
  else
    image->convertToRGB();

  // draw image
  int left, bottom, right, top;

  image_->getWindow(&left, &bottom, &right, &top);

  drawImage(left, bottom, image);

  initImage();
}

void
CQPixmap::
initImage()
{
  int size = std::max(image_->getWidth(), image_->getHeight());

  if (size > 0)
    setGridSize(std::max(256/size, 1));

  setCanvasSize();

  initColors();

  if (sizeLabel_)
    sizeLabel_->update();

  updateStatusMessage();
}

void
CQPixmap::
setCanvasSize()
{
  // resize image canvas

  int width  = gridSize()*image_->getWidth ();
  int height = gridSize()*image_->getHeight();

  delete pixmap_;

  pixmap_ = new QPixmap(width + 1, height + 1);

  delete pixmap_painter_;

  pixmap_painter_ = new QPainter;

  setChanged();

  canvas_->resize(width + 1, height + 1);

  if (sizeLabel_)
    sizeLabel_->update();

  updateStatusMessage();
}

void
CQPixmap::
initColors()
{
  // Clear colors

  uint num_color_buttons = color_buttons_.size();

  for (uint i = 0; i < num_color_buttons; ++i)
    delete color_buttons_[i];

  color_buttons_.clear();

  delete color_spacer_;

  color_spacer_ = 0;

  // Add new color buttons

  if (isColorMap()) {
    int num_colors = image_->getNumColors();

    int black_color_num       = -1;
    int white_color_num       = -1;
    int transparent_color_num = -1;

    for (int i = 0; i < num_colors; ++i) {
      CRGBA rgba = image_->getColor(i);

      int r = int(rgba.getRed  ()*255);
      int g = int(rgba.getGreen()*255);
      int b = int(rgba.getBlue ()*255);
      int a = int(rgba.getAlpha()*255);

      if      (r ==   0 && g ==   0 && b ==   0 && a > 0)
        black_color_num = i;
      else if (r == 255 && g == 255 && b == 255 && a > 0)
        white_color_num = i;
      else if (a < 127)
        transparent_color_num = i;
    }

    if (black_color_num == -1 && num_colors < 256) {
      black_color_num = image_->addColor(CRGB(0));

      ++num_colors;
    }

    if (white_color_num == -1 && num_colors < 256) {
      white_color_num = image_->addColor(CRGB(1));

      ++num_colors;
    }

    if (transparent_color_num == -1 && num_colors < 256) {
      transparent_color_num = image_->addColor(CRGBA(0,0,0,0));

      ++num_colors;
    }

    int num_added = 0;

    if (transparent_color_num != -1) {
      addColorButton(transparent_color_num);

      ++num_added;
    }

    if (black_color_num != -1) {
      addColorButton(black_color_num);

      ++num_added;
    }

    if (white_color_num != -1) {
      addColorButton(white_color_num);

      ++num_added;
    }

    num_colors = image_->getNumColors();

    for (int i = 0; i < num_colors; ++i) {
      if (i == transparent_color_num ||
          i == black_color_num ||
          i == white_color_num)
        continue;

      addColorButton(i);

      ++num_added;
    }

    //---

    if (num_added < 8) {
      for (int i = num_added; i < 8; ++i)
        addColorSpacer(i);
    }

    //---

    int row = (num_colors + numColorColumns_ - 1)/numColorColumns_;

    color_spacer_ = new QWidget;

    color_spacer_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    colormap_widget_layout_->addWidget(color_spacer_, row, 0);

    //---

    setFgColorNum         (black_color_num);
    setBgColorNum         (white_color_num);
    setTransparentColorNum(transparent_color_num);
  }
}

void
CQPixmap::
addColorButton(int i)
{
  CQPixmapColorButton *button = new CQPixmapColorButton(this, i);

  colormap_widget_layout_->addWidget(button, i / numColorColumns_, i % numColorColumns_);

  color_buttons_.push_back(button);
}

void
CQPixmap::
addColorSpacer(int i)
{
  QLabel *spacer = new QLabel(this);

  colormap_widget_layout_->addWidget(spacer, i / numColorColumns_, i % numColorColumns_);
}

CImagePtr
CQPixmap::
getImage(int x, int y, int width, int height)
{
  return image_->subImage(x, y, width, height);
}

void
CQPixmap::
drawImage(int x, int y, CImagePtr image)
{
  if (isColorMap())
    image->convertToColorIndex();
  else
    image->convertToRGB();

  image_->subCopyFrom(image, 0, 0, -1, -1, x, y);
}

void
CQPixmap::
rotate(int angle)
{
  int x1, y1, x2, y2;

  image_->getWindow(&x1, &y1, &x2, &y2);

  int width  = x2 - x1;
  int height = y2 - y1;

  int size = std::min(width, height);

  int xx1 = (x1 + x2 - size)/2;
  int yy1 = (y1 + y2 - size)/2;
  int xx2 = (x1 + x2 + size)/2;
  int yy2 = (y1 + y2 + size)/2;

  image_->setWindow(xx1, yy1, xx2, yy2);

  CImagePtr image = image_->rotate(angle);

  drawImage(xx1, yy1, image);

  setChanged();

  redraw();
}

void
CQPixmap::
floodFill(int x, int y)
{
  if (isColorMap())
    image_->floodFill(x, y, getColorNum());
  else
    image_->floodFill(x, y, CQUtil::colorToRGBA(getColor()));

  setChanged();

  redraw();
}

void
CQPixmap::
largestRect(int x, int y)
{
  if (isColorMap())
    image_->fillLargestRect(x, y, getColorNum());
  else
    image_->fillLargestRect(x, y, CQUtil::colorToRGBA(getColor()));

  setChanged();

  redraw();
}

void
CQPixmap::
clear(int left, int bottom, int right, int top)
{
  if (isColorMap())
    image_->setColorIndexData(getBgColorNum(), left, bottom, right, top);
  else
    image_->setRGBAData(CQUtil::colorToRGBA(getBgColor()));

  setChanged();

  redraw();
}

void
CQPixmap::
resizeImage(int width, int height)
{
  CImagePtr image = image_->dup();

  image->setDataSize(width, height);

  image->subCopyFrom(image_, 0, 0, width, height);

  image_ = image;

  setCanvasSize();
}

void
CQPixmap::
rescaleImage(int width, int height, bool keep_aspect)
{
  if (keep_aspect)
    image_ = image_->resizeKeepAspect(width, height);
  else
    image_ = image_->resize(width, height);

  setCanvasSize();

  thumbnail_canvas_->update();
}

void
CQPixmap::
redraw()
{
  canvas_->update();

  thumbnail_canvas_->update();
}

QPixmap *
CQPixmap::
getPixmap()
{
  if      (xor_mode_ == XOR_NONE) {
    if (changed_) {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      int canvas_width  = canvas_->width ();
      int canvas_height = canvas_->height();

      int width  = image_->getWidth ();
      int height = image_->getHeight();

      painter_ = pixmap_painter_;

      startPainter();

      QColor color(178,178,178);

      QBrush brush;

      brush.setColor(color);
      brush.setStyle(Qt::SolidPattern);

      painter_->fillRect(0, 0, canvas_width, canvas_height, brush);

      int y = 0;

      for (int j = 0; j < height; ++j, y += gridSize()) {
        if (y >= canvas_height || y + gridSize() < 0)
          continue;

        int x = 0;

        for (int i = 0; i < width; ++i, x += gridSize()) {
          if (x >= canvas_width || x + gridSize() < 0)
            continue;

          drawCanvasPoint(i, j);
        }
      }

      if (mark_.isValid()) {
        Qt::BrushStyle brushStyle = Qt::Dense2Pattern;

        int xs = std::min(mark_.getValue().left(), mark_.getValue().right());
        int ys = std::min(mark_.getValue().bottom(), mark_.getValue().top());
        int xe = std::max(mark_.getValue().left(), mark_.getValue().right());
        int ye = std::max(mark_.getValue().bottom(), mark_.getValue().top());

        for (int x = xs; x <= xe; ++x) {
          drawCanvasRect(x, ys, brushStyle);
          drawCanvasRect(x, ye, brushStyle);
        }

        for (int y = ys; y <= ye; ++y) {
          drawCanvasRect(xs, y, brushStyle);
          drawCanvasRect(xe, y, brushStyle);
        }
      }

      if (x_hot_ >= 0 && y_hot_ >= 0) {
        int px, py;

        windowToPixel(x_hot_, y_hot_, &px, &py);

        QColor color(0,0,0);

        brush.setColor(color);
        brush.setStyle(Qt::CrossPattern);

        if (gridSize() > 1)
          painter_->fillRect(px, py, gridSize(), gridSize(), brush);
        else
          painter_->drawPoint(px, py);
      }

      endPainter();

      QApplication::restoreOverrideCursor();

      changed_ = false;
    }

    return pixmap_;
  }
  else {
    QPainter painter(&xor_pixmap_);

    painter_ = &painter;

    painter_->drawPixmap(0, 0, *pixmap_);

    if      (xor_mode_ == XOR_LINE) {
      drawRectLine();
    }
    else if (xor_mode_ == XOR_RECTANGLE) {
      drawRectRect();
    }
    else if (xor_mode_ == XOR_FILLED_RECTANGLE) {
      fillRectRect();
    }
    else if (xor_mode_ == XOR_CIRCLE) {
      drawRectCircle();
    }
    else if (xor_mode_ == XOR_FILLED_CIRCLE) {
      fillRectCircle();
    }

    painter_ = pixmap_painter_;

    return &xor_pixmap_;
  }
}

QColor
CQPixmap::
getColor(int x, int y) const
{
  CRGBA rgba;

  image_->getRGBAPixel(x, y, rgba);

  return CQUtil::rgbaToColor(rgba);
}

void
CQPixmap::
xorStart(XorMode mode)
{
  xor_pixmap_ = QPixmap::grabWidget(canvas_);

  xor_mode_ = mode;
}

void
CQPixmap::
xorEnd()
{
  xor_mode_ = XOR_NONE;
}

void
CQPixmap::
setXorRectStart(int x, int y)
{
  xor_rect_ = QRect(x, y, 1, 1);
}

void
CQPixmap::
setXorRectEnd(int x, int y)
{
  xor_rect_.setRight(x);
  xor_rect_.setTop  (y);
}

void
CQPixmap::
moveXorRect(int dx, int dy)
{
  xor_rect_.adjust(dx, 0, dx, 0);
  xor_rect_.adjust(0, dy, 0, dy);
}

void
CQPixmap::
setXorRectFromMark()
{
  if (mark_.isValid())
    xor_rect_ = mark_.getValue();
  else {
    int w = image_->getWidth ();
    int h = image_->getHeight();

    xor_rect_ = QRect(0, 0, w, h);
  }
}

void
CQPixmap::
setRectMark()
{
  mark_.setValue(xor_rect_);

  int x1 = xor_rect_.left (); int y1 = xor_rect_.bottom();
  int x2 = xor_rect_.right(); int y2 = xor_rect_.top   ();

  image_->setWindow(x1, y1, x2, y2);
}

void
CQPixmap::
copyArea(const QRect &from, const QRect &to)
{
  int x = from.left();
  int y = from.bottom();
  int w = from.width();
  int h = from.height();

  CImagePtr image = getImage(x, y, w, h);

  x = to.left();
  y = to.bottom();

  drawImage(x, y, image);
}

void
CQPixmap::
moveArea(const QRect &from, const QRect &to)
{
  int x = from.left();
  int y = from.bottom();
  int w = from.width();
  int h = from.height();

  CImagePtr image = getImage(x, y, w, h);

  clear(x, y, x + w - 1, y + h - 1);

  x = to.left();
  y = to.bottom();

  drawImage(x, y, image);
}

void
CQPixmap::
drawRectLine()
{
  int x1 = xor_rect_.left (); int y1 = xor_rect_.bottom();
  int x2 = xor_rect_.right(); int y2 = xor_rect_.top   ();

  drawLine(x1, y1, x2, y2);
}

void
CQPixmap::
drawLine(int x1, int y1, int x2, int y2)
{
  class Bresenham : public CBresenham {
   private:
    CQPixmap *pixmap_;

   public:
    Bresenham(CQPixmap *pixmap) :
     pixmap_(pixmap) {
    }

    void drawPoint(int x, int y) {
      pixmap_->drawPoint(x, y);
    }
  };

  Bresenham bresenham(this);

  bresenham.drawLine(x1, y1, x2, y2);
}

void
CQPixmap::
drawRectCircle()
{
  class Bresenham : public CBresenham {
   private:
    CQPixmap *pixmap_;

   public:
    Bresenham(CQPixmap *pixmap) :
     pixmap_(pixmap) {
    }

    void drawPoint(int x, int y) {
      pixmap_->drawPoint(x, y);
    }
  };

  int xc = xor_rect_.left();
  int yc = xor_rect_.bottom();

  int dx = xor_rect_.width() - 1;
  int dy = xor_rect_.height() - 1;

  int r = int(sqrt(dx*dx + dy*dy) + 0.5);

  if (xor_mode_ == XOR_NONE) {
    QRect rect(xc - r, yc - r, 2*r, 2*r);

    addUndoImage(rect);
  }

  Bresenham bresenham(this);

  bresenham.drawCircle(xc, yc, r);
}

void
CQPixmap::
fillRectCircle()
{
  class Bresenham : public CBresenham {
   private:
    CQPixmap *pixmap_;

   public:
    Bresenham(CQPixmap *pixmap) :
     pixmap_(pixmap) {
    }

    void drawPoint(int x, int y) {
      pixmap_->drawPoint(x, y);
    }
  };

  int xc = xor_rect_.left();
  int yc = xor_rect_.bottom();

  int dx = xor_rect_.width() - 1;
  int dy = xor_rect_.height() - 1;

  int r = int(sqrt(dx*dx + dy*dy) + 0.5);

  if (xor_mode_ == XOR_NONE) {
    QRect rect(xc - r, yc - r, 2*r, 2* r);

    addUndoImage(rect);
  }

  Bresenham bresenham(this);

  bresenham.fillCircle(xc, yc, r);
}

void
CQPixmap::
drawRectRect()
{
  int l = std::min(xor_rect_.left(), xor_rect_.right());
  int b = std::min(xor_rect_.bottom(), xor_rect_.top());

  int w = std::abs(xor_rect_.right() - xor_rect_.left()) + 1;
  int h = std::abs(xor_rect_.top() - xor_rect_.bottom()) + 1;

  drawRectangle(l, b, w, h);
}

void
CQPixmap::
fillRectRect()
{
  int l = std::min(xor_rect_.left(), xor_rect_.right());
  int b = std::min(xor_rect_.bottom(), xor_rect_.top());

  int w = std::abs(xor_rect_.right() - xor_rect_.left()) + 1;
  int h = std::abs(xor_rect_.top() - xor_rect_.bottom()) + 1;

  fillRectangle(l, b, w, h);
}

void
CQPixmap::
drawRectangle(int x, int y, int w, int h)
{
  int xs = x;
  int ys = y;
  int xe = x + w - 1;
  int ye = y + h - 1;

  for (x = xs; x <= xe; ++x) {
    drawPoint(x, ys);
    drawPoint(x, ye);
  }

  for (y = ys; y <= ye; ++y) {
    drawPoint(xs, y);
    drawPoint(xe, y);
  }
}

void
CQPixmap::
fillRectangle(int x, int y, int w, int h)
{
  int xs = x;
  int ys = y;
  int xe = x + w - 1;
  int ye = y + h - 1;

  for (x = xs; x <= xe; ++x)
    for (y = ys; y <= ye; ++y)
      drawPoint(x, y);
}

void
CQPixmap::
drawPoint(int x, int y)
{
  if (xor_mode_ != XOR_NONE)
    drawXorCanvasPoint(x, y);
  else {
    if (image_->validPixel(x, y)) {
      if (isColorMap())
        image_->setColorIndexPixel(x, y, getColorNum());
      else
        image_->setRGBAPixel(x, y, CQUtil::colorToRGBA(getColor()));

      drawCanvasPoint(x, y);
    }
  }
}

void
CQPixmap::
drawCanvasPoint(int x, int y)
{
  if (! image_->isTransparent(x, y)) {
    CRGBA rgba;

    image_->getRGBAPixel(x, y, rgba);

    drawCanvasRect(x, y, CQUtil::rgbaToColor(rgba));
  }
  else {
#if 0
    if (grid_ && gridSize() > 4)
      drawCanvasRect(x, y, Qt::Dense6Pattern);
    else
      drawCanvasRect(x, y, QColor(128,128,128));
#else
    drawCanvasRect(x, y, Qt::Dense6Pattern);
#endif
  }
}

void
CQPixmap::
drawCanvasRect(int x, int y, const QColor &c)
{
  int px, py;

  windowToPixel(x, y, &px, &py);

  QBrush brush;

  startPainter();

  brush.setColor(c);
  brush.setStyle(Qt::SolidPattern);

  painter_->fillRect(px, py, gridSize(), gridSize(), brush);

  if (grid_ && gridSize() > 4) {
    QColor color;

    if (qGray(c.red(), c.green(), c.blue()) > 128)
      color = QColor(0,0,0);
    else
      color = QColor(255,255,255);

    painter_->setPen(color);

    painter_->drawRect(px, py, gridSize(), gridSize());
  }

  endPainter();
}

void
CQPixmap::
drawCanvasRect(int x, int y, Qt::BrushStyle brushStyle)
{
  int px, py;

  windowToPixel(x, y, &px, &py);

  QBrush brush;

  startPainter();

  QColor c(196,196,196);

  brush.setColor(c);
  brush.setStyle(Qt::SolidPattern);

  painter_->fillRect(px, py, gridSize(), gridSize(), brush);

  brush.setColor(QColor(0,0,0));
  brush.setStyle(brushStyle);

  painter_->fillRect(px, py, gridSize(), gridSize(), brush);

  if (grid_ && gridSize() > 4) {
    QColor color(0,0,0);

    painter_->setPen(color);

    painter_->drawRect(px, py, gridSize(), gridSize());
  }

  endPainter();
}

void
CQPixmap::
drawXorCanvasPoint(int x, int y)
{
  int width  = image_->getWidth ();
  int height = image_->getHeight();

  if (x < 0 || x >= width || y < 0 || y >= height)
    return;

  int px, py;

  windowToPixel(x, y, &px, &py);

  QColor c;

  if (isColorMap())
    c = CQUtil::rgbaToColor(image_->getColor(getColorNum()));
  else
    c = getColor();

  QBrush brush;

  brush.setColor(c);
  brush.setStyle(Qt::SolidPattern);

  painter_->fillRect(px, py, gridSize(), gridSize(), brush);

  if (grid_ && gridSize() > 4) {
    QColor color(255,255,255);

    if (qGray(c.red(), c.green(), c.blue()) > 128)
      color = QColor(0,0,0);

    painter_->setPen(color);

    painter_->drawRect(px, py, gridSize(), gridSize());
  }
}

void
CQPixmap::
startPainter()
{
  if (painter_depth_ == 0)
    painter_->begin(pixmap_);

  ++painter_depth_;
}

void
CQPixmap::
endPainter()
{
  --painter_depth_;

  if (painter_depth_  == 0)
    painter_->end();
}

void
CQPixmap::
startUndoGroup()
{
  undo_->startGroup();
}

void
CQPixmap::
endUndoGroup()
{
  undo_->endGroup();

  updateUndoButtons();
}

void
CQPixmap::
addUndoImage()
{
  int x1, y1, x2, y2;

  image_->getWindow(&x1, &y1, &x2, &y2);

  addUndoImage(QRect(x1, y1, x2 - x1 + 1, y2 - x1 + 1));
}

void
CQPixmap::
addUndoImage(const QRect &rect)
{
  int x, y, w, h;

  if (! rect.isNull()) {
    x = std::min(rect.left(), rect.right());
    y = std::min(rect.bottom(), rect.top());

    w = std::abs(rect.right() - rect.left()) + 1;
    h = std::abs(rect.top() - rect.bottom()) + 1;
  }
  else {
    x = 0;
    y = 0;
    w = image_->getWidth();
    h = image_->getHeight();
  }

  CImagePtr image = getImage(x, y, w, h);

  undo_->addUndo(new CQPixmapUndoImage(this, x, y, image));

  updateUndoButtons();
}

void
CQPixmap::
updateUndoButtons()
{
  undoButton_->setEnabled(undo_->canUndo());
  redoButton_->setEnabled(undo_->canRedo());

  undoItem_->setEnabled(undo_->canUndo());
  redoItem_->setEnabled(undo_->canRedo());
}

bool
CQPixmap::
pixelToWindow(int px, int py, int *wx, int *wy)
{
  *wx = px/gridSize();
  *wy = py/gridSize();

  return (*wx >= 0 && *wx < int(image_->getWidth()) && *wy >= 0 && *wy < int(image_->getHeight()));
}

bool
CQPixmap::
windowToPixel(int wx, int wy, int *px, int *py)
{
  *px = wx*gridSize();
  *py = wy*gridSize();

  return (*px >= 0 && *px < width() && *py >= 0 && *py < height());
}

//-------------------

bool
CQPixmapUndoImage::
exec()
{
  CImagePtr image = pixmap_->getImage(x_, y_, image_->getWidth(), image_->getHeight());

  pixmap_->drawImage(x_, y_, image_);

  image_ = image;

  return true;
}


//-------------------

CQPixmapFilenameLabel::
CQPixmapFilenameLabel(CQPixmap *pixmap) :
 pixmap_(pixmap)
{
  setObjectName("filename");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  label_    = new CQPixmapStatusLabel("Filename:");
  fileName_ = new QLabel;

  layout->addWidget(label_   );
  layout->addWidget(fileName_);
}

void
CQPixmapFilenameLabel::
update()
{
  fileName_->setText(QString("%1").arg(pixmap_->filename().c_str()));
}

//-------------------

CQPixmapSizeLabel::
CQPixmapSizeLabel(CQPixmap *pixmap) :
 pixmap_(pixmap)
{
  setObjectName("size");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  label_ = new CQPixmapStatusLabel("Size:");
  size_  = new QLabel;

  layout->addWidget(label_);
  layout->addWidget(size_ );
}

void
CQPixmapSizeLabel::
update()
{
  size_->setText(QString("%1x%2").arg(pixmap_->getImage()->getWidth()).
                                  arg(pixmap_->getImage()->getHeight()));
}

//-------------------

CQPixmapPosLabel::
CQPixmapPosLabel(CQPixmap *pixmap) :
 pixmap_(pixmap)
{
  setObjectName("pos");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  label_ = new CQPixmapStatusLabel("Pos:");
  pos_   = new QLabel;

  pos_->setText("0, 0");

  QFontMetrics fm(font());

  pos_->setFixedWidth(fm.width("888, 888"));

  layout->addWidget(label_);
  layout->addWidget(pos_ );
}

void
CQPixmapPosLabel::
update(int x, int y)
{
  pos_->setText(QString("%1, %2").arg(x).arg(y));
}

//-------------------

CQPixmapGridSize::
CQPixmapGridSize(CQPixmap *pixmap) :
 pixmap_(pixmap)
{
  setObjectName("grid_size");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  label_ = new CQPixmapStatusLabel("Grid Size:");
  spin_  = new QSpinBox;

  layout->addWidget(label_);
  layout->addWidget(spin_ );

  spin_->setRange(1, 256);
  spin_->setValue(pixmap->gridSize());

  connect(spin_, SIGNAL(valueChanged(int)), pixmap, SLOT(setGridSize(int)));
}

void
CQPixmapGridSize::
update()
{
  spin_->setValue(pixmap_->gridSize());
}

//-------------------

CQPixmapFgControl::
CQPixmapFgControl(CQPixmap *pixmap) :
 pixmap_(pixmap)
{
  setObjectName("fg_control");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  QLabel *label = new CQPixmapStatusLabel("Fg:");

  button_ = new CQPixmapFgButton(pixmap_);

  layout->addWidget(label);
  layout->addWidget(button_);
}

void
CQPixmapFgControl::
update()
{
  button_->update();
}

//-------------------

CQPixmapBgControl::
CQPixmapBgControl(CQPixmap *pixmap) :
 pixmap_(pixmap)
{
  setObjectName("bg_control");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  QLabel *label = new CQPixmapStatusLabel("Bg:");

  button_ = new CQPixmapBgButton(pixmap_);

  layout->addWidget(label);
  layout->addWidget(button_);
}

void
CQPixmapBgControl::
update()
{
  button_->update();
}

//-------------------

CQPixmapStatusLabel::
CQPixmapStatusLabel(const QString &str) :
 QLabel(str)
{
  QFont sfont = font();

  QFontMetrics fm(sfont);

  setFixedHeight(fm.height() + 2);

  sfont.setPointSizeF(sfont.pointSizeF()*0.8);
  sfont.setBold(true);

  setFont(sfont);

  setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}
