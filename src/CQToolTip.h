#ifndef CQToolTip_H
#define CQToolTip_H

#include <QWidget>
#include <QPointer>
#include <QMap>

class QVBoxLayout;

class CQToolTipIFace {
 public:
  CQToolTipIFace() { }

  virtual ~CQToolTipIFace() { }

  virtual QWidget *showWidget() = 0;

  virtual void hideWidget() { }

  virtual void updateWidget() { }

  virtual bool trackMouse() const { return false; }
};

class CQToolTipWidgetIFace : public CQToolTipIFace {
 public:
  CQToolTipWidgetIFace(QWidget *w) :
    w_(w) {
  }

  virtual QWidget *showWidget() { return w_; }

 private:
  typedef QPointer<QWidget> QWidgetP;

  QWidgetP w_;
};

class CQToolTip : public QWidget {
  Q_OBJECT

 public:
  static void setToolTip(QWidget *parent, QWidget *tooltip);
  static void setToolTip(QWidget *parent, CQToolTipIFace *tooltip);

  static void unsetToolTip(QWidget *parent);

  CQToolTipIFace *getToolTip(QWidget *w) const;

 protected:
  static CQToolTip *getInstance();

  CQToolTip();
 ~CQToolTip();

  void show(const QPoint &pos, CQToolTipIFace *tooltip, QWidget *parent);

 private:
  void showAtPos(const QPoint &pos);

  void enterEvent(QEvent *e);

  void paintEvent(QPaintEvent *);

  bool eventFilter(QObject *o, QEvent *e);

  void timerEvent(QTimerEvent *event);

  void hideLater();

  void startHideTimer();

 private slots:
  void hideSlot();

 private:
  typedef QPointer<QWidget> QWidgetP;

  typedef QMap<QWidgetP,CQToolTipIFace *> WidgetMap;

  QVBoxLayout *layout_;
  WidgetMap    tooltips_;
  QWidgetP     tooltip_;
  QWidgetP     parent_;
  int          hideSecs_;
  int          hideTimer_;
};

#endif
