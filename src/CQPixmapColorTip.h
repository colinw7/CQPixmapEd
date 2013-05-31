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

  void setColor(const QColor &c) {
    QString astr; astr.sprintf("%02X", c.alpha());
    QString rstr; rstr.sprintf("%02X", c.red  ());
    QString gstr; gstr.sprintf("%02X", c.green());
    QString bstr; bstr.sprintf("%02X", c.blue ());

    label_->setText(QString("%1%2%3%4").arg(astr).arg(rstr).arg(gstr).arg(bstr));

    swatch_->setColor(c);
  }

 private:
  CQPixmapTipSwatch *swatch_;
  QLabel            *label_;
};

#endif
