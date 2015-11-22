#include <CQPixmapResizeDialog.h>

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

CQPixmapResizeDialog::
CQPixmapResizeDialog(QWidget *parent) :
 QDialog(parent)
{
  setWindowTitle("Resize");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  QHBoxLayout *width_layout = new QHBoxLayout;
  width_layout->setMargin(2); width_layout->setSpacing(2);

  QLabel *width_label = new QLabel("Width");

  width_edit_ = new QLineEdit;

  width_layout->addWidget(width_label);
  width_layout->addWidget(width_edit_);

  layout->addLayout(width_layout);

  QHBoxLayout *height_layout = new QHBoxLayout;
  height_layout->setMargin(2); height_layout->setSpacing(2);

  QLabel *height_label = new QLabel("Height");

  height_edit_ = new QLineEdit;

  height_layout->addWidget(height_label);
  height_layout->addWidget(height_edit_);

  layout->addLayout(height_layout);

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
CQPixmapResizeDialog::
init(int width, int height)
{
  width_edit_ ->setText(QString("%1").arg(width ));
  height_edit_->setText(QString("%1").arg(height));
}

void
CQPixmapResizeDialog::
accept()
{
  int w = width_edit_ ->text().toInt();
  int h = height_edit_->text().toInt();

  emit resize(w, h);

  close();
}
