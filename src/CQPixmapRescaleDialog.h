#ifndef CQPixmapRescaleDialog_H
#define CQPixmapRescaleDialog_H

#include <QDialog>

class CQIntegerEdit;
class QCheckBox;

class CQPixmapRescaleDialog : public QDialog {
  Q_OBJECT

 public:
  CQPixmapRescaleDialog(QWidget *parent=NULL);

  void init(int width, int height, bool keep_aspect);

 private slots:
  void accept() override;

 signals:
  void rescale(int width, int height, bool keep_aspect);

 private:
  CQIntegerEdit *width_edit_;
  CQIntegerEdit *height_edit_;
  QCheckBox     *aspect_;
};

#endif
