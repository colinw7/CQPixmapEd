// TODO: allow control of scale to allow R,G,B,H,S,V
// Square for R,G,B : Circle H,S,V

#include <CColorWheel.h>
#include <CMathGen.h>
#include <cmath>

CColorWheel::
CColorWheel(int radius, Qt::Orientation orient, bool show_swatches) :
 orient_       (orient),
 show_swatches_(show_swatches),
 control_type_ (CONTROL_RED),
 mode_         (MODE_FOREGROUND),
 radius_       (radius),
 tradius_      (radius),
 xc_           (0),
 bg_rgb_       (0,0,0),
 fg_rgb_       (1,1,1),
 bg_hsv_       (0,0,0),
 fg_hsv_       (0,0,1)
{
}

CColorWheel::
~CColorWheel()
{
}

void
CColorWheel::
draw(int w, int h)
{
  int margin = 4;

  // calc coordinates

  ws_ = w/10;

  if (show_swatches_)
    ss_ = std::min(w, h)/6;
  else
    ss_ = 0;

  int wc = w - ws_ - 3*margin;
  int hc = h - 2*margin;

  if (show_swatches_) {
    if (orient_ == Qt::Vertical)
      hc -= ss_ + margin;
    else
      wc -= ss_ + margin;
  }

  tradius_ = radius_;

  if (tradius_ == 0)
    tradius_ = std::min(wc/2, hc/2);

  // draw color wheel

  QColor rgb;

  xc_ = wc/2 + margin;
  yc_ = hc/2 + margin;

  int x, y;

  for (y = yc_ - tradius_; y < yc_ + tradius_; ++y) {
    for (x = xc_ - tradius_; x < xc_ + tradius_; ++x) {
      if (! posToRGB(x, y, rgb))
        continue;

      setForeground(rgb);

      drawPoint(x, y);
    }
  }

  // draw cross

  int sx, sy;

  rgbToPos(getRGB(), x, y, sx, sy);

  setForeground(QColor(0,0,0));

  drawLine(x - 2, y, x + 2, y);
  drawLine(x, y - 2, x, y + 2);

  // draw value scale

  xs_ = xc_ + tradius_ + margin;

  for (int y = yc_ - tradius_, y1 = 0; y < yc_ + tradius_; ++y, ++y1) {
    int v = 255*(1.0 - (0.5*y1)/tradius_);

    if      (control_type_ == CONTROL_RED)
      rgb = QColor(v, 0, 0);
    else if (control_type_ == CONTROL_GREEN)
      rgb = QColor(0, v, 0);
    else if (control_type_ == CONTROL_BLUE)
      rgb = QColor(0, 0, v);
    else if (control_type_ == CONTROL_HUE) {
      QColor hsv;

      hsv.setHsv(360*v, 255, 255);

      rgb = hsv.toRgb();
    }
    else if (control_type_ == CONTROL_SATURATION)
      rgb = QColor(v, v, v);
    else if (control_type_ == CONTROL_VALUE)
      rgb = QColor(v, v, v);

    setForeground(rgb);

    drawLine(xs_, y, xs_ + ws_, y);
  }

  // draw current value on scale

  int v = 0;

  if      (control_type_ == CONTROL_RED)
    v  = getRGB().red();
  else if (control_type_ == CONTROL_GREEN)
    v  = getRGB().green();
  else if (control_type_ == CONTROL_BLUE)
    v  = getRGB().blue();
  else if (control_type_ == CONTROL_HUE)
    v  = getHSV().hue()/360.0;
  else if (control_type_ == CONTROL_SATURATION)
    v  = getHSV().saturation();
  else if (control_type_ == CONTROL_VALUE)
    v  = getHSV().value();

  int yy = int(yc_ + (1 - 2*v)*tradius_);

  if (control_type_ == CONTROL_VALUE)
    setForeground(QColor(0,0,255));
  else
    setForeground(QColor(255,255,255));

  drawLine(xs_, yy, xs_ + ws_, yy);

  // draw swatches

  if (show_swatches_) {
    if (orient_ == Qt::Vertical) {
      xs1_ = margin;
      ys1_ = h - ss_ - margin - 1;

      xs2_ = xs1_ + ss_ + margin;
      ys2_ = ys1_;
    }
    else {
      xs1_ = xs_ + ws_ + margin;
      ys1_ = yc_ - tradius_;

      xs2_ = xs1_;
      ys2_ = ys1_ + ss_ + margin;
    }

    setForeground(QColor(255,255,255));

    if (mode_ == MODE_BACKGROUND)
      fillRectangle(QRect(xs1_ - 2, ys1_ - 2, ss_ + 2, ss_ + 2));
    else
      fillRectangle(QRect(xs2_ - 2, ys2_ - 2, ss_ + 2, ss_ + 2));

    setForeground(QColor(0,0,0));

    if (mode_ == MODE_BACKGROUND)
      fillRectangle(QRect(xs1_ - 1, ys1_ - 1, ss_ + 1, ss_ + 1));
    else
      fillRectangle(QRect(xs2_ - 1, ys2_ - 1, ss_ + 1, ss_ + 1));

    setForeground(bg_rgb_);

    fillRectangle(QRect(xs1_, ys1_, ss_, ss_));

    setForeground(fg_rgb_);

    fillRectangle(QRect(xs2_, ys2_, ss_, ss_));
  }
  else {
    xs1_ = 0;
    ys1_ = 0;

    xs2_ = 0;
    ys2_ = 0;
  }
}

