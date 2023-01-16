#ifndef CQPixmapCanvas_H
#define CQPixmapCanvas_H

#include <QWidget>

class CQPixmap;

class CQPixmapCanvas : public QWidget {
 public:
  CQPixmapCanvas(CQPixmap *pixmap);

  void getColor(const QPoint &p, QColor &c, int &ind) const;

 private:
  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void keyPressEvent(QKeyEvent *) override;

 private:
  CQPixmap        *pixmap_;
  Qt::MouseButton  button_;
  int              pressed_;
  int              x_, y_;
};

#endif
