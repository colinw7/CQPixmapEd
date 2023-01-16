#ifndef CQPixmapResizeDialog_H
#define CQPixmapResizeDialog_H

#include <QDialog>

class CQIntegerEdit;

class CQPixmapResizeDialog : public QDialog {
  Q_OBJECT

 public:
  CQPixmapResizeDialog(QWidget *parent=NULL);

  void init(int width, int height);

 private slots:
  void accept() override;

 signals:
  void resize(int width, int height);

 private:
  CQIntegerEdit *width_edit_;
  CQIntegerEdit *height_edit_;
};

#endif
