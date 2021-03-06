#ifndef CIRANGE_2D_H
#define CIRANGE_2D_H

template<typename T>
class CIRange2DT {
 public:
  T x1, y1, x2, y2;

  explicit CIRange2DT(T x11 = 0, T y11 = 0, T x21 = 0, T y21 = 0) {
    set(x11, y11, x21, y21);
  }

  void set(T x11, T y11, T x21, T y21) {
    x1 = x11, y1 = y11, x2 = x21, y2 = y21;
  }

  void get(T *x11, T *y11, T *x21, T *y21) const {
    *x11 = x1, *y11 = y1, *x21 = x2, *y21 = y2;
  }

  T dx() const { return x2 - x1; }
  T dy() const { return y2 - y1; }

  T xmid() const { return (x2 + x1) >> 1; }
  T ymid() const { return (y2 + y1) >> 1; }

  T xmin() const { return std::min(x1, x2); }
  T ymin() const { return std::min(y1, y2); }
  T xmax() const { return std::max(x1, x2); }
  T ymax() const { return std::max(y1, y2); }

  T xsize() const { return abs(x2 - x1) + 1; }
  T ysize() const { return abs(y2 - y1) + 1; }

  void inc(T dx, T dy) { x1 += dx; y1 += dy; x2 += dx; y2 += dy; }

  void incX(T dx) { x1 += dx; x2 += dx; }
  void incY(T dy) { y1 += dy; y2 += dy; }

  CIRange2DT &operator=(const CIRange2DT &range) {
    x1 = range.x1; y1 = range.y1;
    x2 = range.x2; y2 = range.y2;

    return *this;
  }
};

typedef CIRange2DT<int> CIRange2D;

#endif
