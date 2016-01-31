#ifndef CQLAYOUT_H
#define CQLAYOUT_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

class CQHLayout : public QHBoxLayout {
 public:
  enum OptionType {
    OPTION_NONE          = 0,
    OPTION_FIXED_WIDTH   = (1<<0),
    OPTION_FIXED_HEIGHT  = (1<<1),
    OPTION_EXPAND_WIDTH  = (1<<2),
    OPTION_EXPAND_HEIGHT = (1<<3)
  };

 private:
  class Option {
   private:
    uint types_;

   public:
    Option(OptionType type1=OPTION_NONE, OptionType type2=OPTION_NONE) :
     types_(0){
      types_ |= type1;
      types_ |= type2;
    }

    bool hasType(OptionType type) const { return (types_ & type); }
  };

 private:
  Option options_;

 public:
  CQHLayout(QWidget *parent=0, OptionType opt1=OPTION_NONE, OptionType opt2=OPTION_NONE) :
   QHBoxLayout(parent), options_(opt1, opt2) {
    //setMargin(2); //setSpacing(2);
  }

  void addWidget(QWidget *widget) {
    QHBoxLayout::addWidget(widget);

    QSizePolicy::Policy hpolicy = QSizePolicy::Preferred;
    QSizePolicy::Policy vpolicy = QSizePolicy::Preferred;

    if      (options_.hasType(OPTION_FIXED_WIDTH))
      hpolicy = QSizePolicy::Fixed;
    else if (options_.hasType(OPTION_EXPAND_WIDTH))
      hpolicy = QSizePolicy::Expanding;

    if      (options_.hasType(OPTION_FIXED_HEIGHT))
      vpolicy = QSizePolicy::Fixed;
    else if (options_.hasType(OPTION_EXPAND_HEIGHT))
      vpolicy = QSizePolicy::Expanding;

    widget->setSizePolicy(hpolicy, vpolicy);
  }

  void addStretch() {
    QWidget *spacer = new QWidget;

    spacer->setObjectName("spacer");

    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout::addWidget(spacer);

    QHBoxLayout *spacer_layout = new QHBoxLayout(spacer);

    QWidget *spacer_child = new QWidget;

    spacer_child->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    spacer_layout->addWidget(spacer_child);
  }
};

class CQVLayout : public QVBoxLayout {
 public:
  enum OptionType {
    OPTION_NONE          = 0,
    OPTION_FIXED_WIDTH   = (1<<0),
    OPTION_FIXED_HEIGHT  = (1<<1),
    OPTION_EXPAND_WIDTH  = (1<<2),
    OPTION_EXPAND_HEIGHT = (1<<3)
  };

 private:
  class Option {
   private:
    uint types_;

   public:
    Option(OptionType type1=OPTION_NONE, OptionType type2=OPTION_NONE) :
     types_(0){
      types_ |= type1;
      types_ |= type2;
    }

    bool hasType(OptionType type) const { return (types_ & type); }
  };

 private:
  Option options_;

 public:
  CQVLayout(QWidget *parent=0, OptionType opt1=OPTION_NONE, OptionType opt2=OPTION_NONE) :
   QVBoxLayout(parent), options_(opt1, opt2) {
    //setMargin (2); //setSpacing(2);
  }

  void addWidget(QWidget *widget) {
    QVBoxLayout::addWidget(widget);

    QSizePolicy::Policy hpolicy = QSizePolicy::Preferred;
    QSizePolicy::Policy vpolicy = QSizePolicy::Preferred;

    if      (options_.hasType(OPTION_FIXED_WIDTH))
      hpolicy = QSizePolicy::Fixed;
    else if (options_.hasType(OPTION_EXPAND_WIDTH))
      hpolicy = QSizePolicy::Expanding;

    if      (options_.hasType(OPTION_FIXED_HEIGHT))
      vpolicy = QSizePolicy::Fixed;
    else if (options_.hasType(OPTION_EXPAND_HEIGHT))
      vpolicy = QSizePolicy::Expanding;

    if (options_.hasType(OPTION_EXPAND_HEIGHT))
      vpolicy = QSizePolicy::Expanding;

    widget->setSizePolicy(hpolicy, vpolicy);
  }

  void addStretch() {
    QWidget *spacer = new QWidget;

    spacer->setObjectName("spacer");

    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QVBoxLayout::addWidget(spacer);

    QVBoxLayout *spacer_layout = new QVBoxLayout(spacer);

    QWidget *spacer_child = new QWidget;

    spacer_child->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    spacer_layout->addWidget(spacer_child);
  }
};

class CQGridLayoutNL : public QObject {
  Q_OBJECT

 public:
  CQGridLayoutNL() { }
};

class CQGridLayoutPad : public QObject {
  Q_OBJECT

 public:
  CQGridLayoutPad() { }
};

class CQGridLayout : public QGridLayout {
  Q_OBJECT

 private:
  int row_, col_;

 public:
  static CQGridLayoutNL *nl() {
    static CQGridLayoutNL *o;
    if (! o) o = new CQGridLayoutNL;
    return o;
  }

  static CQGridLayoutPad *pad() {
    static CQGridLayoutPad *o;
    if (! o) o = new CQGridLayoutPad;
    return o;
  }

  CQGridLayout(QWidget *parent=0,
               QObject *child01 = NULL, QObject *child02 = NULL,
               QObject *child03 = NULL, QObject *child04 = NULL,
               QObject *child05 = NULL, QObject *child06 = NULL,
               QObject *child07 = NULL, QObject *child08 = NULL,
               QObject *child09 = NULL, QObject *child10 = NULL,
               QObject *child11 = NULL, QObject *child12 = NULL,
               QObject *child13 = NULL, QObject *child14 = NULL,
               QObject *child15 = NULL, QObject *child16 = NULL,
               QObject *child17 = NULL, QObject *child18 = NULL,
               QObject *child19 = NULL, QObject *child20 = NULL,
               QObject *child21 = NULL, QObject *child22 = NULL,
               QObject *child23 = NULL, QObject *child24 = NULL,
               QObject *child25 = NULL, QObject *child26 = NULL,
               QObject *child27 = NULL, QObject *child28 = NULL,
               QObject *child29 = NULL, QObject *child30 = NULL) :
   QGridLayout(parent), row_(0), col_(0) {
    addObject(child01); addObject(child02);
    addObject(child03); addObject(child04);
    addObject(child05); addObject(child06);
    addObject(child07); addObject(child08);
    addObject(child09); addObject(child10);
    addObject(child11); addObject(child12);
    addObject(child13); addObject(child14);
    addObject(child15); addObject(child16);
    addObject(child17); addObject(child18);
    addObject(child19); addObject(child20);
    addObject(child21); addObject(child22);
    addObject(child23); addObject(child24);
    addObject(child25); addObject(child26);
    addObject(child27); addObject(child28);
    addObject(child29); addObject(child30);
  }

  void addObject(QObject *o) {
    if (! o) return;

    CQGridLayoutNL  *nl  = qobject_cast<CQGridLayoutNL *>(o);
    CQGridLayoutPad *pad = qobject_cast<CQGridLayoutPad *>(o);

    if      (nl != NULL) {
      ++row_;

      col_ = 0;
    }
    else if (pad != NULL)
      ++col_;
    else {
      QWidget *w = qobject_cast<QWidget *>(o);
      QLayout *l = qobject_cast<QLayout *>(o);

      if      (w != NULL)
        addWidget(w, row_, col_);
      else if (l != NULL)
        addLayout(l, row_, col_);
      else
        return;

      ++col_;
    }
  }
};

#endif
