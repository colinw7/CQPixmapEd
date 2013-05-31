#ifndef CQPixmapResizeDialog_H
#define CQPixmapResizeDialog_H

#include <QDialog>

class QLineEdit;

class CQPixmapResizeDialog : public QDialog {
  Q_OBJECT

 public:
  CQPixmapResizeDialog(QWidget *parent=NULL);

  void init(int width, int height);

 private slots:
  void accept();

 signals:
  void resize(int width, int height);

 private:
  QLineEdit *width_edit_;
  QLineEdit *height_edit_;
};

#endif
