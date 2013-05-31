#include <CQPixmapImage.h>

#include <QFontMetrics>
#include <QPainter>

#include <cassert>
#include <cmath>
#include <map>
#include <set>

#define RAD_TO_DEG(r) ((180.0*(r))/M_PI)
#define DEG_TO_RAD(d) ((M_PI*(d))/180.0)

CQPixmapImage::
CQPixmapImage()
{
}

void
CQPixmapImage::
setImage(const QImage &image)
{
  image_ = image;
}

int
CQPixmapImage::
width() const
{
  return image_.width();
}

int
CQPixmapImage::
height() const
{
  return image_.height();
}

bool
CQPixmapImage::
valid(int x, int y) const
{
  return image_.valid(x, y);
}

bool
CQPixmapImage::
isColorMap() const
{
  return (image_.format() == QImage::Format_Indexed8);
}

int
CQPixmapImage::
colorCount() const
{
  return image_.colorCount();
}

QRgb
CQPixmapImage::
pixel(int x, int y) const
{
  return image_.pixel(x, y);
}

QColor
CQPixmapImage::
colorPixel(int x, int y) const
{
  return colorPixel(image_, x, y);
}

QColor
CQPixmapImage::
colorPixel(const QImage &image, int x, int y)
{
  QRgb rgba = image.pixel(x, y);

  return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
}

int
CQPixmapImage::
pixelIndex(int x, int y) const
{
  return image_.pixelIndex(x, y);
}

QColor
CQPixmapImage::
color(int i) const
{
  return color(image_, i);
}

QColor
CQPixmapImage::
color(const QImage &image, int i)
{
  QRgb rgba = image.color(i);

  return QColor(qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba));
}

void
CQPixmapImage::
setPixel(int x, int y, int colorNum)
{
  setPixel(image_, x, y, colorNum);
}

void
CQPixmapImage::
setPixel(QImage &image, int x, int y, int colorNum)
{
  image.setPixel(x, y, colorNum);
}

void
CQPixmapImage::
setPixel(int x, int y, const QColor &c)
{
  setPixel(image_, x, y, c);
}

void
CQPixmapImage::
setPixel(QImage &image, int x, int y, const QColor &c)
{
  image.setPixel(x, y, c.rgba());
}

int
CQPixmapImage::
findColor(const QColor &c) const
{
  return findColor(image_, c);
}

int
CQPixmapImage::
findColor(const QImage &image, const QColor &c)
{
  QVector<QRgb> colors = image.colorTable();

  int n = colors.size();

  for (int i = 0; i < n; ++i)
    if (colors[i] == c.rgba())
      return i;

  return -1;
}

int
CQPixmapImage::
addColor(const QColor &c)
{
  return addColor(image_, c);
}

int
CQPixmapImage::
addColor(QImage &image, const QColor &c)
{
  int ind;

  if ((ind = findColor(image, c)) >= 0)
    return ind;

  QVector<QRgb> colors = image.colorTable();

  if (colors.size() >= 256)
    return -1;

  colors.push_back(c.rgba());

  image.setColorTable(colors);

  return colors.size() - 1;
}

void
CQPixmapImage::
convertToRGB()
{
  image_ = convertToRGB(image_);
}

QImage
CQPixmapImage::
convertToRGB(const QImage &image)
{
  return image.convertToFormat(QImage::Format_ARGB32);
}

void
CQPixmapImage::
resetWindow()
{
  window_ = QRect();
}

void
CQPixmapImage::
setWindow(int l, int b, int r, int t)
{
  setWindow(QRect(l, b, r - l + 1, t - b + 1));
}

void
CQPixmapImage::
setWindow(const QRect &window)
{
  window_ = window;
}

void
CQPixmapImage::
getWindow(QRect &window) const
{
  window = window_;
}

void
CQPixmapImage::
getWindow(int *l, int *b, int *r, int *t) const
{
  if (window_.isNull()) {
    *l = 0;
    *b = 0;
    *r = width () - 1;
    *t = height() - 1;
  }
  else {
    *l = std::min(window_.left  (), window_.right ());
    *b = std::min(window_.bottom(), window_.top   ());
    *r = std::max(window_.left  (), window_.right ());
    *t = std::max(window_.bottom(), window_.top   ());
  }
}

