#ifndef CQPixmapColorTip_H
#define CQPixmapColorTip_H

#include <CQPixmapTipSwatch.h>
#include <QLabel>
#include <QVBoxLayout>

class CQPixmapColorTip : public QWidget {
 public:
  CQPixmapColorTip() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0); layout->setSpacing(0);

    swatch_ = new CQPixmapTipSwatch;
    label_  = new QLabel;

    //label_->setFont(QToolTip::font());
    label_->setFont(QFont("Courier", 8));

    layout->addWidget(swatch_);
    layout->addWidget(label_ );
  }

  void setColor(const QColor &c, int i) {
    auto astr = QString::asprintf("%02X", c.alpha());
    auto rstr = QString::asprintf("%02X", c.red  ());
    auto gstr = QString::asprintf("%02X", c.green());
    auto bstr = QString::asprintf("%02X", c.blue ());

    if (i >= 0)
      label_->setText(QString("%1 (#%2%3%4%5)").arg(i).arg(astr).arg(rstr).arg(gstr).arg(bstr));
    else
      label_->setText(QString("#%1%2%3%4").arg(astr).arg(rstr).arg(gstr).arg(bstr));

    swatch_->setColor(c);
  }

 private:
  CQPixmapTipSwatch *swatch_;
  QLabel            *label_;
};

#endif
