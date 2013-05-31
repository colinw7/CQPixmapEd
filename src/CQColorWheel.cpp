#include <CQColorWheel.h>
#include <CColorWheel.h>
#include <CQPixmapColorTip.h>
#include <CQToolTip.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include <QGroupBox>
#include <QSpinBox>
#include <QPainter>
#include <QMouseEvent>

class CQColorWheelCanvas : public QWidget, public CColorWheel {
 public:
  CQColorWheelCanvas(CQColorWheel *wheel, int radius=0, Qt::Orientation orient=Qt::Vertical,
                     bool show_swatches=true);

 ~CQColorWheelCanvas();

  QColor getColor(int x, int y);

  void paintEvent(QPaintEvent *e);

  void mousePressEvent  (QMouseEvent *event);
  void mouseMoveEvent   (QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

  void setForeground(const QColor &rgba);

  void drawPoint(int x, int y);
  void drawLine(int x1, int y1, int x2, int y2);
  void fillRectangle(const QRect &rect);

 private:
  CQColorWheel *wheel_;
  QPainter     *painter_;
  QColor        fg_;
};

CQColorWheel::
CQColorWheel(QWidget *parent, Qt::Orientation orient, int size, bool show_swatches) :
 QWidget(parent)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QBoxLayout *layout;

  if (orient == Qt::Vertical)
    layout = new QVBoxLayout(this);
  else
    layout = new QHBoxLayout(this);

  layout->setMargin(0); layout->setSpacing(0);

  QFrame *wheelCanvas = new QFrame;

  int margin = 4;

  int ws = size/10;
  int ss = size/6;

  int w = size + 3*margin + ws;
  int h = size + 2*margin;

  if (show_swatches) {
    if (orient == Qt::Vertical)
      h += margin + ss;
    else
      w += margin + ss;
  }

  wheelCanvas->setMinimumSize(QSize(w, h));
  wheelCanvas->setMaximumSize(QSize(w, h));

  QVBoxLayout *wheelCanvasLayout = new QVBoxLayout(wheelCanvas);

  wheelCanvasLayout->setMargin(0); wheelCanvasLayout->setSpacing(0);

  layout->addWidget(wheelCanvas);

  QGroupBox *optionsGroup = new QGroupBox;

  optionsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(optionsGroup);

  QVBoxLayout *optionsGroupLayout = new QVBoxLayout(optionsGroup);

  optionsGroupLayout->setMargin(0); optionsGroupLayout->setSpacing(0);

  QTabWidget *tabWidget = new QTabWidget;

  optionsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  optionsGroupLayout->addWidget(tabWidget);

  QWidget *rgbTab = new QWidget;
  QWidget *hsvTab = new QWidget;
  QWidget *optTab = new QWidget;

  rgbTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  hsvTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  optTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  tabWidget->addTab(rgbTab, "RGB");
  tabWidget->addTab(hsvTab, "HSV");
  tabWidget->addTab(optTab, "Options");

  QGridLayout *rgbLayout = new QGridLayout(rgbTab);

  rgbLayout->setMargin(2); rgbLayout->setSpacing(2);

  QLabel *redLabel   = new QLabel("R");
  QLabel *greenLabel = new QLabel("G");
  QLabel *blueLabel  = new QLabel("B");

  redSlider   = new QSlider(Qt::Horizontal); redValue   = new QSpinBox();
  greenSlider = new QSlider(Qt::Horizontal); greenValue = new QSpinBox();
  blueSlider  = new QSlider(Qt::Horizontal); blueValue  = new QSpinBox();

  redSlider  ->setRange(0, 255); redValue  ->setRange(0, 255);
  greenSlider->setRange(0, 255); greenValue->setRange(0, 255);
  blueSlider ->setRange(0, 255); blueValue ->setRange(0, 255);

  rgbLayout->addWidget(redLabel   , 0, 0);
  rgbLayout->addWidget(redSlider  , 0, 1);
  rgbLayout->addWidget(redValue   , 0, 2);
  rgbLayout->addWidget(greenLabel , 1, 0);
  rgbLayout->addWidget(greenSlider, 1, 1);
  rgbLayout->addWidget(greenValue , 1, 2);
  rgbLayout->addWidget(blueLabel  , 2, 0);
  rgbLayout->addWidget(blueSlider , 2, 1);
  rgbLayout->addWidget(blueValue  , 2, 2);

  rgbLayout->setRowStretch(3, 1);

  QGridLayout *hsvLayout = new QGridLayout(hsvTab);

  hsvLayout->setMargin(2); hsvLayout->setSpacing(2);

  QLabel *hueLabel   = new QLabel("H");
  QLabel *satLabel   = new QLabel("S");
  QLabel *valueLabel = new QLabel("V");

  hueSlider   = new QSlider(Qt::Horizontal); hueValue   = new QSpinBox();
  satSlider   = new QSlider(Qt::Horizontal); satValue   = new QSpinBox();
  valueSlider = new QSlider(Qt::Horizontal); valueValue = new QSpinBox();

