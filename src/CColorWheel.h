#ifndef CCOLOR_WHEEL_H
#define CCOLOR_WHEEL_H

#include <QColor>
#include <QRect>

class CColorWheel {
 public:
  enum ControlType {
    CONTROL_RED,
    CONTROL_GREEN,
    CONTROL_BLUE,
    CONTROL_HUE,
    CONTROL_SATURATION,
    CONTROL_VALUE
  };

  enum ModeType {
    MODE_BACKGROUND,
    MODE_FOREGROUND
  };

 public:
  CColorWheel(int radius=0, Qt::Orientation orient=Qt::Vertical,
              bool show_swatches=true);

  virtual ~CColorWheel();

  ModeType getMode() const { return mode_; }

  void setMode(ModeType mode) { mode_ = mode; }

  void draw(int w, int h);

  void select(int x, int y);

  const QColor &getBgRGB() const { return bg_rgb_; }
  const QColor &getFgRGB() const { return fg_rgb_; }

  void setBgRGB(const QColor &rgb) { bg_rgb_ = rgb; }
  void setFgRGB(const QColor &rgb) { fg_rgb_ = rgb; }

  const QColor &getRGB() const {
    if (getMode() == MODE_FOREGROUND)
      return fg_rgb_;
    else
      return bg_rgb_;
  }

  void setRGB(const QColor &rgb);

  const QColor &getHSV() const {
    if (getMode() == MODE_FOREGROUND)
      return fg_hsv_;
    else
      return bg_hsv_;
  }

  void setHSV(const QColor &hsv);

  void setControlType(ControlType type);

  bool posToRGB(int x, int y, QColor &rgb) const;

  bool rgbToPos(const QColor &rgb, int &wx, int &wy, int &sx, int &sy) const;

 private:
  bool insideWheel(int x, int y) {
    return (x >= xc_ - tradius_ && x < xc_ + tradius_ &&
            y >= yc_ - tradius_ && y < yc_ + tradius_);
  }

  bool insideSlider(int x, int y) {
    return (x >= xs_ && x < xs_ + ws_ && y >= yc_ - tradius_ && y < yc_ + tradius_);
  }

  bool insideBackground(int x, int y) {
    if (show_swatches_)
      return (x >= xs1_ && x < xs1_ + ss_ && y >= ys1_ && y < ys1_ + ss_);
    else
      return false;
  }

  bool insideForeground(int x, int y) {
    if (show_swatches_)
      return (x >= xs2_ && x < xs2_ + ss_ && y >= ys2_ && y < ys2_ + ss_);
    else
      return false;
  }

  virtual void setForeground(const QColor &rgba) = 0;

  virtual void drawPoint(int x, int y) = 0;

  virtual void drawLine(int x1, int y1, int x2, int y2) = 0;

  virtual void fillRectangle(const QRect &rect) = 0;

 private:
  Qt::Orientation orient_;
  bool            show_swatches_;
  ControlType     control_type_;
  ModeType        mode_;
  int             radius_;
  int             tradius_;
  int             xc_, yc_; // color wheel center
  int             xs_, ws_; // scale x and width
  int             xs1_, ys1_; // swatch 1 (x,y)
  int             xs2_, ys2_; // swatch 2 (x,y)
  int             ss_; // swatch size
  QColor          bg_rgb_, fg_rgb_;
  QColor          bg_hsv_, fg_hsv_;
};

#endif
