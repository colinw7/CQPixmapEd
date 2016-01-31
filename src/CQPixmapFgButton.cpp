#include <CQPixmapFgButton.h>
#include <CQPixmapEd.h>
#include <CQPixmapColorTip.h>
#include <CQToolTip.h>
#include <CQUtil.h>

#include <QPainter>

class CQPixmapFgButtonTip : public CQToolTipIFace {
 public:
  CQPixmapFgButtonTip(CQPixmapFgButton *button) :
   button_(button), tip_(0) {
  }

 ~CQPixmapFgButtonTip() {
    delete tip_;
  }

  QWidget *showWidget(const QPoint &) {
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

  bool updateWidget() {
    if (! tip_) return false;

    QColor c;
    int    ind;

    button_->getColor(c, ind);

    tip_->setColor(c, ind);

    return true;
  }

 private:
  CQPixmapFgButton *button_;
  CQPixmapColorTip *tip_;
};

//------

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

void
CQPixmapFgButton::
getColor(QColor &c, int &ind) const
{
  ind = -1;

  if (pixmap_->getImage()->hasColormap()) {
    ind = pixmap_->getFgColorNum();

    c = CQUtil::rgbaToColor(pixmap_->getImage()->getColor(ind));
  }
  else
    c = pixmap_->getFgColor();
}

void
CQPixmapFgButton::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QColor         color;
  Qt::BrushStyle style;

  if (pixmap_->isColorMap()) {
    int color_num = pixmap_->getFgColorNum();

    if (color_num < 0 || color_num == pixmap_->getTransparentColorNum()) {
      color = QColor(0,0,0);
      style = Qt::Dense6Pattern;
    }
    else {
      color = CQUtil::rgbaToColor(pixmap_->getImage()->getColor(color_num));
      style = Qt::SolidPattern;
    }
  }
  else {
    color = pixmap_->getFgColor();
    style = Qt::SolidPattern;
  }

  QBrush brush;

  brush.setColor(color);
  brush.setStyle(style);

  painter.fillRect(0, 0, size_, size_, brush);

  if (pixmap_->isFgActive()) {
    CRGBA rgba = CQUtil::colorToRGBA(color);

    QColor color1 = CQUtil::rgbaToColor(rgba.inverse().toBW());

    painter.setPen(color1);

    painter.drawRect(0, 0, size_ - 1, size_ - 1);
  }
}

void
CQPixmapFgButton::
mousePressEvent(QMouseEvent *)
{
  pixmap_->setFgActive(true);
}