void
CQPixmapImage::
invert()
{
  int left, bottom, right, top;

  getWindow(&left, &bottom, &right, &top);

  // process non-colormap image (pixels are colored)
  if      (image_.format() == QImage::Format_ARGB32) {
    for (int y = bottom; y <= top; ++y) {
      for (int x = left; x <= right; ++x) {
        QColor c = colorPixel(x, y);

        if (isTransparentColor(c)) continue;

        c = inverse(c);

        setPixel(x, y, c);
      }
    }
  }
  // process colormap image (pixels are index into colormap)
  else if (image_.format() == QImage::Format_Indexed8) {
    for (int y = bottom; y <= top; ++y) {
      for (int x = left; x <= right; ++x) {
        QColor c = color(pixelIndex(x, y));

        if (isTransparentColor(c)) continue;

        c = inverse(c);

        int ind = addColor(c);

        if (ind >= 0)
          setPixel(x, y, ind);
      }
    }
  }
  else {
    // shoud never get here
    assert(false);
  }
}

QColor
CQPixmapImage::
inverse(const QColor &c)
{
  return QColor(255 - c.red(), 255 - c.green(), 255 - c.blue(), c.alpha());
}

void
CQPixmapImage::
twoColor(const QColor &bg, const QColor &fg)
{
  twoColor(image_, bg, fg);
}

void
CQPixmapImage::
twoColor(QImage &image, const QColor &bg, const QColor &fg)
{
  // process non-colormap image (pixels are colored)
  if      (image.format() == QImage::Format_ARGB32) {
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        QColor c = colorPixel(image, x, y);

        if (! isTransparentColor(c))
          setPixel(image, x, y, twoColor(c, bg, fg));
      }
    }
  }
  // process colormap image (pixels are index into colormap)
  else if (image.format() == QImage::Format_Indexed8) {
    int ncolors = image.colorCount();

    for (int i = 0; i < ncolors; ++i) {
      QColor c = color(image, i);

      if (! isTransparentColor(c))
        image.setColor(i, twoColor(c, bg, fg).rgba());
    }
  }
  else {
    // shoud never get here
    assert(false);
  }
}

QColor
CQPixmapImage::
twoColor(const QColor &c, const QColor &bg, const QColor &fg)
{
  int g = qGray(c.red(), c.green(), c.blue());

  if (g < 128)
    return fg;
  else
    return bg;
}

QColor
CQPixmapImage::
toBW(const QColor &c)
{
  return twoColor(c, QColor(0,0,0), QColor(255,255,255));
}

void
CQPixmapImage::
flipH()
{
  int ym = height()/2;

  for (int y = 0; y < ym; ++y) {
    int y1 = height() - 1 - y;

    for (int x = 0; x < width(); ++x) {
      // process non-colormap image (pixels are colored)
      if      (image_.format() == QImage::Format_ARGB32) {
        QColor c1 = colorPixel(x, y );
        QColor c2 = colorPixel(x, y1);

        setPixel(x, y , c2);
        setPixel(x, y1, c1);
      }
      // process colormap image (pixels are index into colormap)
      else if (image_.format() == QImage::Format_Indexed8) {
        int i1 = image_.pixelIndex(x, y );
        int i2 = image_.pixelIndex(x, y1);

        image_.setPixel(x, y , i2);
        image_.setPixel(x, y1, i1);
      }
    }
  }
}

void
CQPixmapImage::
flipV()
{
  int xm = width()/2;

  for (int x = 0; x < xm; ++x) {
    int x1 = width() - 1 - x;

    for (int y = 0; y < height(); ++y) {
      // process non-colormap image (pixels are colored)
      if      (image_.format() == QImage::Format_ARGB32) {
        QColor c1 = colorPixel(x , y);
        QColor c2 = colorPixel(x1, y);

        setPixel(x , y, c2);
        setPixel(x1, y, c1);
      }
      // process colormap image (pixels are index into colormap)
      else if (image_.format() == QImage::Format_Indexed8) {
        int i1 = image_.pixelIndex(x , y);
        int i2 = image_.pixelIndex(x1, y);

        setPixel(x , y, i2);
        setPixel(x1, y, i1);
      }
    }
  }
}

