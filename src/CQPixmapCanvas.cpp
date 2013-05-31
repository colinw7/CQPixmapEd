#include <CQPixmapCanvas.h>
#include <CQPixmapColorTip.h>
#include <CQPixmapEd.h>
#include <CQToolTip.h>

#include <QToolTip>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>

class CQPixmapCanvasTip : public CQToolTipIFace {
 public:
  CQPixmapCanvasTip(CQPixmapCanvas *canvas) :
   canvas_(canvas), widget_(0) {
  }

 ~CQPixmapCanvasTip() {
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

    QColor c = canvas_->getColor(p);

    widget_->setColor(c);
  }

 private:
  CQPixmapCanvas   *canvas_;
  CQPixmapColorTip *widget_;
};

CQPixmapCanvas::
CQPixmapCanvas(CQPixmap *pixmap) :
 pixmap_(pixmap), pressed_(false)
{
  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);

  CQToolTip::setToolTip(this, new CQPixmapCanvasTip(this));
}

void
CQPixmapCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QPixmap *pixmap = pixmap_->getPixmap();

  painter.drawPixmap(0, 0, *pixmap);
}

void
CQPixmapCanvas::
mousePressEvent(QMouseEvent *mouseEvent)
{
  pressed_ = true;

  button_ = mouseEvent->button();

  pixmap_->pixelToWindow(mouseEvent->x(), mouseEvent->y(), &x_, &y_);

  if (button_ == Qt::MidButton) {
    if (pixmap_->getImage().valid(x_, y_)) {
      if (pixmap_->getImage().isColorMap()) {
        int ind = pixmap_->getImage().pixelIndex(x_, y_);

        if (pixmap_->isFgActive())
          pixmap_->setFgColorNum(ind);
        else
          pixmap_->setBgColorNum(ind);
      }
      else {
        QColor rgba = pixmap_->getImage().colorPixel(x_, y_);

        if (pixmap_->isFgActive())
          pixmap_->setFgColor(rgba);
        else
          pixmap_->setBgColor(rgba);
      }
    }

    return;
  }

  if (button_ != Qt::LeftButton && button_ != Qt::RightButton)
    return;

  if (pixmap_->getImage().isColorMap()) {
    if (button_ == Qt::LeftButton)
      pixmap_->setColorNum(pixmap_->getFgColorNum());
    else
      pixmap_->setColorNum(pixmap_->getBgColorNum());
  }
  else {
    if (button_ == Qt::LeftButton)
      pixmap_->setColor(pixmap_->getFgColor());
    else
      pixmap_->setColor(pixmap_->getBgColor());
  }

  CQPixmap::Function function = pixmap_->getFunction();

  if      (function == CQPixmap::FUNCTION_COPY) {
    pixmap_->xorStart(CQPixmap::XOR_RECTANGLE);

    pixmap_->setXorRectFromMark();
  }
  else if (function == CQPixmap::FUNCTION_MOVE) {
    pixmap_->xorStart(CQPixmap::XOR_RECTANGLE);

    pixmap_->setXorRectFromMark();
  }
  else if (function == CQPixmap::FUNCTION_MARK) {
    pixmap_->xorStart(CQPixmap::XOR_RECTANGLE);

    pixmap_->setXorRectStart(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_POINT) {
    pixmap_->startUndoGroup();

    pixmap_->addUndoImage(QRect(x_, y_, 1, 1));

    pixmap_->drawPoint(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_CURVE) {
    pixmap_->startUndoGroup();

    pixmap_->addUndoImage(QRect(x_, y_, 1, 1));

    pixmap_->drawPoint(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_LINE) {
    pixmap_->xorStart(CQPixmap::XOR_LINE);

    pixmap_->setXorRectStart(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_RECTANGLE) {
    pixmap_->xorStart(CQPixmap::XOR_RECTANGLE);

    pixmap_->setXorRectStart(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_FILLED_RECTANGLE) {
    pixmap_->xorStart(CQPixmap::XOR_FILLED_RECTANGLE);

    pixmap_->setXorRectStart(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_CIRCLE) {
    pixmap_->xorStart(CQPixmap::XOR_CIRCLE);

    pixmap_->setXorRectStart(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_FILLED_CIRCLE) {
    pixmap_->xorStart(CQPixmap::XOR_FILLED_CIRCLE);

    pixmap_->setXorRectStart(x_, y_);

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_FLOOD_FILL) {
    pixmap_->addUndoImage();

    pixmap_->floodFill(x_, y_);
  }
  else if (function == CQPixmap::FUNCTION_TEXT) {
    pixmap_->drawText(x_, y_);
  }
  else if (function == CQPixmap::FUNCTION_SET_HOT_SPOT) {
    pixmap_->setHotSpot(x_, y_);
  }
}

void
CQPixmapCanvas::
mouseMoveEvent(QMouseEvent *mouseEvent)
{
  int x, y;

  pixmap_->pixelToWindow(mouseEvent->x(), mouseEvent->y(), &x, &y);

  pixmap_->updatePosition(x, y);

  if (pressed_) {
    if (button_ != Qt::LeftButton && button_ != Qt::RightButton)
      return;

    if (x == x_ && y_ == y) return;

    CQPixmap::Function function = pixmap_->getFunction();

    if      (function == CQPixmap::FUNCTION_POINT) {
      x_ = x; y_ = y;

      if (x_ < 0 || x_ >= pixmap_->getImage().width() ||
          y_ < 0 || y_ >= pixmap_->getImage().height())
        return;

      pixmap_->addUndoImage(QRect(x_, y_, 1, 1));

      pixmap_->drawPoint(x_, y_);

      pixmap_->redraw();
    }
    else if (function == CQPixmap::FUNCTION_CURVE) {
      pixmap_->addUndoImage(QRect(x_, y_, x - x_ + 1, y - y_ + 1));

      pixmap_->drawLine(x, y, x_, y_);

      x_ = x; y_ = y;

      pixmap_->redraw();
    }
    else if (function == CQPixmap::FUNCTION_COPY ||
             function == CQPixmap::FUNCTION_MOVE) {
      pixmap_->moveXorRect(x - x_, y - y_);

      x_ = x; y_ = y;

      pixmap_->setChanged();

      pixmap_->redraw();
    }
    else if (function == CQPixmap::FUNCTION_MARK ||
             function == CQPixmap::FUNCTION_LINE ||
             function == CQPixmap::FUNCTION_RECTANGLE ||
             function == CQPixmap::FUNCTION_FILLED_RECTANGLE ||
             function == CQPixmap::FUNCTION_CIRCLE ||
             function == CQPixmap::FUNCTION_FILLED_CIRCLE) {
      pixmap_->setXorRectEnd(x, y);

      pixmap_->redraw();
    }
  }
}

void
CQPixmapCanvas::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;

  if (button_ != Qt::LeftButton && button_ != Qt::RightButton)
    return;

  CQPixmap::Function function = pixmap_->getFunction();

  if      (function == CQPixmap::FUNCTION_COPY) {
    pixmap_->xorEnd();

    pixmap_->addUndoImage(pixmap_->getXorRect());

    pixmap_->copyArea(pixmap_->getMark(), pixmap_->getXorRect());

    pixmap_->setRectMark();

    pixmap_->setChanged();

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_MOVE) {
    pixmap_->xorEnd();

    pixmap_->startUndoGroup();
    pixmap_->addUndoImage(pixmap_->getMark   ());
    pixmap_->addUndoImage(pixmap_->getXorRect());
    pixmap_->endUndoGroup();

    pixmap_->moveArea(pixmap_->getMark(), pixmap_->getXorRect());

    pixmap_->setRectMark();

    pixmap_->setChanged();

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_MARK) {
    pixmap_->xorEnd();

    pixmap_->setRectMark();

    pixmap_->setChanged();

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_POINT) {
    pixmap_->endUndoGroup();
  }
  else if (function == CQPixmap::FUNCTION_CURVE) {
    pixmap_->endUndoGroup();
  }
  else if (function == CQPixmap::FUNCTION_LINE) {
    pixmap_->xorEnd();

    pixmap_->addUndoImage(pixmap_->getXorRect());

    pixmap_->drawRectLine();

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_RECTANGLE) {
    pixmap_->xorEnd();

    pixmap_->addUndoImage(pixmap_->getXorRect());

    pixmap_->drawRectRect();

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_FILLED_RECTANGLE) {
    pixmap_->xorEnd();

    pixmap_->addUndoImage(pixmap_->getXorRect());

    pixmap_->fillRectRect();

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_CIRCLE) {
    pixmap_->xorEnd();

    pixmap_->drawRectCircle();

    pixmap_->redraw();
  }
  else if (function == CQPixmap::FUNCTION_FILLED_CIRCLE) {
    pixmap_->xorEnd();

    pixmap_->fillRectCircle();

    pixmap_->redraw();
  }
}

void
CQPixmapCanvas::
keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_G) {
    pixmap_->setGrid(! pixmap_->getGrid());

    pixmap_->redraw();
  }
}

QColor
CQPixmapCanvas::
getColor(const QPoint &p) const
{
  int x, y;

  if (pixmap_->pixelToWindow(p.x(), p.y(), &x, &y))
    return pixmap_->getColor(x, y);
  else
    return QColor();
}
