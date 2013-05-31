#ifndef CQPixmapImage_H
#define CQPixmapImage_H

#include <QColor>
#include <QImage>

class CQPixmapImage {
 public:
  CQPixmapImage();

  QImage image() const { return image_; }

  QImage::Format format() const { return image_.format(); }

  void setImage(const QImage &image);

  void resetWindow();

  void setWindow(int l, int b, int r, int t);
  void setWindow(const QRect &window);

  void getWindow(QRect &window) const;
  void getWindow(int *l, int *b, int *r, int *t) const;

  int width () const;
  int height() const;

  bool isColorMap() const;

  int colorCount() const;

  bool valid(int x, int y) const;

  int  pixelIndex(int x, int y) const;

  QColor colorPixel(int x, int y) const;

  static QColor colorPixel(const QImage &image, int x, int y);

  QColor color(int i) const;

  static QColor color(const QImage &image, int i);

  void setPixel(int x, int y, int colorNum);
  void setPixel(int x, int y, const QColor &c);

  static void setPixel(QImage &image, int x, int y, int colorNum);
  static void setPixel(QImage &image, int x, int y, const QColor &c);

  int findColor(const QColor &c) const;

  static int findColor(const QImage &image, const QColor &c);

  int addColor(const QColor &c);

  static int addColor(QImage &image, const QColor &c);

  void convertToRGB();

  static QImage convertToRGB(const QImage &image);

  void invert();

  static QColor inverse(const QColor &c);

  void twoColor(const QColor &bg, const QColor &fg);

  static void twoColor(QImage &image, const QColor &bg, const QColor &fg);

  static QColor twoColor(const QColor &c, const QColor &bg, const QColor &fg);

  static QColor toBW(const QColor &c);

  void flipH();
  void flipV();

  void scrollX(int d);
  void scrollY(int d);

  void swapPixelX(int x1, int x2, int y);
  void swapPixelY(int y1, int y2, int x);

  void fill(int colorNum);
  void fill(const QColor &color);

  void fill(int x1, int y1, int x2, int y2, int colorNum);
  void fill(int x1, int y1, int x2, int y2, const QColor &color);

  static void fill(QImage &image, int x1, int y1, int x2, int y2, int colorNum);
  static void fill(QImage &image, int x1, int y1, int x2, int y2, const QColor &color);

  void floodFill(int x, int y, int colorNum);
  void floodFill(int x, int y, const QColor &color);

  QImage rotate(double angle) const;

  QImage resize(int w, int h) const;

  void resizeNearest(QImage &new_image) const;

  QImage resizeKeepAspect(int w, int h) const;

  QImage subImage(int x, int y, int w, int h) const;

  void subCopyFrom(const QImage &dst_image, int src_x=0, int src_y=0,
                   int width=-1, int height=-1, int dest_x=0, int dest_y=0,
                   bool skip_transparent=false);

  void subCopyTo(int src_x, int src_y, int width, int height,
                 QImage &dst_image, int dest_x=0, int dest_y=0,
                 bool skip_transparent=false) const;

  static QColor blendColor(const QColor &fg, const QColor &bg);

  void convertToColorIndex();

  static QImage convertToColorIndex(const QImage &image);

  int countColors() const;

  static int countColors(const QImage &image);

  bool isTransparent(int x, int y) const;
  bool isTransparentColor(int i) const;

  static bool isTransparent(const QImage &image, int x, int y);
  static bool isTransparentColor(const QImage &image, int i);

  static bool isTransparentColor(const QColor &c);

  static QImage getStringImage(const QFont &font, const QString &str);

 private:
  QRgb pixel(int x, int y) const;

  static void subCopy(const QImage &src_image, int src_x, int src_y, int width, int height,
                      QImage &dst_image, int dst_x, int dst_y, bool skip_transparent);

 private:
  QImage image_;
  QRect  window_;
};

#endif