void
CQPixmapImage::
scrollX(int d)
{
  if (d < 0) {
    d = abs(d) % width();

    d = width() - d;
  }
  else
    d %= width();

  for (int y = 0; y < height(); ++y) {
    swapPixelX(0, width() - 1, y);
    swapPixelX(0, d - 1      , y);
    swapPixelX(d, width() - 1, y);
  }
}

void
CQPixmapImage::
scrollY(int d)
{
  if (d < 0) {
    d = abs(d) % height();

    d = height() - d;
  }
  else
    d %= height();

  for (int x = 0; x < width(); ++x) {
    swapPixelY(0, height() - 1, x);
    swapPixelY(0, d - 1       , x);
    swapPixelY(d, height() - 1, x);
  }
}

void
CQPixmapImage::
swapPixelX(int x1, int x2, int y)
{
  int n = (x2 - x1 + 1)/2;

  for (int i = 0; i < n; ++i) {
    // process non-colormap image (pixels are colored)
    if      (image_.format() == QImage::Format_ARGB32) {
      QColor c1 = colorPixel(x1 + i, y);
      QColor c2 = colorPixel(x2 - i, y);

      setPixel(x1 + i, y, c2);
      setPixel(x2 - i, y, c1);
    }
    // process colormap image (pixels are index into colormap)
    else if (image_.format() == QImage::Format_Indexed8) {
      int c1 = pixelIndex(x1 + i, y);
      int c2 = pixelIndex(x2 - i, y);

      setPixel(x1 + i, y, c2);
      setPixel(x2 - i, y, c1);
    }
  }
}

void
CQPixmapImage::
swapPixelY(int y1, int y2, int x)
{
  int n = (y2 - y1 + 1)/2;

  for (int i = 0; i < n; ++i) {
    // process non-colormap image (pixels are colored)
    if      (image_.format() == QImage::Format_ARGB32) {
      QColor c1 = colorPixel(x, y1 + i);
      QColor c2 = colorPixel(x, y2 - i);

      setPixel(x, y1 + i, c2);
      setPixel(x, y2 - i, c1);
    }
    // process colormap image (pixels are index into colormap)
    else if (image_.format() == QImage::Format_Indexed8) {
      int c1 = pixelIndex(x, y1 + i);
      int c2 = pixelIndex(x, y2 - i);

      setPixel(x, y1 + i, c2);
      setPixel(x, y2 - i, c1);
    }
  }
}

struct CFillSegment {
  int y, xl, xr, dy;
};

#define CIMAGE_FLOOD_FILL_MAX_DEPTH 16384

#define CIMAGE_FLOOD_FILL_PUSH(Y, XL, XR, DY) \
  if (sp < stack + CIMAGE_FLOOD_FILL_MAX_DEPTH && \
      Y + (DY) >= bottom && Y + (DY) <= top) { \
    sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++; \
  }

#define CIMAGE_FLOOD_FILL_POP(Y, XL, XR, DY) { \
  sp--; Y = sp->y; XL = sp->xl; XR = sp->xr; DY = sp->dy; \
}

void
CQPixmapImage::
fill(int colorNum)
{
  int left, bottom, right, top;

  getWindow(&left, &bottom, &right, &top);

  fill(left, bottom, right, top, colorNum);
}

void
CQPixmapImage::
fill(const QColor &c)
{
  int left, bottom, right, top;

  getWindow(&left, &bottom, &right, &top);

  fill(left, bottom, right, top, c);
}

void
CQPixmapImage::
fill(int x1, int y1, int x2, int y2, int color)
{
  fill(image_, x1, y1, x2, y2, color);
}

void
CQPixmapImage::
fill(QImage &image, int x1, int y1, int x2, int y2, int color)
{
  assert(image.format() == QImage::Format_Indexed8);

  for (int y = y1; y <= y2; ++y)
    for (int x = x1; x <= x2; ++x)
      setPixel(image, x, y, color);
}

void
CQPixmapImage::
fill(int x1, int y1, int x2, int y2, const QColor &color)
{
  fill(image_, x1, y1, x2, y2, color);
}

void
CQPixmapImage::
fill(QImage &image, int x1, int y1, int x2, int y2, const QColor &color)
{
  assert(image.format() == QImage::Format_ARGB32);

  for (int y = y1; y <= y2; ++y)
    for (int x = x1; x <= x2; ++x)
      setPixel(image, x, y, color);
}

