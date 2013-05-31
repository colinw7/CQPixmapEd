#ifndef CQThumbnailCanvas_H
#define CQThumbnailCanvas_H

#include <QWidget>

class CQPixmap;

class CQThumbnailCanvas : public QWidget {
 public:
  CQThumbnailCanvas(CQPixmap *pixmap);

 private:
  void paintEvent(QPaintEvent *);

 private:
  CQPixmap *pixmap_;
};

#endif
