#ifndef CQColorChooser_H
#define CQColorChooser_H

#include <QWidget>
#include <QColor>

class QLineEdit;
class QPushButton;
class QToolButton;
class QLabel;
class CQAlphaButton;

class CQColorChooser : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QColor  color     READ color     WRITE setColor    )
  Q_PROPERTY(QString colorName READ colorName WRITE setColorName)

 public:
  CQColorChooser(QWidget *parent=0);

  bool getEditable() const { return editable_; }
  void setEditable(bool editable=true);

  const QColor &color() const;
  void setColor(const QColor &color, double alpha=1.0);

  void setRGBA(const QColor &rgba);
  QColor getRGBA() const;

  QString colorName() const;
  void setColorName(const QString &colorName, double alpha=1.0);

  double getAlpha() const;
  void setAlpha(double alpha);

  QLineEdit *getTextWidget() const { return cedit_; }

 private:
  void init();
  void update();
  void changeColor();
  void applyColor();

 private slots:
  void textColorChanged();

 signals:
  void colorChanged(const QColor &color);
  void colorChanged(const QString &colorName);

 private:
  bool       editable_;
  QColor     color_;
  QString    colorName_;
  double     alpha_;
  QLineEdit *cedit_;   // text color name
};

#endif