void
CQPixmapImage::
floodFill(int x, int y, int colorNum)
{
  assert(image_.format() == QImage::Format_Indexed8);

  int colorNum1 = image_.pixelIndex(x, y);

  if (colorNum1 == colorNum)
    return;

  //-------

  int left, bottom, right, top;

  getWindow(&left, &bottom, &right, &top);

  //-------

  CFillSegment stack[CIMAGE_FLOOD_FILL_MAX_DEPTH];

  CFillSegment *sp = stack;

  CIMAGE_FLOOD_FILL_PUSH(y    , x, x,  1); // needed in some cases
  CIMAGE_FLOOD_FILL_PUSH(y + 1, x, x, -1); // seed segment (popped 1st)

  int colorNum2;
  int x1, x2, dy, l;

  while (sp > stack) {
    // pop segment off stack and fill a neighboring scan line */

    CIMAGE_FLOOD_FILL_POP(y, x1, x2, dy);

    y += dy;

    // segment of scan line y - dy for x1 <= x <= x2 was previously filled,
    // now explore adjacent pixels in scan line y

    for (x = x1; x >= left; x--) {
      colorNum2 = image_.pixelIndex(x, y);

      if (colorNum2 != colorNum1)
        break;

      setPixel(x, y, colorNum);
    }

    if (x >= x1)
      goto skip;

    l = x + 1;

    if (l < x1)
      CIMAGE_FLOOD_FILL_PUSH(y, l, x1 - 1, -dy); // leak on left?

    x = x1 + 1;

    do {
      for (; x <= right; x++) {
        colorNum2 = image_.pixelIndex(x, y);

        if (colorNum2 != colorNum1)
          break;

        setPixel(x, y, colorNum);
      }

      CIMAGE_FLOOD_FILL_PUSH(y, l, x - 1, dy);

      if (x > x2 + 1)
        CIMAGE_FLOOD_FILL_PUSH(y, x2 + 1, x - 1, -dy); // leak on right?

 skip:
      for (x++; x <= x2; x++) {
        colorNum2 = image_.pixelIndex(x, y);

        if (colorNum2 == colorNum1)
          break;
      }

      l = x;
    }
    while (x <= x2);
  }
}

void
CQPixmapImage::
floodFill(int x, int y, const QColor &color)
{
  assert(image_.format() == QImage::Format_ARGB32);

  QColor color1 = colorPixel(x, y);

  if (color == color1)
    return;

  //-------

  int left, bottom, right, top;

  getWindow(&left, &bottom, &right, &top);

  //-------

  CFillSegment stack[CIMAGE_FLOOD_FILL_MAX_DEPTH];

  CFillSegment *sp = &stack[0];

  CIMAGE_FLOOD_FILL_PUSH(y    , x, x,  1); // needed in some cases
  CIMAGE_FLOOD_FILL_PUSH(y + 1, x, x, -1); // seed segment (popped 1st)

  QColor color2;
  int    x1, x2, dy, l;

  while (sp > stack) {
    // pop segment off stack and fill a neighboring scan line */

    CIMAGE_FLOOD_FILL_POP(y, x1, x2, dy);

    y += dy;

    // segment of scan line y - dy for x1 <= x <= x2 was previously filled,
    // now explore adjacent pixels in scan line y

    for (x = x1; x >= left; x--) {
      color2 = colorPixel(x, y);

      if (color2 != color1)
        break;

      setPixel(x, y, color);
    }

    if (x >= x1)
      goto skip;

    l = x + 1;

    if (l < x1)
      CIMAGE_FLOOD_FILL_PUSH(y, l, x1 - 1, -dy); // leak on left?

    x = x1 + 1;

    do {
      for (; x <= right; x++) {
        color2 = colorPixel(x, y);

        if (color2 != color1)
          break;

        setPixel(x, y, color);
      }

      CIMAGE_FLOOD_FILL_PUSH(y, l, x - 1, dy);

      if (x > x2 + 1)
        CIMAGE_FLOOD_FILL_PUSH(y, x2 + 1, x - 1, -dy); // leak on right?

 skip:
      for (x++; x <= x2; x++) {
        color2 = colorPixel(x, y);

        if (color2 == color1)
          break;
      }

      l = x;
    }
    while (x <= x2);
  }
}

