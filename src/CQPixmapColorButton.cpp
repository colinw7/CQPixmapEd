#include <CQPixmapColorButton.h>
#include <CQPixmapEd.h>
#include <CQPixmapColorTip.h>
#include <CQToolTip.h>
#include <CQUtil.h>
#include <CQUtilRGBA.h>

#include <QPainter>
#include <QMouseEvent>

class CQPixmapColorButtonTip : public CQToolTipIFace {
 public:
  CQPixmapColorButtonTip(CQPixmapColorButton *button) :
   button_(button), tip_(nullptr) {
  }

 ~CQPixmapColorButtonTip() {
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

    tip_ = nullptr;
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
  CQPixmapColorButton *button_;
  CQPixmapColorTip    *tip_;
};

//------

CQPixmapColorButton::
CQPixmapColorButton(CQPixmap *pixmap, int color_num) :
 pixmap_(pixmap), color_num_(color_num), size_(16)
{
  setObjectName(QString("color_%1").arg(color_num));

  size_ = style()->pixelMetric(QStyle::PM_SmallIconSize);

  setFixedSize(size_, size_);

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  CQToolTip::setToolTip(this, new CQPixmapColorButtonTip(this));
}

CQPixmapColorButton::
~CQPixmapColorButton()
{
  CQToolTip::unsetToolTip(this);
}

void
CQPixmapColorButton::
getColor(QColor &c, int &ind) const
{
  ind = color_num_;

  c = CQUtil::rgbaToColor(pixmap_->getImage()->getColor(ind));
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
    color = CQUtil::rgbaToColor(pixmap_->getImage()->getColor(color_num_));
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
