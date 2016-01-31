#ifndef CQPixmapAddColorDialog_H
#define CQPixmapAddColorDialog_H

#include <QDialog>

#if 0
class CQColorChooser;
#else
class CQColorSelector;
#endif

class CQPixmapAddColorDialog : public QDialog {
  Q_OBJECT

 public:
  CQPixmapAddColorDialog(QWidget *parent=NULL);

 private slots:
  void okSlot();
  void applySlot();

 signals:
#if 0
  void addColor(const QString &colorName);
#else
  void addColor(const QColor &c);
#endif

 private:
#if 0
  CQColorChooser *color_edit_;
#else
  CQColorSelector *selector_;
#endif
};

#endif