void
CColorWheel::
select(int x, int y)
{
  if      (insideWheel(x, y) || insideSlider(x, y)) {
    QColor rgb;

    if (mode_ == MODE_BACKGROUND)
      posToRGB(x, y, rgb);
    else
      posToRGB(x, y, rgb);

    setRGB(rgb);
  }
  else if (insideBackground(x, y))
    mode_ = MODE_BACKGROUND;
  else if (insideForeground(x, y))
    mode_ = MODE_FOREGROUND;
}

bool
CColorWheel::
posToRGB(int x, int y, QColor &rgb) const
{
  if      (control_type_ == CONTROL_RED) {
    rgb = getRGB();

    if      (x >= xc_ - tradius_ && x < xc_ + tradius_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double g =       (0.5*(x - (xc_ - tradius_)))/tradius_;
      double b = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      rgb.setGreen(255*g);
      rgb.setBlue (255*b);
    }
    else if (x >= xs_            && x < xs_ + ws_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      rgb.setRed(255*v);
    }
  }
  else if (control_type_ == CONTROL_GREEN) {
    rgb = getRGB();

    if      (x >= xc_ - tradius_ && x < xc_ + tradius_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double r =       (0.5*(x - (xc_ - tradius_)))/tradius_;
      double b = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      rgb.setRed (255*r);
      rgb.setBlue(255*b);
    }
    else if (x >= xs_            && x < xs_ + ws_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      rgb.setGreen(255*v);
    }
  }
  else if (control_type_ == CONTROL_BLUE) {
    rgb = getRGB();

    if      (x >= xc_ - tradius_ && x < xc_ + tradius_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double r =       (0.5*(x - (xc_ - tradius_)))/tradius_;
      double g = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      rgb.setRed  (255*r);
      rgb.setGreen(255*g);
    }
    else if (x >= xs_            && x < xs_ + ws_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      rgb.setBlue(255*v);
    }
  }
  else if (control_type_ == CONTROL_HUE) {
    QColor hsv = getHSV();

    if      (x >= xc_ - tradius_ && x < xc_ + tradius_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double s =       (0.5*(x - (xc_ - tradius_)))/tradius_;
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      hsv.setHsv(hsv.hue(), 255*s, 255*v);
    }
    else if (x >= xs_            && x < xs_ + ws_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      hsv.setHsv(360*v, hsv.saturation(), hsv.value());
    }

    rgb = hsv.toRgb();
  }
  else if (control_type_ == CONTROL_SATURATION) {
    QColor hsv = getHSV();

    if      (x >= xc_ - tradius_ && x < xc_ + tradius_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double h =       (0.5*(x - (xc_ - tradius_)))/tradius_;
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      hsv.setHsv(360*h, hsv.saturation(), 255*v);
    }
    else if (x >= xs_            && x < xs_ + ws_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      hsv.setHsv(hsv.hue(), 255*v, hsv.value());
    }

    rgb = hsv.toRgb();
  }
  else if (control_type_ == CONTROL_VALUE) {
    QColor hsv = getHSV();

    if      (x >= xc_ - tradius_ && x < xc_ + tradius_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      rgb = QColor(0, 0, 0);

      double x1 = x - xc_;
      double y1 = yc_ - y;

      double hypot = sqrt(x1*x1 + y1*y1);

      if (hypot > tradius_)
        return false;

      double a = RAD_TO_DEG(atan2(x1, y1));

      a -= 90.0;

      if (a < 0)
        a += 360;

      hsv.setHsv(a, hypot/tradius_*255, hsv.value());
    }
    else if (x >= xs_            && x < xs_ + ws_ &&
             y >= yc_ - tradius_ && y < yc_ + tradius_) {
      double v = 1.0 - (0.5*(y - (yc_ - tradius_)))/tradius_;

      hsv.setHsv(hsv.hue(), hsv.saturation(), 255*v);
    }

    rgb = hsv.toRgb();
  }

  return true;
}