QImage
CQPixmapImage::
rotate(double angle) const
{
  while (angle < 0)
    angle += 360;

  while (angle >= 360)
    angle -= 360;

  //-----

  double c = cos(DEG_TO_RAD(angle));
  double s = sin(DEG_TO_RAD(angle));

  int left, bottom, right, top;

  getWindow(&left, &bottom, &right, &top);

  int width  = right - left + 1;
  int height = top - bottom + 1;

  int width1  = (int) (fabs(width *c) + fabs(height*s));
  int height1 = (int) (fabs(height*c) + fabs(width *s));

  QImage image1 = QImage(width1, height1, image_.format());

  //-----

  if (image_.format() == QImage::Format_Indexed8) {
    image1.setColorTable(image_.colorTable());

    //if (isTransparent(image_))
    //  setTransparentColor(image1, getTransparentColor(image_));
  }

  //-----

  double x_offset = 0;
  double y_offset = 0;

  if      (angle <= 90) {
    y_offset = (width - 1)*s;
  }
  else if (angle <= 180) {
    x_offset = -(width - 1)*c;
    y_offset = height1 - 1;
  }
  else if (angle <= 270) {
    x_offset = width1 - 1;
    y_offset = -(height - 1)*c;
  }
  else
    x_offset = -(height - 1)*s;

  if (image_.format() == QImage::Format_Indexed8) {
    for (int y = 0; y < height1; ++y) {
      for (int x = 0; x < width1; ++x) {
        int x1 = round((x - x_offset)*c - (y - y_offset)*s) + left;
        int y1 = round((x - x_offset)*s + (y - y_offset)*c) + bottom;

        x1 = std::min(std::max(x1, left  ), right);
        y1 = std::min(std::max(y1, bottom), top  );

        int ind = image_.pixelIndex(x1, y1);

        setPixel(image1, x, y, ind);
      }
    }
  }
  else {
    for (int y = 0; y < height1; ++y) {
      for (int x = 0; x < width1; ++x) {
        int x1 = round((x - x_offset)*c - (y - y_offset)*s) + left;
        int y1 = round((x - x_offset)*s + (y - y_offset)*c) + bottom;

        x1 = std::min(std::max(x1, left  ), right);
        y1 = std::min(std::max(y1, bottom), top  );

        QColor c = colorPixel(x1, y1);

        setPixel(image1, x, y, c);
      }
    }
  }

  return image1;
}

QImage
CQPixmapImage::
resize(int w, int h) const
{
  if (w <= 0 || h <= 0)
    return QImage();

  QImage image = QImage(w, h, image_.format());

  if (isColorMap())
    image.setColorTable(image_.colorTable());

  resizeNearest(image);

  return image;
}

void
CQPixmapImage::
resizeNearest(QImage &new_image) const
{
  int w1 = width ();
  int h1 = height();

  if (w1 <= 0 || h1 <= 0)
    return;

  int w2 = new_image.width ();
  int h2 = new_image.height();

  if (w2 <= 0 || h2 <= 0)
    return;

  double dx = (1.0*w1)/w2;
  double dy = (1.0*h1)/h2;

  double y1 = 0.0;

  if (! isColorMap()) {
    for (int y = 0; y < h2; ++y, y1 += dy) {
      double x1 = 0.0;

      for (int x = 0; x < w2; ++x, x1 += dx) {
        int x2 = std::min(std::max(int(x1), 0), w1 - 1);
        int y2 = std::min(std::max(int(y1), 0), h1 - 1);

        QColor c = colorPixel(x2, y2);

        setPixel(new_image, x, y, c);
      }
    }
  }
  else {
    for (int y = 0; y < h2; ++y, y1 += dy) {
      double x1 = 0.0;

      for (int x = 0; x < w2; ++x, x1 += dx) {
        int x2 = std::min(std::max(int(x1), 0), w1  - 1);
        int y2 = std::min(std::max(int(y1), 0), h1 - 1);

        int pixel = image_.pixelIndex(x2, y2);

        setPixel(new_image, x, y, pixel);
      }
    }
  }
}

QImage
CQPixmapImage::
resizeKeepAspect(int w, int h) const
{
  if (width() <= 0 || height() <= 0)
    return QImage();

  double xfactor = (1.0*w)/width ();
  double yfactor = (1.0*h)/height();

  if (xfactor < yfactor)
    h = height()*xfactor;
  else
    w = width ()*yfactor;

  return resize(w, h);
}


