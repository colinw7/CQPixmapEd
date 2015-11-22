#include <CQPixmapTextDialog.h>

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

CQPixmapTextDialog::
CQPixmapTextDialog(QWidget *parent) :
 QDialog(parent)
{
  setWindowTitle("Set Text");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  QHBoxLayout *text_layout = new QHBoxLayout;
  text_layout->setMargin(2); text_layout->setSpacing(2);

  QLabel *text_label = new QLabel("Text");

  text_edit_ = new QLineEdit;

  text_layout->addWidget(text_label);
  text_layout->addWidget(text_edit_);

  layout->addLayout(text_layout);

  QHBoxLayout *button_layout = new QHBoxLayout;
  button_layout->setMargin(2); button_layout->setSpacing(2);

  QPushButton *ok     = new QPushButton("OK");
  QPushButton *cancel = new QPushButton("Cancel");

  button_layout->addWidget(ok);
  button_layout->addWidget(cancel);

  layout->addLayout(button_layout);

  connect(ok    , SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
}

void
CQPixmapTextDialog::
init(const QString &str)
{
  text_edit_->setText(str);
}

void
CQPixmapTextDialog::
accept()
{
  emit setText(text_edit_->text());

  close();
}
