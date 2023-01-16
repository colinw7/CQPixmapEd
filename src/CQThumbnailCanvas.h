#ifndef CQThumbnailCanvas_H
#define CQThumbnailCanvas_H

#include <QWidget>

class CQPixmap;

class CQThumbnailCanvas : public QWidget {
 public:
  enum BgMode {
    BGCOLOR,
    CHECKERBOARD
  };

 public:
  CQThumbnailCanvas(CQPixmap *pixmap);

 private:
  void paintEvent(QPaintEvent *) override;

 private:
  CQPixmap *pixmap_;
  BgMode    bgMode_;
};

#endif
