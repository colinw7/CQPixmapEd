#ifndef CQPixmapAddColorDialog_H
#define CQPixmapAddColorDialog_H

#include <QDialog>

class CQColorChooser;

class CQPixmapAddColorDialog : public QDialog {
  Q_OBJECT

 public:
  CQPixmapAddColorDialog(QWidget *parent=NULL);

 private slots:
  void okSlot();
  void applySlot();

 signals:
  void addColor(const QString &colorName);

 private:
  CQColorChooser *color_edit_;
};

#endif
