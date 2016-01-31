#include <CQThumbnailCanvas.h>
#include <CQPixmapEd.h>
#include <CQUtil.h>

#include <QPainter>

namespace {

void paintCheckerboard(QPainter *p, int px, int py, int pw, int ph, int s) {
  int h = ph/s; h += (ph % s == 0 ? 0 : 1);
  int w = pw/s; w += (pw % s == 0 ? 0 : 1);

  for (int y = 0; y < h; ++y) {
    int py1 = py + s*y;

    for (int x = 0; x < w; ++x) {
      int px1 = px + s*x;

      QColor c = (((x + y) & 1) ? QColor(102,102,102) : QColor(153,153,153));

      int px2 = std::min(px1 + s - 1, pw - 1);
      int py2 = std::min(py1 + s - 1, ph - 1);

      p->fillRect(QRect(px1, py1, px2 - px1 + 1, py2 - py1 + 1), QBrush(c));
    }
  }
}

}

//------

CQThumbnailCanvas::
CQThumbnailCanvas(CQPixmap *pixmap) :
 pixmap_(pixmap), bgMode_(CHECKERBOARD)
{
  setObjectName("thumbnail");

  setFixedSize(64, 64);
}

void
CQThumbnailCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  CImagePtr image = pixmap_->getImage();

  QRect rect = painter.viewport();

  int canvas_size = rect.width();

  int width  = image->getWidth ();
  int height = image->getHeight();

  int size = std::max(width, height);

  if (size < canvas_size)
    canvas_size = size;

  double s = double(size - 1)/(canvas_size - 1);

  int width1  = width /s;
  int height1 = height/s;

  if (bgMode_ == CHECKERBOARD) {
    paintCheckerboard(&painter, 0, 0, width1, height1, 7);
  }
  else {
    QColor color;

    if (pixmap_->isColorMap()) {
      CRGBA rgba;

      image->getColor(pixmap_->getBgColorNum(), rgba);

      color = CQUtil::rgbaToColor(rgba);
    }
    else
      color = pixmap_->getBgColor();

    painter.fillRect(QRect(0, 0, width1, height1), QBrush(color));
  }

  for (int j = 0; j < canvas_size; ++j) {
    int y = int(j*s);

    if (y >= height) continue;

    for (int i = 0; i < canvas_size; ++i) {
      int x = int(i*s);

      if (x >= width) continue;

      QColor color;

      if (image->isTransparent(x, y))
        continue;

      CRGBA rgba;

      image->getRGBAPixel(x, y, rgba);

      color = CQUtil::rgbaToColor(rgba);

      painter.setPen(color);

      painter.drawPoint(i, j);
    }
  }
}
