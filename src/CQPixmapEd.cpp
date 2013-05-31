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
#include <QToolButton>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <CUndo.h>
#include <CBresenham.h>
#include <CQColorWheel.h>
#include <CQColorChooser.h>

#include <cmath>

#include <xpm/fliph.xpm>
#include <xpm/flipv.xpm>
#include <xpm/fold.xpm>
#include <xpm/up.xpm>
#include <xpm/down.xpm>
#include <xpm/left.xpm>
#include <xpm/right.xpm>
#include <xpm/rotatel.xpm>
#include <xpm/rotater.xpm>

#include <xpm/clear.xpm>
#include <xpm/set.xpm>
#include <xpm/invert.xpm>
#include <xpm/copy.xpm>
#include <xpm/move.xpm>
#include <xpm/mark.xpm>
#include <xpm/unmark.xpm>
#include <xpm/point.xpm>
#include <xpm/curve.xpm>
#include <xpm/line.xpm>
#include <xpm/rect.xpm>
#include <xpm/fill_rect.xpm>
#include <xpm/circle.xpm>
#include <xpm/fill_circle.xpm>
#include <xpm/fill.xpm>
#include <xpm/text.xpm>
#include <xpm/hot_spot.xpm>
#include <xpm/undo_16.xpm>
#include <xpm/redo_16.xpm>

CQPixmap::
CQPixmap() :
 function_             (FUNCTION_POINT),
 xor_mode_             (XOR_NONE),
 filename_             ("scratch"),
 pixmap_               (NULL),
 pixmap_painter_       (NULL),
 painter_              (NULL),
 painter_depth_        (0),
 changed_              (true),
 grid_                 (true),
 grid_size_            (1),
 bg_color_num_         (0),
 fg_color_num_         (1),
 transparent_color_num_(2),
 bg_color_             (QColor(255,255,255)),
 fg_color_             (QColor(0,0,0)),
 color_num_            (0),
 color_                (QColor(255,255,255)),
 fg_active_            (true),
 x_hot_                (-1),
 y_hot_                (-1)
{
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

    bg_button_->update();

    thumbnail_canvas_->update();

    updateStatusMessage();
  }
}

void
CQPixmap::
setFgColorNum(int color_num)
{
  if (color_num != fg_color_num_) {
    fg_color_num_ = color_num;

    fg_button_->update();

    updateStatusMessage();
  }
}