QImage
CQPixmapImage::
subImage(int x, int y, int w, int h) const
{
  QImage image = QImage(w, h, image_.format());

  if (isColorMap()) {
    image.setColorTable(image_.colorTable());

    for (int y1 = y; y1 < y + h; ++y1) {
      for (int x1 = x; x1 < x + w; ++x1) {
        setPixel(image, x1 - x, y1 - y, pixelIndex(x1, y1));
      }
    }
  }
  else {
    for (int y1 = y; y1 < y + h; ++y1) {
      for (int x1 = x; x1 < x + w; ++x1) {
        setPixel(image, x1 - x, y1 - y, colorPixel(x1, y1));
      }
    }
  }

  return image;
}

void
CQPixmapImage::
subCopyFrom(const QImage &src_image, int src_x, int src_y, int width, int height,
            int dst_x, int dst_y, bool skip_transparent)
{
  subCopy(src_image, src_x, src_y, width, height, image_, dst_x, dst_y, skip_transparent);
}

void
CQPixmapImage::
subCopyTo(int src_x, int src_y, int width, int height, QImage &dst_image,
          int dst_x, int dst_y, bool skip_transparent) const
{
  subCopy(image_, src_x, src_y, width, height, dst_image, dst_x, dst_y, skip_transparent);
}

void
CQPixmapImage::
subCopy(const QImage &src_image, int src_x, int src_y, int width, int height,
        QImage &dst_image, int dst_x, int dst_y, bool skip_transparent)
{
  int src_width  = src_image.width ();
  int src_height = src_image.height();

  int dst_width  = dst_image.width ();
  int dst_height = dst_image.height();

  if (width  < 0) width  = src_width;
  if (height < 0) height = src_height;

  int width1  = width;
  int height1 = height;

  if (src_x +  width1 > src_width )  width1 = src_width  - src_x;
  if (src_y + height1 > src_height) height1 = src_height - src_y;
  if (dst_x +  width1 > dst_width )  width1 = dst_width  - dst_x;
  if (dst_y + height1 > dst_height) height1 = dst_height - dst_y;

  uint xs, ys, xd, yd;

  if (src_image.format() == QImage::Format_Indexed8) {
    if (dst_image.format() != QImage::Format_Indexed8)
      dst_image = convertToColorIndex(dst_image);

    std::map<int,int> colorMap;

    int num_colors = src_image.colorCount();

    for (int i = 0; i < num_colors; ++i) {
      if (skip_transparent && isTransparentColor(src_image, i)) continue;

      int ind = addColor(dst_image, color(src_image, i));

      if (ind >= 0)
        colorMap[i] = ind;
    }

    ys = src_y; yd = dst_y;

    for (int y = 0; y < height1; ++y, ++ys, ++yd) {
      xs = src_x; xd = dst_x;

      for (int x = 0; x < width1; ++x, ++xs, ++xd) {
        if (! src_image.valid(xs, ys)) continue;
        if (! dst_image.valid(xd, yd)) continue;

        int pixel = src_image.pixelIndex(xs, ys);

        if (skip_transparent && isTransparentColor(src_image, pixel)) continue;

        setPixel(dst_image, xd, yd, colorMap[pixel]);
      }
    }
  }
  else {
    ys = src_y; yd = dst_y;

    for (int y = 0; y < height1; ++y, ++ys, ++yd) {
      xs = src_x; xd = dst_x;

      for (int x = 0; x < width1; ++x, ++xs, ++xd) {
        if (! src_image.valid(xs, ys)) continue;
        if (! dst_image.valid(xd, yd)) continue;

        QColor fg = colorPixel(src_image, xs, ys);
        QColor bg = colorPixel(dst_image, xd, yd);

        QColor c = blendColor(fg, bg);

        setPixel(dst_image, xd, yd, c);
      }
    }
  }
}

