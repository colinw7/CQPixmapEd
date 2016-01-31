#ifndef CQPixmapColorButton_H
#define CQPixmapColorButton_H

#include <QPushButton>

class CQPixmap;

class CQPixmapColorButton : public QPushButton {
  Q_OBJECT

 public:
  CQPixmapColorButton(CQPixmap *pixmap, int color_num);
 ~CQPixmapColorButton();

  void getColor(QColor &c, int &ind) const;

 private:
  void paintEvent(QPaintEvent *);

  void mousePressEvent(QMouseEvent *);

 private:
  CQPixmap *pixmap_;
  int       color_num_;
  int       size_;
};

#endif