void
CQPixmap::
setBgColor(const QColor &c)
{
  if (c != bg_color_) {
    bg_color_ = c;

    bg_button_->update();

    color_wheel_->setRGB(bg_color_);

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

    color_wheel_->setRGB(fg_color_);

    fg_button_->update();

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

  fg_button_->update();
  bg_button_->update();

  if (fg_active_)
    color_wheel_->setRGB(getFgColor());
  else
    color_wheel_->setRGB(getBgColor());
}

void
CQPixmap::
setChanged()
{
  changed_ = true;
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
  undoItem_->setIcon(QIcon(QPixmap(undo_16_data)));

  connect(undoItem_, SIGNAL(triggered()), this, SLOT(undo()));

  redoItem_ = new QAction("&Redo", this); editMenu->addAction(redoItem_);

  redoItem_->setShortcut(QString("Ctrl+Y"));
  redoItem_->setStatusTip("Redo last undo");
  redoItem_->setIcon(QIcon(QPixmap(redo_16_data)));

  connect(redoItem_, SIGNAL(triggered()), this, SLOT(redo()));

  clearItem_ = new QAction("&Clear", this); editMenu->addAction(clearItem_);

  clearItem_->setStatusTip("Clear all pixels in image");
  clearItem_->setIcon(QIcon(QPixmap(clear_data)));

  connect(clearItem_, SIGNAL(triggered()), this, SLOT(clear()));

  setItem_ = new QAction("&Set", this); editMenu->addAction(setItem_);

  setItem_->setStatusTip("Set all pixels in image");
  setItem_->setIcon(QIcon(QPixmap(set_data)));

  connect(setItem_, SIGNAL(triggered()), this, SLOT(set()));

  invertItem_ = new QAction("&Invert", this); editMenu->addAction(invertItem_);

  invertItem_->setStatusTip("Invert image");
  invertItem_->setIcon(QIcon(QPixmap(invert_data)));

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
  copyItem_->setIcon(QIcon(QPixmap(copy_data)));

  connect(copyItem_, SIGNAL(toggled(bool)), this, SLOT(setCopyMode()));

  //----

  moveItem_ = new QAction("&Move", this); functionMenu->addAction(moveItem_);
  moveItem_->setCheckable(true);

  moveItem_->setStatusTip("Move mode");
  moveItem_->setIcon(QIcon(QPixmap(move_data)));

  connect(moveItem_, SIGNAL(toggled(bool)), this, SLOT(setMoveMode()));

  //----

  markItem_ = new QAction("&Mark", this); functionMenu->addAction(markItem_);
  markItem_->setCheckable(true);

  markItem_->setStatusTip("Mark mode");
  markItem_->setIcon(QIcon(QPixmap(mark_data)));

  connect(markItem_, SIGNAL(toggled(bool)), this, SLOT(setMarkMode()));

  //----

  unmarkItem_ = new QAction("&Unmark", this); functionMenu->addAction(unmarkItem_);

  unmarkItem_->setStatusTip("Unmark mode");
  unmarkItem_->setIcon(QIcon(QPixmap(unmark_data)));

  connect(unmarkItem_, SIGNAL(triggered()), this, SLOT(unmark()));

  //----

  pointItem_ = new QAction("&Point", this); functionMenu->addAction(pointItem_);
  pointItem_->setCheckable(true); pointItem_->setChecked(true);

  pointItem_->setStatusTip("Point mode");
  pointItem_->setIcon(QIcon(QPixmap(point_data)));

  connect(pointItem_, SIGNAL(toggled(bool)), this, SLOT(setPointMode()));

  //----

  curveItem_ = new QAction("&Curve", this); functionMenu->addAction(curveItem_);
  curveItem_->setCheckable(true);

  curveItem_->setStatusTip("Curve mode");
  curveItem_->setIcon(QIcon(QPixmap(curve_data)));

  connect(curveItem_, SIGNAL(toggled(bool)), this, SLOT(setCurveMode()));

  //----

  lineItem_ = new QAction("&Line", this); functionMenu->addAction(lineItem_);
  lineItem_->setCheckable(true);

  lineItem_->setStatusTip("Line mode");
  lineItem_->setIcon(QIcon(QPixmap(line_data)));

  connect(lineItem_, SIGNAL(toggled(bool)), this, SLOT(setLineMode()));

  //----

  rectItem_ = new QAction("&Rectangle", this); functionMenu->addAction(rectItem_);
  rectItem_->setCheckable(true);

  rectItem_->setStatusTip("Rectangle mode");
  rectItem_->setIcon(QIcon(QPixmap(rect_data)));

  connect(rectItem_, SIGNAL(toggled(bool)), this, SLOT(setRectangleMode()));

  //----

  fillRectItem_ = new QAction("&Filled Rectangle", this); functionMenu->addAction(fillRectItem_);
  fillRectItem_->setCheckable(true);

  fillRectItem_->setStatusTip("Filled rectangle mode");
  fillRectItem_->setIcon(QIcon(QPixmap(fill_rect_data)));

  connect(fillRectItem_, SIGNAL(toggled(bool)), this, SLOT(setFilledRectangleMode()));

  //----

  circleItem_ = new QAction("&Circle", this); functionMenu->addAction(circleItem_);
  circleItem_->setCheckable(true);

  circleItem_->setStatusTip("Circle mode");
  circleItem_->setIcon(QIcon(QPixmap(circle_data)));

  connect(circleItem_, SIGNAL(toggled(bool)), this, SLOT(setCircleMode()));

  //----

  fillCircleItem_ = new QAction("&Filled Circle", this); functionMenu->addAction(fillCircleItem_);
  fillCircleItem_->setCheckable(true);

  fillCircleItem_->setStatusTip("Filled circle mode");
  fillCircleItem_->setIcon(QIcon(QPixmap(fill_circle_data)));

  connect(fillCircleItem_, SIGNAL(toggled(bool)), this, SLOT(setFilledCircleMode()));

  //----

  floodFillItem_ = new QAction("&Flood Fill", this); functionMenu->addAction(floodFillItem_);
  floodFillItem_->setCheckable(true);

  floodFillItem_->setStatusTip("Flood fill");
  floodFillItem_->setIcon(QIcon(QPixmap(fill_data)));

  connect(floodFillItem_, SIGNAL(toggled(bool)), this, SLOT(setFloodFillMode()));

  //----

  textItem_ = new QAction("&Text", this); functionMenu->addAction(textItem_);
  textItem_->setCheckable(true);

  textItem_->setStatusTip("Text mode");
  textItem_->setIcon(QIcon(QPixmap(text_data)));

  connect(textItem_, SIGNAL(toggled(bool)), this, SLOT(setTextMode()));

  //----

  hotSpotItem_ = new QAction("&Set Hot Spot", this); functionMenu->addAction(hotSpotItem_);
  hotSpotItem_->setCheckable(true);

  hotSpotItem_->setStatusTip("Set Hot Spot mode");
  hotSpotItem_->setIcon(QIcon(QPixmap(hot_spot_data)));

  connect(hotSpotItem_, SIGNAL(toggled(bool)), this, SLOT(setHotSpotMode()));

  //--------

  QMenu *moveMenu = new QMenu("&Move", this);

  menuBar()->addMenu(moveMenu);

  leftItem_ = new QAction("&Left", this); moveMenu->addAction(leftItem_);

  leftItem_->setStatusTip("Scroll image left");
  leftItem_->setIcon(QIcon(QPixmap(left_xpm)));

  connect(leftItem_, SIGNAL(triggered()), this, SLOT(scrollLeft()));

  //----

  rightItem_ = new QAction("&Right", this); moveMenu->addAction(rightItem_);

  rightItem_->setStatusTip("Scroll image right");
  rightItem_->setIcon(QIcon(QPixmap(right_xpm)));

  connect(rightItem_, SIGNAL(triggered()), this, SLOT(scrollRight()));

  //----

  upItem_ = new QAction("&Up", this); moveMenu->addAction(upItem_);

  upItem_->setStatusTip("Scroll image up");
  upItem_->setIcon(QIcon(QPixmap(up_xpm)));

  connect(upItem_, SIGNAL(triggered()), this, SLOT(scrollUp()));

  //----

  downItem_ = new QAction("&Down", this); moveMenu->addAction(downItem_);

  downItem_->setStatusTip("Scroll image down");
  downItem_->setIcon(QIcon(QPixmap(down_xpm)));

  connect(downItem_, SIGNAL(triggered()), this, SLOT(scrollDown()));

  //----

  flipXItem_ = new QAction("&Flip X", this); moveMenu->addAction(flipXItem_);

  flipXItem_->setStatusTip("Flip image in X direction");
  flipXItem_->setIcon(QIcon(QPixmap(fliph_xpm)));

  connect(flipXItem_, SIGNAL(triggered()), this, SLOT(flipH()));

  //----

  flipYItem_ = new QAction("&Flip Y", this); moveMenu->addAction(flipYItem_);

  flipYItem_->setStatusTip("Flip image in Y direction");
  flipYItem_->setIcon(QIcon(QPixmap(flipv_xpm)));

  connect(flipYItem_, SIGNAL(triggered()), this, SLOT(flipV()));

  //----

  foldItem_ = new QAction("&Fold", this); moveMenu->addAction(foldItem_);

  foldItem_->setStatusTip("Flip image in both directions");
  foldItem_->setIcon(QIcon(QPixmap(fold_xpm)));

  connect(foldItem_, SIGNAL(triggered()), this, SLOT(flipHV()));

  //----

  rotateRItem_ = new QAction("&Rotate Right", this); moveMenu->addAction(rotateRItem_);

  rotateRItem_->setStatusTip("Rotate image right");
  rotateRItem_->setIcon(QIcon(QPixmap(rotater_xpm)));

  connect(rotateRItem_, SIGNAL(triggered()), this, SLOT(rotateRight()));

  //----

  rotateLItem_ = new QAction("&Rotate Left", this); moveMenu->addAction(rotateLItem_);

  rotateLItem_->setStatusTip("Rotate image left");
  rotateLItem_->setIcon(QIcon(QPixmap(rotatel_xpm)));

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

  editToolBar_->setIconSize(QSize(16,16));

  editToolBar_->addAction(undoItem_);
  editToolBar_->addAction(redoItem_);
  editToolBar_->addAction(clearItem_);
  editToolBar_->addAction(setItem_);
  editToolBar_->addAction(invertItem_);

  //------

  functionToolBar_ = new QToolBar("&Function", this);

  addToolBar(Qt::LeftToolBarArea, functionToolBar_);

  functionToolBar_->setIconSize(QSize(16,16));

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
  functionToolBar_->addAction(textItem_);
  functionToolBar_->addAction(hotSpotItem_);

  //------

  moveToolBar_ = new QToolBar("&Move", this);

  addToolBar(Qt::TopToolBarArea, moveToolBar_);

  moveToolBar_->setIconSize(QSize(16,16));

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

  QHBoxLayout *layout = new QHBoxLayout(widget);
  layout->setMargin(2); layout->setSpacing(2);

  //-------

  controlArea_ = new QWidget;

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

  controlLayout->addWidget(scroll_buttons);

  QGridLayout *scroll_buttons_layout = new QGridLayout(scroll_buttons);
  scroll_buttons_layout->setMargin(2); scroll_buttons_layout->setSpacing(2);

  QToolButton *flip_x_button  = new QToolButton; flip_x_button ->setIcon(QPixmap(fliph_xpm));
  QToolButton *flip_y_button  = new QToolButton; flip_y_button ->setIcon(QPixmap(flipv_xpm));
  QToolButton *fold_button    = new QToolButton; fold_button   ->setIcon(QPixmap(fold_xpm));
  QToolButton *up_button      = new QToolButton; up_button     ->setIcon(QPixmap(up_xpm));
  QToolButton *down_button    = new QToolButton; down_button   ->setIcon(QPixmap(down_xpm));
  QToolButton *left_button    = new QToolButton; left_button   ->setIcon(QPixmap(left_xpm));
  QToolButton *right_button   = new QToolButton; right_button  ->setIcon(QPixmap(right_xpm));
  QToolButton *rrotate_button = new QToolButton; rrotate_button->setIcon(QPixmap(rotater_xpm));
  QToolButton *lrotate_button = new QToolButton; lrotate_button->setIcon(QPixmap(rotatel_xpm));

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

  layout->addWidget(color_canvas);

  QVBoxLayout *color_canvas_layout = new QVBoxLayout(color_canvas);
  color_canvas_layout->setMargin(2); color_canvas_layout->setSpacing(2);

  //-------

  QWidget *colorControl = new QWidget;

  colorControl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  color_canvas_layout->addWidget(colorControl);

  QHBoxLayout *colorControlLayout = new QHBoxLayout(colorControl);

  colorControlLayout->setMargin(2); colorControlLayout->setSpacing(2);

  //---

  QFrame *fg_bg_control = new QFrame;

  fg_bg_control->setFrameShape (QFrame::Panel);
  fg_bg_control->setFrameShadow(QFrame::Sunken);

  colorControlLayout->addWidget(fg_bg_control);

  QVBoxLayout *fg_bg_controlLayout = new QVBoxLayout(fg_bg_control);
  fg_bg_controlLayout->setMargin(2); fg_bg_controlLayout->setSpacing(2);

  QWidget *fg_control = new QWidget;

  fg_bg_controlLayout->addWidget(fg_control);

  QHBoxLayout *fg_controlLayout = new QHBoxLayout(fg_control);
  fg_controlLayout->setMargin(2); fg_controlLayout->setSpacing(2);

  QLabel *fg_label = new QLabel("Fg");

  fg_button_ = new CQPixmapFgButton(this);

  fg_controlLayout->addWidget(fg_label);
  fg_controlLayout->addWidget(fg_button_);

  QWidget *bg_control = new QWidget;

  fg_bg_controlLayout->addWidget(bg_control);

  QHBoxLayout *bg_controlLayout = new QHBoxLayout(bg_control);
  bg_controlLayout->setMargin(2); bg_controlLayout->setSpacing(2);

  QLabel *bg_label = new QLabel("Bg");

  bg_button_ = new CQPixmapBgButton(this);

  bg_controlLayout->addWidget(bg_label);
  bg_controlLayout->addWidget(bg_button_);

  fg_bg_controlLayout->addStretch();

  //---

  color_stack_ = new QStackedWidget;

  colorControlLayout->addWidget(color_stack_);

  QFrame *colormap_widget = new QFrame;

  colormap_widget->setFrameShape (QFrame::Panel);
  colormap_widget->setFrameShadow(QFrame::Sunken);

  colormap_widget_layout_ = new QGridLayout(colormap_widget);
  colormap_widget_layout_->setMargin(2); colormap_widget_layout_->setSpacing(2);

  color_stack_->insertWidget(0, colormap_widget);

  //---

  QFrame *colorrgb_widget = new QFrame;

  colorrgb_widget->setFrameShape (QFrame::Panel);
  colorrgb_widget->setFrameShadow(QFrame::Sunken);

  QHBoxLayout *colorrgb_widget_layout = new QHBoxLayout(colorrgb_widget);
  colorrgb_widget_layout->setMargin(2); colorrgb_widget_layout->setSpacing(2);

  color_wheel_ = new CQColorWheel(0, Qt::Horizontal, 100, false);

  connect(color_wheel_, SIGNAL(colorBgChanged()), this, SLOT(wheelBgColorChanged()));
  connect(color_wheel_, SIGNAL(colorFgChanged()), this, SLOT(wheelFgColorChanged()));

  colorrgb_widget_layout->addWidget(color_wheel_);

  color_stack_->insertWidget(1, colorrgb_widget);

  //---

  colorControlLayout->addStretch();

  //---

  QFrame *thumbnail_frame = new QFrame;

  thumbnail_frame->setFrameShape (QFrame::Panel);
  thumbnail_frame->setFrameShadow(QFrame::Sunken);

  colorControlLayout->addWidget(thumbnail_frame);

  QHBoxLayout *thumbnail_layout = new QHBoxLayout(thumbnail_frame);
  thumbnail_layout->setMargin(2); thumbnail_layout->setSpacing(2);

  thumbnail_canvas_ = new CQThumbnailCanvas(this);

  thumbnail_layout->addWidget(thumbnail_canvas_);

  //-------

  QFrame *canvas_frame = new QFrame;

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
  positionLabel_ = new QLabel("0, 0");

  QStatusBar *sbar = statusBar();

  sbar->insertPermanentWidget(0, positionLabel_);

  updateStatusMessage();

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
  QString msg;

  QString bg_str, fg_str;

  if (image_.isColorMap()) {
    //bg = image_.color(getBgColorNum());
    //fg = image_.color(getFgColorNum());

    int bg_num = getBgColorNum();

    bg_str = QString("%1").arg(bg_num) + "(";

    if (bg_num < 0)
      bg_str += "#0000000";
    else {
      QColor bg = image_.color(bg_num);

      bg_str += bg.name();
    }

    bg_str += ")";

    int fg_num = getFgColorNum();

    fg_str = QString("%1").arg(fg_num) + "(";

    if (fg_num < 0)
      fg_str += "#0000000";
    else {
      QColor fg = image_.color(fg_num);

      fg_str += fg.name();
    }

    fg_str += ")";
  }
  else {
    bg_str = bg_color_.name();
    fg_str = fg_color_.name();
  }

  msg = QString("Filename: %1, Size %2x%3, Grid Size %4, Bg %5, Fg %6").
                arg(filename_.c_str()).arg(image_.width()).arg(image_.height()).
                arg(grid_size_).arg(bg_str).arg(fg_str);

  QStatusBar *sbar = statusBar();

  sbar->showMessage(msg);
}

void
CQPixmap::
updatePosition(int x, int y)
{
  positionLabel_->setText(QString("%1, %2").arg(x).arg(y));
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
#if 0
  CFileType type = getImageTypeFromName(filename_);

  if (type == CFILE_TYPE_NONE)
    type = CFILE_TYPE_IMAGE_XPM;

  image_.write(filename_, type);
#endif
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

  updateStatusMessage();
}

void
CQPixmap::
promptResizeImage()
{
  CQPixmapResizeDialog *dialog = new CQPixmapResizeDialog(this);

  dialog->init(image_.width(), image_.height());

  connect(dialog, SIGNAL(resize(int, int)), this, SLOT(resizeImage(int, int)));

  dialog->show();
}

void
CQPixmap::
promptRescaleImage()
{
  CQPixmapRescaleDialog *dialog = new CQPixmapRescaleDialog(this);

  dialog->init(image_.width(), image_.height(), false);

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
  ++grid_size_;

  grid_size_ = std::min(std::max(grid_size_, 1), 64);

  setCanvasSize();
}

void
CQPixmap::
zoomOut()
{
  --grid_size_;

  grid_size_ = std::min(std::max(grid_size_, 1), 64);

  setCanvasSize();
}

void
CQPixmap::
zoomFull()
{
  grid_size_ = 1;

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
  setColorMode(active ? QImage::Format_Indexed8 : QImage::Format_ARGB32);
}

void
CQPixmap::
setColorRGB(bool active)
{
  setColorMode(active ? QImage::Format_ARGB32 : QImage::Format_Indexed8);
}

void
CQPixmap::
setColorMode(QImage::Format format, bool force)
{
  if (! force && format == image_.format())
    return;

  color_stack_->setCurrentIndex(format == QImage::Format_Indexed8 ? 0 : 1);

  if (format == QImage::Format_Indexed8)
    image_.convertToColorIndex();
  else
    image_.convertToRGB();

  colorMapItem_->setChecked(format == QImage::Format_Indexed8);
  colorRGBItem_->setChecked(format == QImage::Format_ARGB32  );

  addColorItem_->setEnabled(format == QImage::Format_Indexed8);

  setChanged();

  initColors();

  redraw();
}

void
CQPixmap::
addColorDialog()
{
  if (! image_.isColorMap())
    return;

  CQPixmapAddColorDialog *dialog = new CQPixmapAddColorDialog(this);

  connect(dialog, SIGNAL(addColor(const QString &)), this, SLOT(addColor(const QString &)));

  dialog->show();
}

void
CQPixmap::
addColor(const QString &name)
{
  if (! image_.isColorMap())
    return;

  QColor c(name);

  image_.addColor(c);

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

  if (image_.isColorMap())
    image_.fill(getBgColorNum());
  else
    image_.fill(getBgColor());

  setChanged();

  redraw();
}

void
CQPixmap::
set()
{
  addUndoImage();

  if (image_.isColorMap())
    image_.fill(getFgColorNum());
  else
    image_.fill(getFgColor());

  setChanged();

  redraw();
}

void
CQPixmap::
invert()
{
  addUndoImage();

  image_.invert();

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

  image_.resetWindow();
}

void
CQPixmap::
flipH()
{
  addUndoImage();

  image_.flipH();

  setChanged();

  redraw();
}

void
CQPixmap::
flipV()
{
  addUndoImage();

  image_.flipV();

  setChanged();

  redraw();
}

void
CQPixmap::
flipHV()
{
  addUndoImage();

  image_.flipH();
  image_.flipV();

  setChanged();

  redraw();
}

void
CQPixmap::
scrollLeft()
{
  addUndoImage();

  image_.scrollX(-1);

  setChanged();

  redraw();
}

void
CQPixmap::
scrollRight()
{
  addUndoImage();

  image_.scrollX(1);

  setChanged();

  redraw();
}

void
CQPixmap::
scrollDown()
{
  addUndoImage();

  image_.scrollY(-1);

  setChanged();

  redraw();
}

void
CQPixmap::
scrollUp()
{
  addUndoImage();

  image_.scrollY(1);

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
    case FUNCTION_TEXT            : textFunction_           ->setChecked(true); break;
    case FUNCTION_SET_HOT_SPOT    : setHotSpotFunction_     ->setChecked(true); break;
    default                       :                                             break;
  }

  copyItem_      ->setChecked(function_ == FUNCTION_COPY);
  moveItem_      ->setChecked(function_ == FUNCTION_MOVE);
  markItem_      ->setChecked(function_ == FUNCTION_MARK);
  pointItem_     ->setChecked(function_ == FUNCTION_POINT);
  curveItem_     ->setChecked(function_ == FUNCTION_CURVE);
  lineItem_      ->setChecked(function_ == FUNCTION_LINE);
  rectItem_      ->setChecked(function_ == FUNCTION_RECTANGLE);
  fillRectItem_  ->setChecked(function_ == FUNCTION_FILLED_RECTANGLE);
  circleItem_    ->setChecked(function_ == FUNCTION_CIRCLE);
  fillCircleItem_->setChecked(function_ == FUNCTION_FILLED_CIRCLE);
  floodFillItem_ ->setChecked(function_ == FUNCTION_FLOOD_FILL);
  textItem_      ->setChecked(function_ == FUNCTION_TEXT);
  hotSpotItem_   ->setChecked(function_ == FUNCTION_SET_HOT_SPOT);

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

  setHotSpot(std::min(x, 0), std::min(y, 0));

  setChanged();

  redraw();
}

