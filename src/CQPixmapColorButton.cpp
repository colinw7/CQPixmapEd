#include <CQPixmapColorButton.h>
#include <CQPixmapEd.h>
#include <CQPixmapColorTip.h>
#include <CQToolTip.h>

#include <QPainter>
#include <QMouseEvent>

class CQPixmapColorButtonTip : public CQToolTipIFace {
 public:
  CQPixmapColorButtonTip(CQPixmapColorButton *button) :
   button_(button), widget_(0) {
  }

 ~CQPixmapColorButtonTip() {
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

  //bool trackMouse() const { return true; }

  void updateWidget() {
    if (! widget_) return;

    QColor c = button_->getColor();

    widget_->setColor(c);
  }

 private:
  CQPixmapColorButton *button_;
  CQPixmapColorTip    *widget_;
};

//------

CQPixmapColorButton::
CQPixmapColorButton(CQPixmap *pixmap, int color_num) :
 pixmap_(pixmap), color_num_(color_num), size_(16)
{
  setObjectName(QString("color_%1").arg(color_num));

  setFixedSize(size_, size_);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  CQToolTip::setToolTip(this, new CQPixmapColorButtonTip(this));
}

CQPixmapColorButton::
~CQPixmapColorButton()
{
  CQToolTip::unsetToolTip(this);
}

QColor
CQPixmapColorButton::
getColor() const
{
  return pixmap_->getImage().color(color_num_);
}

void
CQPixmapColorButton::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QColor         color;
  Qt::BrushStyle style;

  if (color_num_ == pixmap_->getTransparentColorNum()) {
    color = QColor(0,0,0);
    style = Qt::Dense6Pattern;
  }
  else {
    color = getColor();
    style = Qt::SolidPattern;
  }

  QBrush brush;

  brush.setColor(color);
  brush.setStyle(style);

  painter.fillRect(0, 0, size_, size_, brush);
}

void
CQPixmapColorButton::
mousePressEvent(QMouseEvent *event)
{
  if      (event->button() == Qt::LeftButton)
    pixmap_->setFgColorNum(color_num_);
  else if (event->button() == Qt::RightButton)
    pixmap_->setBgColorNum(color_num_);
  else {
    if (pixmap_->isFgActive())
      pixmap_->setFgColorNum(color_num_);
    else
      pixmap_->setBgColorNum(color_num_);
  }
}
