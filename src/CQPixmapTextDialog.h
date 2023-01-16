#ifndef CQPixmapTextDialog_H
#define CQPixmapTextDialog_H

#include <QDialog>

class QLineEdit;

class CQPixmapTextDialog : public QDialog {
  Q_OBJECT

 public:
  CQPixmapTextDialog(QWidget *parent=NULL);

  void init(const QString &str);

 private slots:
  void accept() override;

 signals:
  void setText(const QString &str);

 private:
  QLineEdit *text_edit_;
};

#endif
