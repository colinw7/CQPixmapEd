#include <CQThumbnailCanvas.h>
#include <CQPixmapEd.h>

#include <QPainter>

CQThumbnailCanvas::
CQThumbnailCanvas(CQPixmap *pixmap) :
 pixmap_(pixmap)
{
  setMinimumSize(64, 64);
  setMaximumSize(64, 64);
}

void
CQThumbnailCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  const CQPixmapImage &image = pixmap_->getImage();

  QColor bg;

  if (image.isColorMap())
    bg = image.color(pixmap_->getBgColorNum());
  else
    bg = pixmap_->getBgColor();

  bool transparent = CQPixmapImage::isTransparentColor(bg);

  QRect rect = painter.viewport();

  int canvas_size = rect.width ();

  int width  = image.width ();
  int height = image.height();

  int size = std::max(width, height);

  if (size < canvas_size)
    canvas_size = size;

  if (transparent)
    painter.fillRect(0, 0, canvas_size, canvas_size, QBrush(Qt::Dense6Pattern));

  double s = double(size - 1)/(canvas_size - 1);

  for (int j = 0; j < canvas_size; ++j) {
    int y = int(j*s);

    if (y >= height) continue;

    for (int i = 0; i < canvas_size; ++i) {
      int x = int(i*s);

      if (x >= width) continue;

      QColor color;

      if (! transparent && image.isTransparent(x, y))
        color = bg;
      else
        color = image.colorPixel(x, y);

      painter.setPen(color);

      painter.drawPoint(i, j);
    }
  }
}
