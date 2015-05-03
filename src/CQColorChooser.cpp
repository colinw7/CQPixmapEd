#include <CQColorChooser.h>

#include <QLineEdit>
#include <QHBoxLayout>

CQColorChooser::
CQColorChooser(QWidget *parent) :
 QWidget(parent), editable_(true)
{
  init();
}

void
CQColorChooser::
init()
{
  setObjectName("colorChooser");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  color_     = QColor(1,2,3);
  alpha_     = 0.456;
  colorName_ = "abcdef";

  //-----

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  cedit_ = new QLineEdit(this); // text color

  layout->addWidget(cedit_);
  layout->addStretch();

  cedit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  connect(cedit_, SIGNAL(editingFinished()), this, SLOT(textColorChanged()));

  update();

  setColor(color_, alpha_);

  setFocusProxy(cedit_);
}

void
CQColorChooser::
setEditable(bool editable)
{
  editable_ = editable;

  update();
}

void
CQColorChooser::
update()
{
  cedit_->setReadOnly(! editable_);
}

void
CQColorChooser::
setColor(const QColor &color, double alpha)
{
  if (! color.isValid())
    return;

  QColor color1(color);

  color1.setAlpha(255);

  if (color_ == color1 && alpha_ == alpha_)
    return;

  color_     = color1;
  colorName_ = color_.name();

  cedit_->setText(colorName_);

  alpha_ = alpha;
}

void
CQColorChooser::
setRGBA(const QColor &rgba)
{
  setColor(rgba);
}

QColor
CQColorChooser::
getRGBA() const
{
  return color();
}

void
CQColorChooser::
setColorName(const QString &colorName, double alpha)
{
  QColor color(colorName);

  if (! color.isValid())
    return;

  QColor color1(color);

  color1.setAlpha(255);

  if (color_ == color1 && alpha_ == alpha_)
    return;

  color_     = color1;
  colorName_ = colorName;

  cedit_->setText(colorName_);

  alpha_ = alpha;
}

const QColor &
CQColorChooser::
color() const
{
  return color_;
}

QString
CQColorChooser::
colorName() const
{
  return colorName_;
}

void
CQColorChooser::
setAlpha(double alpha)
{
  alpha_ = alpha;
}

double
CQColorChooser::
getAlpha() const
{
  return alpha_;
}

void
CQColorChooser::
textColorChanged()
{
  setColorName(cedit_->text(), alpha_);

  changeColor();
}

void
CQColorChooser::
changeColor()
{
  QColor color(color_);

  color_.setAlpha(255*alpha_);

  emit colorChanged(color);
  emit colorChanged(colorName_);
}