  hueSlider  ->setRange(0, 359); hueValue  ->setRange(0, 359);
  satSlider  ->setRange(0, 255); satValue  ->setRange(0, 255);
  valueSlider->setRange(0, 255); valueValue->setRange(0, 255);

  hsvLayout->addWidget(hueLabel   , 0, 0);
  hsvLayout->addWidget(hueSlider  , 0, 1);
  hsvLayout->addWidget(hueValue   , 0, 2);
  hsvLayout->addWidget(satLabel   , 1, 0);
  hsvLayout->addWidget(satSlider  , 1, 1);
  hsvLayout->addWidget(satValue   , 1, 2);
  hsvLayout->addWidget(valueLabel , 2, 0);
  hsvLayout->addWidget(valueSlider, 2, 1);
  hsvLayout->addWidget(valueValue , 2, 2);

  hsvLayout->setRowStretch(3, 1);

  QGridLayout *optLayout = new QGridLayout(optTab);

  optLayout->setMargin(2); optLayout->setSpacing(2);

  QLabel    *baseLabel = new QLabel("Base");
  QComboBox *baseCombo = new QComboBox;

  baseCombo->addItems(QStringList() << "Red" << "Green" << "Blue");

  optLayout->addWidget(baseLabel, 0, 0);
  optLayout->addWidget(baseCombo, 0, 1);

  optLayout->setRowStretch(1, 1);

  //----------

  canvas_ = new CQColorWheelCanvas(this, 0, orient, show_swatches);

  wheelCanvasLayout->addWidget(canvas_);

  setFgRGB(QColor(0  ,0  ,0  ));
  setBgRGB(QColor(255,255,255));

  // RGB
  connect(redSlider  , SIGNAL(valueChanged(int)), this, SLOT(updateRGBFromSlider()));
  connect(greenSlider, SIGNAL(valueChanged(int)), this, SLOT(updateRGBFromSlider()));
  connect(blueSlider , SIGNAL(valueChanged(int)), this, SLOT(updateRGBFromSlider()));
  connect(redValue   , SIGNAL(valueChanged(int)), this, SLOT(updateRGBFromSpin()));
  connect(greenValue , SIGNAL(valueChanged(int)), this, SLOT(updateRGBFromSpin()));
  connect(blueValue  , SIGNAL(valueChanged(int)), this, SLOT(updateRGBFromSpin()));

  // HSV
  connect(hueSlider  , SIGNAL(valueChanged(int)), this, SLOT(updateHSVFromSlider()));
  connect(satSlider  , SIGNAL(valueChanged(int)), this, SLOT(updateHSVFromSlider()));
  connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(updateHSVFromSlider()));
  connect(hueValue   , SIGNAL(valueChanged(int)), this, SLOT(updateHSVFromSpin()));
  connect(satValue   , SIGNAL(valueChanged(int)), this, SLOT(updateHSVFromSpin()));
  connect(valueValue , SIGNAL(valueChanged(int)), this, SLOT(updateHSVFromSpin()));

  // Base
  connect(baseCombo, SIGNAL(activated(const QString &)), this, SLOT(updateBase(const QString &)));
}

CQColorWheel::
~CQColorWheel()
{
  delete canvas_;
}

void
CQColorWheel::
updateRGBFromSlider()
{
  QColor rgb(redSlider->value(), greenSlider->value(), blueSlider->value());

  setRGB(rgb);
}

void
CQColorWheel::
updateRGBFromSpin()
{
  QColor rgb(redValue->value(), greenValue->value(), blueValue->value());

  setRGB(rgb);
}

void
CQColorWheel::
updateHSVFromSlider()
{
  QColor hsv;

  hsv.setHsv(hueSlider->value(), satSlider->value(), valueSlider->value());

  setHSV(hsv);
}

void
CQColorWheel::
updateHSVFromSpin()
{
  QColor hsv;

  hsv.setHsv(hueValue->value(), satValue->value(), valueValue->value());

  setHSV(hsv);
}

void
CQColorWheel::
updateBase(const QString &str)
{
  if      (str == "Red")
    canvas_->setControlType(CColorWheel::CONTROL_RED);
  else if (str == "Green")
    canvas_->setControlType(CColorWheel::CONTROL_GREEN);
  else if (str == "Blue")
    canvas_->setControlType(CColorWheel::CONTROL_BLUE);
  else if (str == "Hue")
    canvas_->setControlType(CColorWheel::CONTROL_HUE);
  else if (str == "Saturation")
    canvas_->setControlType(CColorWheel::CONTROL_SATURATION);
  else if (str == "Value")
    canvas_->setControlType(CColorWheel::CONTROL_VALUE);

  canvas_->update();
}

void
CQColorWheel::
setBgRGB(const QColor &rgb)
{
  canvas_->setBgRGB(rgb);

  updateRGB();
}

