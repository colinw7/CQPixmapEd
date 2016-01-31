#ifndef CQPixmapBgButton_H
#define CQPixmapBgButton_H

#include <QPushButton>

class CQPixmap;

class CQPixmapBgButton : public QPushButton {
  Q_OBJECT

 public:
  CQPixmapBgButton(CQPixmap *pixmap);
 ~CQPixmapBgButton();

  void getColor(QColor &c, int &ind) const;

 private:
  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);

 private:
  CQPixmap *pixmap_;
  int       size_;
};

#endif
