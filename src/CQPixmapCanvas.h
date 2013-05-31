#ifndef CQPixmapCanvas_H
#define CQPixmapCanvas_H

#include <QWidget>

class CQPixmap;

class CQPixmapCanvas : public QWidget {
 public:
  CQPixmapCanvas(CQPixmap *pixmap);

  QColor getColor(const QPoint &p) const;

 private:
  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

  void keyPressEvent(QKeyEvent *);

 private:
  CQPixmap        *pixmap_;
  Qt::MouseButton  button_;
  int              pressed_;
  int              x_, y_;
};

#endif
