#include <CQPixmapAddColorDialog.h>
#if 0
#include <CQColorChooser.h>
#else
#include <CQColorSelector.h>
#endif

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

CQPixmapAddColorDialog::
CQPixmapAddColorDialog(QWidget *parent) :
 QDialog(parent)
{
  setWindowTitle("Add Color");

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  QHBoxLayout *color_layout = new QHBoxLayout;
  color_layout->setMargin(2); color_layout->setSpacing(2);

#if 0
  QLabel *color_label = new QLabel("Color");

  color_edit_ = new CQColorChooser;

  color_layout->addWidget(color_label);
  color_layout->addWidget(color_edit_);

  connect(color_edit_, SIGNAL(colorChanged(const QString &)), this, SLOT(accept()));

  layout->addLayout(color_layout);
#else
  selector_ = new CQColorSelector;

  layout->addWidget(selector_);
#endif

  QHBoxLayout *button_layout = new QHBoxLayout;
  button_layout->setMargin(2); button_layout->setSpacing(2);

  QPushButton *ok     = new QPushButton("OK");
  QPushButton *apply  = new QPushButton("Apply");
  QPushButton *cancel = new QPushButton("Cancel");

  button_layout->addWidget(ok);
  button_layout->addWidget(apply);
  button_layout->addWidget(cancel);

  layout->addLayout(button_layout);

  connect(ok    , SIGNAL(clicked()), this, SLOT(okSlot()));
  connect(apply , SIGNAL(clicked()), this, SLOT(applySlot()));
  connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
}

void
CQPixmapAddColorDialog::
okSlot()
{
  applySlot();

  close();
}

void
CQPixmapAddColorDialog::
applySlot()
{
  static QString colorName;

#if 0
  colorName = color_edit_->colorName();

  emit addColor(colorName);
#else
  QColor c = selector_->color();

  emit addColor(c);
#endif
}