void
CQPixmap::
wheelBgColorChanged()
{
  setBgColor(color_wheel_->getRGB());
}

void
CQPixmap::
wheelFgColorChanged()
{
  setFgColor(color_wheel_->getRGB());
}

void
CQPixmap::
drawText(int x, int y)
{
  QImage image = CQPixmapImage::getStringImage(drawFont_, drawText_);

  int x1 = std::min(x + image.width () - 1, image_.width () - 1);
  int y1 = std::min(y + image.height() - 1, image_.height() - 1);

  addUndoImage(QRect(x, y, x1 - x + 1, y1 - y + 1));

  if (image_.isColorMap()) {
    image = CQPixmapImage::convertToColorIndex(image);

    QColor bg = image_.color(getTransparentColorNum());
    QColor fg = image_.color(getFgColorNum());

    CQPixmapImage::twoColor(image, bg, fg);

    image_.subCopyFrom(image, 0, 0, -1, -1, x, y, true);
  }
  else
    image_.subCopyFrom(image, 0, 0, -1, -1, x, y);

  setChanged();

  redraw();
}

void
CQPixmap::
newImage()
{
  QImage image = QImage(32, 32, QImage::Format_Indexed8);

  image_.setImage(image);

  if (image_.isColorMap()) {
    image_.addColor(QColor(  0,  0,  0,255)); // black
    image_.addColor(QColor(255,255,255,255)); // white
    image_.addColor(QColor(  0,  0,  0,  0)); // transparent

    image_.fill(1U); // white
  }
  else
    image_.fill(QColor(255,255,255));

  initImage();
}

