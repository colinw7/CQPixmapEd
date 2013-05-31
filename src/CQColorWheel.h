#ifndef CQCOLOR_WHEEL_H
#define CQCOLOR_WHEEL_H

#include <QWidget>

class CQColorWheelCanvas;
class CQColorWheelArea;
class QSlider;
class QSpinBox;

class CQColorWheel : public QWidget {
  Q_OBJECT

 public:
  CQColorWheel(QWidget *parent=0, Qt::Orientation orient=Qt::Vertical,
               int size=64, bool show_swatches=true);
 ~CQColorWheel();

  const QColor &getBgRGB() const;
  const QColor &getFgRGB() const;

  const QColor &getRGB() const;

  void setBgRGB(const QColor &rgb);
  void setFgRGB(const QColor &rgb);

  void setRGB(const QColor &rgb);

  void setHSV(const QColor &hsv);

  const QColor &getHSV() const;

 private:
  void updateRGB();

 private slots:
  void updateRGBFromSlider();
  void updateRGBFromSpin();

  void updateHSVFromSlider();
  void updateHSVFromSpin();

  void updateBase(const QString &str);

 signals:
  void colorBgChanged();
  void colorFgChanged();

 private:
  CQColorWheelCanvas *canvas_;
  CQColorWheelArea   *wheel_;
  QSlider            *redSlider;
  QSlider            *greenSlider;
  QSlider            *blueSlider;
  QSpinBox           *redValue;
  QSpinBox           *greenValue;
  QSpinBox           *blueValue;
  QSlider            *hueSlider;
  QSlider            *satSlider;
  QSlider            *valueSlider;
  QSpinBox           *hueValue;
  QSpinBox           *satValue;
  QSpinBox           *valueValue;
};

#endif