QColor
CQPixmapImage::
blendColor(const QColor &fg, const QColor &bg)
{
  if (fg.alpha() == 0) return bg;
  if (bg.alpha() == 0) return fg;

  double a1 = fg.alpha()/255.0;
  double a2 = bg.alpha()/255.0;
  double a  = std::max(a1, a2);

  a1 /= a;
  a2 /= a;

  int r = std::min(int(fg.red  ()*a1 + a2*(1 - a1)*bg.red  ()), 255);
  int g = std::min(int(fg.green()*a1 + a2*(1 - a1)*bg.green()), 255);
  int b = std::min(int(fg.blue ()*a1 + a2*(1 - a1)*bg.blue ()), 255);

  return QColor(r, g, b, 255*a);
}

void
CQPixmapImage::
convertToColorIndex()
{
  image_ = convertToColorIndex(image_);
}

QImage
CQPixmapImage::
convertToColorIndex(const QImage &image)
{
  double ralpha_tol = 0.5; // convertAlphaTol_
  int    alpha_tol  = 256*ralpha_tol;

  if (image.format() == QImage::Format_Indexed8)
    return image;

  // convert (blend) background
  double br = 1.0;
  double bg = 1.0;
  double bb = 1.0;

  bool transparent = false;

  uint num_colors;
  uint colors[256];

  uint cycle = 0;

  uint r_bits = 8;
  uint g_bits = 8;
  uint b_bits = 8;

  QImage image1 = image;

  int w = image1.width ();
  int h = image1.height();

  if (w == 0 || h == 0)
    return image;

  while (true) {
    // count number of unique colors
    num_colors = 0;

    for (int y = 0; y < h; ++y) {
      int x = 0;

      while (x < w) {
        QRgb pixel = image1.pixel(x, y);

        ++x;

        while (x < w && image1.pixel(x, y) == pixel)
          ++x;

        int a = qAlpha(pixel);

        if (a > alpha_tol) {
          uint k = 0;

          for ( ; k < num_colors; ++k)
            if (colors[k] == pixel)
              break;

          if (k >= num_colors) {
            if (num_colors >= 255)
              break;

            colors[num_colors++] = pixel;
          }
        }
        else
          transparent = true;
      }

      if (num_colors >= 255)
        break;
    }

    //----

    // we have enough colors (<= 255)
    if (num_colors <= 255)
      break;

    //----

    // reduce colors one bit at a time in r, g, b order

    uint error = 0;

    if      (cycle == 2) {
      int max = (1 << b_bits) - 1;

      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          QRgb pixel = image1.pixel(x, y);

          int ia = qAlpha(pixel);

          if (ia > alpha_tol) {
            double r = qRed  (pixel)/255.0;
            double g = qGreen(pixel)/255.0;
            double b = qBlue (pixel)/255.0;
            double a = ia/255.0;

            int r1 = int(255*(r*a + br*(1 - a)));
            int g1 = int(255*(g*a + bg*(1 - a)));
            int b1 = int(255*(b*a + bb*(1 - a)));

            if (b1 < max)
              b1 += error;

            error = b1 & 1;

            setPixel(image1, x, y, QColor(r1, g1, b1 >> 1, 255));
          }
          else
            setPixel(image1, x, y, QColor(0, 0, 0, 0));
        }
      }

      --b_bits;
    }
    else if (cycle == 1) {
      int max = (1 << g_bits) - 1;

      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          QRgb pixel = image1.pixel(x, y);

          int ia = qAlpha(pixel);

          if (ia > alpha_tol) {
            double r = qRed  (pixel)/255.0;
            double g = qGreen(pixel)/255.0;
            double b = qBlue (pixel)/255.0;
            double a = ia/255.0;

            int r1 = int(255*(r*a + br*(1 - a)));
            int g1 = int(255*(g*a + bg*(1 - a)));
            int b1 = int(255*(b*a + bb*(1 - a)));

            if (g1 < max)
              g1 += error;

            error = g1 & 1;

            setPixel(image1, x, y, QColor(r1, g1 >> 1, b1, 255));
          }
          else
            setPixel(image1, x, y, QColor(0, 0, 0, 0));
        }
      }

      --g_bits;
    }
    else {
      int max = (1 << r_bits) - 1;

      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          QRgb pixel = image1.pixel(x, y);

          int ia = qAlpha(pixel);

          if (ia > alpha_tol) {
            double r = qRed  (pixel)/255.0;
            double g = qGreen(pixel)/255.0;
            double b = qBlue (pixel)/255.0;
            double a = ia/255.0;

            int r1 = int(255*(r*a + br*(1 - a)));
            int g1 = int(255*(g*a + bg*(1 - a)));
            int b1 = int(255*(b*a + bb*(1 - a)));

            if (r1 < max)
              r1 += error;

            error = r1 & 1;

            setPixel(image1, x, y, QColor(r1 >> 1, g1, b1, 255));
          }
          else
            setPixel(image1, x, y, QColor(0, 0, 0, 0));
        }
      }

      --r_bits;
    }

    ++cycle;

    cycle %= 3;
  }

  //------