void
CQPixmap::
loadImage(const std::string &fileName)
{
  QImage image(fileName.c_str());

  if (image.isNull() || image.width() == 0 || image.height() == 0) {
    image = QImage(32, 32, QImage::Format_Indexed8);

    if (image.format() == QImage::Format_Indexed8) {
      CQPixmapImage::addColor(image, QColor(  0,  0,  0,255)); // black
      CQPixmapImage::addColor(image, QColor(255,255,255,255)); // white
      CQPixmapImage::addColor(image, QColor(  0,  0,  0,  0)); // transparent

      image.fill(1U); // white
    }
    else
      image.fill(QColor(255,255,255));
  }

  loadImage(image);

  filename_ = fileName;

  updateStatusMessage();
}

void
CQPixmap::
loadImage(QImage image)
{
  if (image.isNull())
    return;

  filename_ = "";

  // convert to best format (minimum colors)
  if (image.format() != QImage::Format_Indexed8 && image.format() != QImage::Format_ARGB32)
    image = CQPixmapImage::convertToRGB(image);

  if (image.format() == QImage::Format_ARGB32) {
    int ncolors = CQPixmapImage::countColors(image);

    if (ncolors < 256)
      image = CQPixmapImage::convertToColorIndex(image);
  }

  image_.setImage(image);

  setColorMode(image_.format(), true);

  initImage();

  updateStatusMessage();
}