bool
CColorWheel::
rgbToPos(const QColor &rgb, int &wx, int &wy, int &sx, int &sy) const
{
  sx = xs_;

  double r = rgb.red  ()/255.0;
  double g = rgb.green()/255.0;
  double b = rgb.blue ()/255.0;

  QColor hsv = rgb.toHsv();

  double h = hsv.hue       ()/360.0;
  double s = hsv.saturation()/255.0;
  double v = hsv.value     ()/255.0;

  if      (control_type_ == CONTROL_RED) {
    wx = int(xc_ + (2*g - 1)*tradius_);
    wy = int(yc_ + (1 - 2*b)*tradius_);

    sy = int(yc_ + (1 - 2*r)*tradius_);
  }
  else if (control_type_ == CONTROL_GREEN) {
    wx = int(xc_ + (2*r - 1)*tradius_);
    wy = int(yc_ + (1 - 2*b)*tradius_);

    sy = int(yc_ + (1 - 2*g)*tradius_);
  }
  else if (control_type_ == CONTROL_BLUE) {
    wx = int(xc_ + (2*r - 1)*tradius_);
    wy = int(yc_ + (1 - 2*g)*tradius_);

    sy = int(yc_ + (1 - 2*b)*tradius_);
  }
  else if (control_type_ == CONTROL_HUE) {
    wx = int(xc_ + (2*s - 1)*tradius_);
    wy = int(yc_ + (1 - 2*v)*tradius_);

    sy = int(yc_ + (1 - 2*hsv.hue()/360.0)*tradius_);
  }
  else if (control_type_ == CONTROL_SATURATION) {
    wx = int(xc_ + (2*h - 1)*tradius_);
    wy = int(yc_ + (1 - 2*v)*tradius_);

    sy = int(yc_ + (1 - 2*s)*tradius_);
  }
  else if (control_type_ == CONTROL_VALUE) {
    double h = hsv.hue() + 90;

    if (h > 360)
      h -= 360;

    double r = tradius_*s;
    double a = DEG_TO_RAD(h);

    wx = int(xc_ + r*cos(a));
    wy = int(yc_ - r*sin(a));

    sy = int(yc_ + (1 - 2*v)*tradius_);
  }

  return true;
}

void
CColorWheel::
setRGB(const QColor &rgb)
{
  if (mode_ == MODE_FOREGROUND) {
    fg_rgb_ = rgb;
    fg_hsv_ = fg_rgb_.toHsv();
  }
  else {
    bg_rgb_ = rgb;
    bg_hsv_ = bg_rgb_.toHsv();
  }
}

void
CColorWheel::
setHSV(const QColor &hsv)
{
  if (mode_ == MODE_FOREGROUND) {
    fg_hsv_ = hsv;
    fg_rgb_ = fg_hsv_.toRgb();
  }
  else {
    bg_hsv_ = hsv;
    bg_rgb_ = bg_hsv_.toRgb();
  }
}

void
CColorWheel::
setControlType(ControlType type)
{
  control_type_ = type;
}
