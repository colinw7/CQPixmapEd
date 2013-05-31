#include <CQPixmapRescaleDialog.h>

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

CQPixmapRescaleDialog::
CQPixmapRescaleDialog(QWidget *parent) :
 QDialog(parent)
{
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

  aspect_ = new QCheckBox("Keep Aspect");

  layout->addWidget(aspect_);

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
CQPixmapRescaleDialog::
init(int width, int height, bool keep_aspect)
{
  width_edit_ ->setText(QString("%1").arg(width ));
  height_edit_->setText(QString("%1").arg(height));

  aspect_->setChecked(keep_aspect);
}

void
CQPixmapRescaleDialog::
accept()
{
  int w = width_edit_ ->text().toInt();
  int h = height_edit_->text().toInt();

  emit rescale(w, h, aspect_->isChecked());

  close();
}