void
CQPixmap::
insertImage(const std::string &fileName)
{
  QImage image(fileName.c_str());

  if (image.isNull())
    return;

  // convert to best format (minimum colors)
  if (image.format() != QImage::Format_Indexed8 && image.format() != QImage::Format_ARGB32)
    image = CQPixmapImage::convertToRGB(image);

  if (image.format() == QImage::Format_ARGB32) {
    int ncolors = CQPixmapImage::countColors(image);

    if (ncolors < 256)
      image = CQPixmapImage::convertToColorIndex(image);
  }

  // ensure common format
  // TODO: count common colors for indexed
  if      (image_.format() == QImage::Format_Indexed8 && image.format() == QImage::Format_ARGB32)
    image_.convertToRGB();
  else if (image.format() == QImage::Format_Indexed8 && image_.format() == QImage::Format_ARGB32)
    image = CQPixmapImage::convertToRGB(image);

  // add indexed colors (may overflow)
  if (image_.format() == QImage::Format_Indexed8) {
    int num_colors = image.colorCount();

    for (int i = 0; i < num_colors; ++i)
      image_.addColor(CQPixmapImage::color(image, i));
  }

  // draw image
  int left, bottom, right, top;

  image_.getWindow(&left, &bottom, &right, &top);

  drawImage(left, bottom, image);

  initImage();
}

