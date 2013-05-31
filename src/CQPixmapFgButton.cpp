#include <CQPixmapFgButton.h>
#include <CQPixmapEd.h>
#include <CQPixmapColorTip.h>
#include <CQToolTip.h>

#include <QPainter>

class CQPixmapFgButtonTip : public CQToolTipIFace {
 public:
  CQPixmapFgButtonTip(CQPixmapFgButton *button) :
   button_(button), widget_(0) {
  }

 ~CQPixmapFgButtonTip() {
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
  CQPixmapFgButton *button_;
  CQPixmapColorTip *widget_;
};

CQPixmapFgButton::
CQPixmapFgButton(CQPixmap *pixmap) :
 pixmap_(pixmap), size_(16)
{
  setFixedSize(size_, size_);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  CQToolTip::setToolTip(this, new CQPixmapFgButtonTip(this));
}

CQPixmapFgButton::
~CQPixmapFgButton()
{
  CQToolTip::unsetToolTip(this);
}

QColor
CQPixmapFgButton::
getColor() const
{
  if (pixmap_->getImage().isColorMap()) {
    int color_num = pixmap_->getFgColorNum();

    return pixmap_->getImage().color(color_num);
  }
  else
    return pixmap_->getFgColor();
}

void
CQPixmapFgButton::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QColor color = getColor();

  Qt::BrushStyle style;

  if (pixmap_->getImage().isColorMap()) {
    int color_num = pixmap_->getFgColorNum();

    if (color_num < 0 || color_num == pixmap_->getTransparentColorNum()) {
      color = QColor(0,0,0);
      style = Qt::Dense6Pattern;
    }
    else
      style = Qt::SolidPattern;
  }
  else
    style = Qt::SolidPattern;

  QBrush brush;

  brush.setColor(color);
  brush.setStyle(style);

  painter.fillRect(0, 0, size_, size_, brush);

  if (pixmap_->isFgActive()) {
    QColor color = CQPixmapImage::toBW(CQPixmapImage::inverse(color));

    painter.setPen(color);

    painter.drawRect(0, 0, size_ - 1, size_ - 1);
  }
}

void
CQPixmapFgButton::
mousePressEvent(QMouseEvent *)
{
  pixmap_->setFgActive(true);
}
