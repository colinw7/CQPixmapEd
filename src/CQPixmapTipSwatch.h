#ifndef CQPixmapTipSwatch_H
#define CQPixmapTipSwatch_H

#include <QPainter>

class CQPixmapTipSwatch : public QWidget {
 public:
  CQPixmapTipSwatch() {
    setFixedHeight(16);
  }

  void setColor(const QColor &c) {
    c_ = c;

    update();
  }

  void paintEvent(QPaintEvent *) {
    QPainter p(this);

    p.fillRect(rect(), QBrush(Qt::Dense6Pattern));

    p.fillRect(rect(), QBrush(c_));

    if (qGray(c_.red(), c_.green(), c_.blue()) > 128 || c_.alpha() < 128)
      p.setPen(QColor(0,0,0));
    else
      p.setPen(QColor(255,255,255));

    p.drawRect(rect().adjusted(0,0,-1,-1));
  }

 private:
  QColor c_;
};

#endif
