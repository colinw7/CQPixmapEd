#ifndef CQPixmapRescaleDialog_H
#define CQPixmapRescaleDialog_H

#include <QDialog>

class QCheckBox;
class QLineEdit;

class CQPixmapRescaleDialog : public QDialog {
  Q_OBJECT

 public:
  CQPixmapRescaleDialog(QWidget *parent=NULL);

  void init(int width, int height, bool keep_aspect);

 private slots:
  void accept();

 signals:
  void rescale(int width, int height, bool keep_aspect);

 private:
  QLineEdit *width_edit_;
  QLineEdit *height_edit_;
  QCheckBox *aspect_;
};

#endif