#if 0
  // set color indices in data
  for (int y = 0; y < h; ++y) {
    int x = 0;

    QRgb pixel = image1.pixel(x, y);

    while (x < w) {
      int ia = qAlpha(pixel);

      uint k = 0;

      if (ia > alpha_tol) {
        for ( ; k < num_colors; ++k)
          if (colors[k] == pixel)
            break;
      }
      else
        k = num_colors;

      setPixel(image1, x, y, k);

      ++x;

      while (x < w && image1.pixel(x, y) == pixel)
        setPixel(image1, x, y, k);
    }
  }
#endif

  // set colors
  QVector<QRgb> colorTable;

  double r_factor = 1.0/(((double) (1 << r_bits)) - 1.0);
  double g_factor = 1.0/(((double) (1 << g_bits)) - 1.0);
  double b_factor = 1.0/(((double) (1 << b_bits)) - 1.0);

  for (uint i = 0; i < num_colors; ++i) {
    double r = qRed  (colors[i])/255.0;
    double g = qGreen(colors[i])/255.0;
    double b = qBlue (colors[i])/255.0;
    double a = qAlpha(colors[i])/255.0;

    assert(a > ralpha_tol);

    int r1 = int(255*(r*a + br*(1 - a)));
    int g1 = int(255*(g*a + bg*(1 - a)));
    int b1 = int(255*(b*a + bb*(1 - a)));

    colorTable.push_back(qRgba(255*r1*r_factor, 255*g1*g_factor, 255*b1*b_factor, 255));
  }

  if (transparent)
    colorTable.push_back(qRgba(0,0,0,0));

  return image1.convertToFormat(QImage::Format_Indexed8, colorTable);
}

int
CQPixmapImage::
countColors() const
{
  return countColors(image_);
}

int
CQPixmapImage::
countColors(const QImage &image)
{
  assert(image.format() != QImage::Format_Indexed8);

  double ralpha_tol = 0.5; // convertAlphaTol_
  int    alpha_tol  = 256*ralpha_tol;

  bool transparent = false;

  std::set<QRgb> colors;

  int w = image.width ();
  int h = image.height();

  // get unique colors
  for (int y = 0; y < h; ++y) {
    int x = 0;

    while (x < w) {
      QRgb pixel = image.pixel(x, y);

      ++x;

      while (x < w && image.pixel(x, y) == pixel)
        ++x;

      int a = qAlpha(pixel);

      if (a > alpha_tol)
        colors.insert(pixel);
      else
        transparent = true;
    }
  }

  int num_colors = colors.size();

  if (transparent)
    ++num_colors;

  return num_colors;
}

bool
CQPixmapImage::
isTransparent(int x, int y) const
{
  return isTransparent(image_, x, y);
}

bool
CQPixmapImage::
isTransparent(const QImage &image, int x, int y)
{
  QRgb rgba = image.pixel(x, y);

  return (qAlpha(rgba) < 128);
}

bool
CQPixmapImage::
isTransparentColor(int i) const
{
  return isTransparentColor(image_, i);
}

bool
CQPixmapImage::
isTransparentColor(const QImage &image, int i)
{
  QRgb rgba = image.color(i);

  return (qAlpha(rgba) < 128);
}

bool
CQPixmapImage::
isTransparentColor(const QColor &c)
{
  return (qAlpha(c.rgba()) < 128);
}

//-------

QImage
CQPixmapImage::
getStringImage(const QFont &font, const QString &str)
{
  QFontMetrics fm(font);

  int w = fm.width(str);
  int h = fm.height();

  QImage image = QImage(w, h, QImage::Format_ARGB32);

  image.fill(QColor(0,0,0,0));

  QPainter painter(&image);

  painter.setPen(QColor(0,0,0));

  painter.setFont(font);

  painter.drawText(0, fm.ascent(), str);

  return image;
}
