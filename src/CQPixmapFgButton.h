#ifndef CQPixmapFgButton_H
#define CQPixmapFgButton_H

#include <QPushButton>

class CQPixmap;

class CQPixmapFgButton : public QPushButton {
  Q_OBJECT

 public:
  CQPixmapFgButton(CQPixmap *pixmap);
 ~CQPixmapFgButton();

  void getColor(QColor &c, int &ind) const;

 private:
  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *) override;

 private:
  CQPixmap *pixmap_;
  int       size_;
};

#endif
