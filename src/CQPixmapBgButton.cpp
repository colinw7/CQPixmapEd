#include <CQPixmapBgButton.h>
#include <CQPixmapEd.h>
#include <CQPixmapColorTip.h>
#include <CQToolTip.h>

#include <QPainter>

class CQPixmapBgButtonTip : public CQToolTipIFace {
 public:
  CQPixmapBgButtonTip(CQPixmapBgButton *button) :
   button_(button), tip_(0) {
  }

 ~CQPixmapBgButtonTip() {
    delete tip_;
  }

  QWidget *showWidget() {
    if (! tip_)
      tip_ = new CQPixmapColorTip;

    updateWidget();

    return tip_;
  }

  void hideWidget() {
    delete tip_;

    tip_ = 0;
  }

  //bool trackMouse() const { return true; }

  void updateWidget() {
    if (! tip_) return;

    QColor c = button_->getColor();

    tip_->setColor(c);
  }

 private:
  CQPixmapBgButton *button_;
  CQPixmapColorTip *tip_;
};

CQPixmapBgButton::
CQPixmapBgButton(CQPixmap *pixmap) :
 pixmap_(pixmap), size_(16)
{
  setFixedSize(size_, size_);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  CQToolTip::setToolTip(this, new CQPixmapBgButtonTip(this));
}

CQPixmapBgButton::
~CQPixmapBgButton()
{
  CQToolTip::unsetToolTip(this);
}

QColor
CQPixmapBgButton::
getColor() const
{
  if (pixmap_->isColorMap()) {
    int color_num = pixmap_->getBgColorNum();

    return pixmap_->getImage().color(color_num);
  }
  else
    return pixmap_->getBgColor();
}

void
CQPixmapBgButton::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QColor color = getColor();

  Qt::BrushStyle style;

  if (pixmap_->isColorMap()) {
    int color_num = pixmap_->getBgColorNum();

    if (color_num < 0)
      color_num = 0;

    if (color_num == pixmap_->getTransparentColorNum()) {
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

  if (pixmap_->isBgActive()) {
    QColor color1 = CQPixmapImage::toBW(CQPixmapImage::inverse(color));

    painter.setPen(color1);

    painter.drawRect(0, 0, size_ - 1, size_ - 1);
  }
}

void
CQPixmapBgButton::
mousePressEvent(QMouseEvent *)
{
  pixmap_->setBgActive(true);
}