void
CQColorWheel::
setFgRGB(const QColor &rgb)
{
  canvas_->setFgRGB(rgb);

  updateRGB();
}

void
CQColorWheel::
setRGB(const QColor &rgb)
{
  canvas_->setRGB(rgb);

  updateRGB();
}

void
CQColorWheel::
updateRGB()
{
  static bool active;

  if (active) return;

  QColor rgb = getRGB();

  active = true;

  redSlider  ->setValue(rgb.red  ());
  greenSlider->setValue(rgb.green());
  blueSlider ->setValue(rgb.blue ());

  redValue  ->setValue(rgb.red  ());
  greenValue->setValue(rgb.green());
  blueValue ->setValue(rgb.blue ());

  QColor hsv = canvas_->getHSV();

  setHSV(hsv);

  canvas_->update();

  active = false;

  if (canvas_->getMode() == CColorWheel::MODE_BACKGROUND)
    emit colorBgChanged();
  else
    emit colorFgChanged();
}

const QColor &
CQColorWheel::
getBgRGB() const
{
  return canvas_->getBgRGB();
}

const QColor &
CQColorWheel::
getFgRGB() const
{
  return canvas_->getFgRGB();
}

const QColor &
CQColorWheel::
getRGB() const
{
  return canvas_->getRGB();
}

void
CQColorWheel::
setHSV(const QColor &hsv)
{
  static bool active;

  if (active) return;

  active = true;

  hueSlider  ->setValue(int(hsv.hue       ()));
  satSlider  ->setValue(int(hsv.saturation()));
  valueSlider->setValue(int(hsv.value     ()));

  hueValue  ->setValue(int(hsv.hue       ()));
  satValue  ->setValue(int(hsv.saturation()));
  valueValue->setValue(int(hsv.value     ()));

  canvas_->setHSV(hsv);

  QColor rgb = canvas_->getRGB();

  setRGB(rgb);

  canvas_->update();

  active = false;
}

const QColor &
CQColorWheel::
getHSV() const
{
  return canvas_->getHSV();
}

//-----------------

class CQColorWheelCanvasTip : public CQToolTipIFace {
 public:
  CQColorWheelCanvasTip(CQColorWheelCanvas *canvas) :
   canvas_(canvas), widget_(0) {
  }

 ~CQColorWheelCanvasTip() {
    delete widget_;
  }

  QWidget *showWidget() {
    if (! widget_)
      widget_ = new CQPixmapColorTip;

    updateWidget();

    return widget_;
  }

  void hideWidget() {
    delete widget_;

    widget_ = 0;
  }

  bool trackMouse() const { return true; }

  void updateWidget() {
    if (! widget_) return;

    QPoint p = canvas_->mapFromGlobal(QCursor::pos());

    QColor c = canvas_->getColor(p.x(), p.y());

    widget_->setColor(c);
  }

 private:
  CQColorWheelCanvas *canvas_;
  CQPixmapColorTip   *widget_;
};

CQColorWheelCanvas::
CQColorWheelCanvas(CQColorWheel *wheel, int radius, Qt::Orientation orient, bool show_swatches) :
 QWidget(NULL), CColorWheel(radius, orient, show_swatches), wheel_(wheel)
{
  setFocusPolicy(Qt::StrongFocus);

  CQToolTip::setToolTip(this, new CQColorWheelCanvasTip(this));
}

CQColorWheelCanvas::
~CQColorWheelCanvas()
{
}

QColor
CQColorWheelCanvas::
getColor(int x, int y)
{
  QColor color(0,0,0,0);

  CColorWheel::posToRGB(x, y, color);

  return color;
}

void
CQColorWheelCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QColor rgba(255*0.93,255*0.93,255*0.93);

  painter.fillRect(rect(), QBrush(rgba));

  painter_ = &painter;

  CColorWheel::draw(width(), height());
}

void
CQColorWheelCanvas::
mousePressEvent(QMouseEvent *e)
{
  CColorWheel::select(e->pos().x(), e->pos().y());

  wheel_->setRGB(getRGB());
}

void
CQColorWheelCanvas::
mouseMoveEvent(QMouseEvent *e)
{
  CColorWheel::select(e->pos().x(), e->pos().y());

  wheel_->setRGB(getRGB());
}

void
CQColorWheelCanvas::
mouseReleaseEvent(QMouseEvent *)
{
}

void
CQColorWheelCanvas::
setForeground(const QColor &fg)
{
  fg_ = fg;

  painter_->setPen(fg);
}

void
CQColorWheelCanvas::
drawPoint(int x, int y)
{
  painter_->drawPoint(x, y);
}

void
CQColorWheelCanvas::
drawLine(int x1, int y1, int x2, int y2)
{
  painter_->drawLine(x1, y1, x2, y2);
}

void
CQColorWheelCanvas::
fillRectangle(const QRect &rect)
{
  painter_->fillRect(rect, QBrush(fg_));
}