void
CQPixmap::
initImage()
{
  int size = std::max(image_.width(), image_.height());

  if (size > 0)
    grid_size_ = std::max(256/size, 1);

  setCanvasSize();

  initColors();

  updateStatusMessage();
}

void
CQPixmap::
setCanvasSize()
{
  // resize image canvas

  int width  = grid_size_*image_.width ();
  int height = grid_size_*image_.height();

  delete pixmap_;

  pixmap_ = new QPixmap(width + 1, height + 1);

  delete pixmap_painter_;

  pixmap_painter_ = new QPainter;

  setChanged();

  canvas_->resize(width + 1, height + 1);

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

  // Add new color buttons

  if (image_.isColorMap()) {
    int num_colors = image_.colorCount();

    int black_color_num       = -1;
    int white_color_num       = -1;
    int transparent_color_num = -1;

    for (int i = 0; i < num_colors; ++i) {
      QColor c = image_.color(i);

      int r = c.red  ();
      int g = c.green();
      int b = c.blue ();
      int a = c.alpha();

      if      (r ==   0 && g ==   0 && b ==   0 && a > 0)
        black_color_num = i;
      else if (r == 255 && g == 255 && b == 255 && a > 0)
        white_color_num = i;
      else if (a < 127)
        transparent_color_num = i;
    }

    if (black_color_num == -1 && num_colors < 256) {
      black_color_num = image_.addColor(QColor(0,0,0));

      ++num_colors;
    }

    if (white_color_num == -1 && num_colors < 256) {
      white_color_num = image_.addColor(QColor(255,255,255));

      ++num_colors;
    }

    if (transparent_color_num == -1 && num_colors < 256) {
      transparent_color_num = image_.addColor(QColor(0,0,0,0));

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

    num_colors = image_.colorCount();

    for (int i = 0; i < num_colors; ++i) {
      if (i == transparent_color_num ||
          i == black_color_num ||
          i == white_color_num)
        continue;

      addColorButton(i);

      ++num_added;
    }

    if (num_added < 8) {
      for (int i = num_added; i < 8; ++i)
        addColorSpacer(i);
    }

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

  colormap_widget_layout_->addWidget(button, i / 32, i % 32);

  color_buttons_.push_back(button);
}

void
CQPixmap::
addColorSpacer(int i)
{
  QLabel *spacer = new QLabel(this);

  colormap_widget_layout_->addWidget(spacer, i / 32, i % 32);
}

QImage
CQPixmap::
getImage(int x, int y, int width, int height)
{
  return image_.subImage(x, y, width, height);
}

void
CQPixmap::
drawImage(int x, int y, QImage image)
{
  if (image.format() != image_.format()) {
    if (image_.format() == QImage::Format_Indexed8)
      image = CQPixmapImage::convertToColorIndex(image);
    else
      image = CQPixmapImage::convertToRGB(image);
  }

  image_.subCopyFrom(image, 0, 0, -1, -1, x, y);
}

void
CQPixmap::
rotate(int angle)
{
  int x1, y1, x2, y2;

  image_.getWindow(&x1, &y1, &x2, &y2);

  int width  = x2 - x1;
  int height = y2 - y1;

  int size = std::min(width, height);

  int xx1 = (x1 + x2 - size)/2;
  int yy1 = (y1 + y2 - size)/2;
  int xx2 = (x1 + x2 + size)/2;
  int yy2 = (y1 + y2 + size)/2;

  image_.setWindow(xx1, yy1, xx2, yy2);

  QImage image = image_.rotate(angle);

  drawImage(xx1, yy1, image);

  setChanged();

  redraw();
}

void
CQPixmap::
floodFill(int x, int y)
{
  if (image_.isColorMap())
    image_.floodFill(x, y, getFgColorNum());
  else
    image_.floodFill(x, y, getFgColor());

  setChanged();

  redraw();
}

void
CQPixmap::
clear(int left, int bottom, int right, int top)
{
  if (image_.isColorMap())
    image_.fill(left, bottom, right, top, getBgColorNum());
  else
    image_.fill(left, bottom, right, top, getBgColor());

  setChanged();

  redraw();
}

void
CQPixmap::
resizeImage(int width, int height)
{
  QImage image = QImage(width, height, image_.format());

  if (image_.isColorMap()) {
    int bg = CQPixmapImage::addColor(image, getBgColor());

    CQPixmapImage::fill(image, 0, 0, width - 1, height - 1, bg);
  }
  else
    CQPixmapImage::fill(image, 0, 0, width - 1, height - 1, getBgColor());

  image_.subCopyTo(0, 0, width, height, image);

  image_.setImage(image);

  setCanvasSize();
}

void
CQPixmap::
rescaleImage(int width, int height, bool keep_aspect)
{
  if (keep_aspect)
    image_.setImage(image_.resizeKeepAspect(width, height));
  else
    image_.setImage(image_.resize(width, height));

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

      int width  = image_.width ();
      int height = image_.height();

      painter_ = pixmap_painter_;

      startPainter();

      painter_->fillRect(0, 0, canvas_width, canvas_height, QColor(178,178,178));

      painter_->fillRect(0, 0, width*grid_size_, height*grid_size_, QBrush(Qt::Dense6Pattern));

      int y = 0;

      for (int j = 0; j < height; ++j, y += grid_size_) {
        if (y >= canvas_height || y + grid_size_ < 0)
          continue;

        int x = 0;

        for (int i = 0; i < width; ++i, x += grid_size_) {
          if (x >= canvas_width || x + grid_size_ < 0)
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

        QBrush brush;

        brush.setColor(QColor(0,0,0));
        brush.setStyle(Qt::CrossPattern);

        if (grid_size_ > 1)
          painter_->fillRect(px, py, grid_size_, grid_size_, brush);
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
  return image_.colorPixel(x, y);
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
    int w = image_.width ();
    int h = image_.height();

    xor_rect_ = QRect(0, 0, w, h);
  }
}

void
CQPixmap::
setRectMark()
{
  mark_.setValue(xor_rect_);

  image_.setWindow(xor_rect_);
}

void
CQPixmap::
copyArea(const QRect &from, const QRect &to)
{
  int x = from.left();
  int y = from.bottom();
  int w = from.width();
  int h = from.height();

  QImage image = getImage(x, y, w, h);

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

  QImage image = getImage(x, y, w, h);

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
    QRect rect(xc - r, yc - r, 2*r, 2*r);

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
    if (image_.valid(x, y)) {
      if (image_.isColorMap())
        image_.setPixel(x, y, getColorNum());
      else
        image_.setPixel(x, y, getColor());

      drawCanvasPoint(x, y);
    }
  }
}

void
CQPixmap::
drawCanvasPoint(int x, int y)
{
  if (! image_.isTransparent(x, y)) {
    QColor c = image_.colorPixel(x, y);

    drawCanvasRect(x, y, c);
  }
  else {
    if (grid_ && grid_size_ > 4)
      drawCanvasRect(x, y, Qt::Dense6Pattern);
    else
      drawCanvasRect(x, y, QColor(128,128,128));
  }
}

void
CQPixmap::
drawCanvasRect(int x, int y, const QColor &c)
{
  int px, py;

  windowToPixel(x, y, &px, &py);

  startPainter();

  painter_->fillRect(px, py, grid_size_, grid_size_, QColor(c));

  if (grid_ && grid_size_ > 4) {
    QColor color;

    if (qGray(c.red(), c.green(), c.blue()) > 128)
      color = QColor(0,0,0);
    else
      color = QColor(255,255,255);

    painter_->setPen(color);

    painter_->drawRect(px, py, grid_size_, grid_size_);
  }

  endPainter();
}

void
CQPixmap::
drawCanvasRect(int x, int y, Qt::BrushStyle brushStyle)
{
  int px, py;

  windowToPixel(x, y, &px, &py);

  startPainter();

  painter_->fillRect(px, py, grid_size_, grid_size_, QColor(196,196,196));

  QBrush brush;

  brush.setColor(QColor(0,0,0));
  brush.setStyle(brushStyle);

  painter_->fillRect(px, py, grid_size_, grid_size_, brush);

  if (grid_ && grid_size_ > 4) {
    QColor color(0,0,0);

    painter_->setPen(color);

    painter_->drawRect(px, py, grid_size_, grid_size_);
  }

  endPainter();
}

void
CQPixmap::
drawXorCanvasPoint(int x, int y)
{
  int width  = image_.width ();
  int height = image_.height();

  if (x < 0 || x >= width || y < 0 || y >= height)
    return;

  int px, py;

  windowToPixel(x, y, &px, &py);

  QColor c;

  if (image_.isColorMap())
    c = image_.color(getColorNum());
  else
    c = getColor();

  painter_->fillRect(px, py, grid_size_, grid_size_, QColor(c));

  if (grid_ && grid_size_ > 4) {
    QColor color(255,255,255);

    if (qGray(c.red(), c.green(), c.blue()) > 128)
      color = QColor(0,0,0);

    painter_->setPen(color);

    painter_->drawRect(px, py, grid_size_, grid_size_);
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
  QRect window;

  image_.getWindow(window);

  addUndoImage(window);
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
    w = image_.width();
    h = image_.height();
  }

  QImage image = getImage(x, y, w, h);

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
  *wx = px/grid_size_;
  *wy = py/grid_size_;

  return (*wx >= 0 && *wx < image_.width() && *wy >= 0 && *wy < image_.height());
}

bool
CQPixmap::
windowToPixel(int wx, int wy, int *px, int *py)
{
  *px = wx*grid_size_;
  *py = wy*grid_size_;

  return (*px >= 0 && *px < width() && *py >= 0 && *py < height());
}

//-------------------

bool
CQPixmapUndoImage::
exec()
{
  QImage image = pixmap_->getImage(x_, y_, image_.width(), image_.height());

  pixmap_->drawImage(x_, y_, image_);

  image_ = image;

  return true;
}
