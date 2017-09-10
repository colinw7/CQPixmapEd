#include <CQPixmapBgButton.h>
#include <CQPixmapEd.h>
#include <CQPixmapColorTip.h>
#include <CQToolTip.h>
#include <CQUtil.h>

#include <QPainter>

class CQPixmapBgButtonTip : public CQToolTipIFace {
 public:
  CQPixmapBgButtonTip(CQPixmapBgButton *button) :
   button_(button), tip_(0) {
  }

 ~CQPixmapBgButtonTip() {
    delete tip_;
  }

  QWidget *showWidget(const QPoint &p) override {
    if (! tip_)
      tip_ = new CQPixmapColorTip;

    updateWidget(p);

    return tip_;
  }

  void hideWidget() override {
    delete tip_;

    tip_ = 0;
  }

  //bool trackMouse() const { return true; }

  bool updateWidget(const QPoint &) override {
    if (! tip_) return false;

    QColor c;
    int    ind;

    button_->getColor(c, ind);

    tip_->setColor(c, ind);

    return true;
  }

 private:
  CQPixmapBgButton *button_;
  CQPixmapColorTip *tip_;
};

//------

CQPixmapBgButton::
CQPixmapBgButton(CQPixmap *pixmap) :
 pixmap_(pixmap), size_(16)
{
  setObjectName("bg");

  size_ = style()->pixelMetric(QStyle::PM_SmallIconSize);

  setFixedSize(size_, size_);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  CQToolTip::setToolTip(this, new CQPixmapBgButtonTip(this));
}

CQPixmapBgButton::
~CQPixmapBgButton()
{
  CQToolTip::unsetToolTip(this);
}

void
CQPixmapBgButton::
getColor(QColor &c, int &ind) const
{
  ind = -1;

  if (pixmap_->getImage()->hasColormap()) {
    ind = pixmap_->getBgColorNum();

    c = CQUtil::rgbaToColor(pixmap_->getImage()->getColor(ind));
  }
  else
    c = pixmap_->getBgColor();
}

void
CQPixmapBgButton::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QColor         color;
  Qt::BrushStyle style;

  if (pixmap_->isColorMap()) {
    int color_num = pixmap_->getBgColorNum();

    if (color_num < 0)
      color_num = 0;

    if (color_num == pixmap_->getTransparentColorNum()) {
      color = QColor(0,0,0);
      style = Qt::Dense6Pattern;
    }
    else {
      color = CQUtil::rgbaToColor(pixmap_->getImage()->getColor(color_num));
      style = Qt::SolidPattern;
    }
  }
  else {
    color = pixmap_->getBgColor();
    style = Qt::SolidPattern;
  }

  QBrush brush;

  brush.setColor(color);
  brush.setStyle(style);

  painter.fillRect(0, 0, size_, size_, brush);

  if (pixmap_->isBgActive()) {
    CRGBA rgba = CQUtil::colorToRGBA(color);

    QColor color1 = CQUtil::rgbaToColor(rgba.inverse().toBW());

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
