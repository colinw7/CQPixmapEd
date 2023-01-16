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
  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *) override;

 private:
  CQPixmap *pixmap_;
  int       size_;
};

#endif
