class CTclPixmapFile;
class CTclPixmapCanvas;

#include <CTclApp.h>
#include <CFontStyle.h>
#include <CUndo.h>
#include <CBresenham.h>
#include <CIRange2D.h>
#include <CPixelRenderer.h>

enum CTclPixmapFunction {
  CTCL_PIXMAP_FUNCTION_COPY,
  CTCL_PIXMAP_FUNCTION_MOVE,
  CTCL_PIXMAP_FUNCTION_MARK,
  CTCL_PIXMAP_FUNCTION_POINT,
  CTCL_PIXMAP_FUNCTION_CURVE,
  CTCL_PIXMAP_FUNCTION_LINE,
  CTCL_PIXMAP_FUNCTION_RECTANGLE,
  CTCL_PIXMAP_FUNCTION_FILLED_RECTANGLE,
  CTCL_PIXMAP_FUNCTION_CIRCLE,
  CTCL_PIXMAP_FUNCTION_FILLED_CIRCLE,
  CTCL_PIXMAP_FUNCTION_FLOOD_FILL,
  CTCL_PIXMAP_FUNCTION_TEXT,
  CTCL_PIXMAP_FUNCTION_SET_HOT_SPOT
};

class CTclPixmapRenderer;
class CTclPixmapXorRenderer;
class CTclPixmapCanvasRenderer;
class CXrtFont;

class CTclPixmap : public CTclApp {
 private:
  typedef std::vector<CTclPixmapFile *> FileList;

  CTclPixmapCanvas         *canvas_;
  CTclPixmapCanvas         *thumbnail_canvas_;
  CTclPixmapFunction        function_;
  int                       x_, y_;
  int                       dx_, dy_;
  CIRange2D                 rect_;
  bool                      mark_motion_;
  CIRange2D                 mark_;
  int                       mark_sx_, mark_sy_;
  int                       mark_ex_, mark_ey_;
  int                       last_x_, last_y_;
  std::string               filename_;
  CImagePtr                 image_;
  bool                      grid_;
  int                       grid_size_;
  CRGB                     *colors_;
  int                       num_colors_;
  int                       bg_color_num_;
  CRGB                      bg_color_;
  int                       fg_color_num_;
  CRGB                      fg_color_;
  int                       transparent_color_num_;
  bool                      auto_load_;
  CUndo                    *undo_;
  CImagePtr                 stipple_image_;
  CTclPixmapRenderer       *renderer_;
  CTclPixmapXorRenderer    *xor_renderer_;
  CTclPixmapCanvasRenderer *canvas_renderer_;
  CPixelRendererBresenham  *bresenham_;
  CPixelRendererBresenham  *bresenham_xor_;
  CPixelRendererBresenham  *bresenham_canvas_;
  std::string               font_name_;
  CFontStyle                font_style_;
  int                       font_size_;
  int                       font_angle_;
  std::string               font_text_;
  CXrtFont                 *font_;
  int                       x_hot_, y_hot_;

 public:
  CTclPixmap(int argc, char **argv);
 ~CTclPixmap();

  uint getWidth ();
  uint getHeight();

  void addCanvas(CTclPixmapCanvas *canvas);
  void addThumbnailCanvas(CTclPixmapCanvas *canvas);

  void loadFile(const std::string &filename);
  void insertFile(const std::string &filename);
  void saveFile(const std::string &filename);
  void saveFile();

  void undo();
  void redo();

  void clear();
  void clear(int left, int bottom, int right, int top);

  void set();

  void invert();

  void redraw();

  void scrollX(int offset);
  void scrollY(int offset);

  void flipH();
  void flipV();
  void flipHV();

  void rotate(int angle);

  void floodFill(int x, int y);

  void zoom(int scale);

  void setGrid(int flag);

  void setFunction(const char *fname);

  void clearHotSpot();

  void unmark();

  void setFgColorNum(int num);
  void setBgColorNum(int num);

  void resizeImage(int width, int height);
  void rescaleImage(int width, int height);

  void setTextStr(const std::string &str);
  void setTextFont(const std::string &name, CFontStyle style, int size, int angle);
  void setTextFont();

  void addColor(const std::string &name);

  void draw();
  void drawCanvasLine(int x1, int y1, int x2, int y2);
  void drawCanvasPoint(int x, int y);
  void drawCanvasPoint(int x, int y, int px, int py);
  void drawThumbnail();
  void drawOverlay();

  void fillXorPixel(int x, int y);

  void drawPoint(int x, int y, int num);

  void setScrollbars();

  void scrollToX(double x);
  void scrollByX(double x);
  void scrollToY(double y);
  void scrollByY(double y);

  void mousePress  (int x, int y, int num);
  void mouseMotion (int x, int y, int num);
  void mouseRelease(int x, int y, int num);

  void keyPress(const std::string &key);

  void addUndoImage();
  void addUndoImage(const CIRange2D &rect);

  CImagePtr getImage(int x, int y, int width, int height);

  void drawImage(int x, int y, CImagePtr &image);

 protected:
  void addCommands();

  void startup();

  std::string getTclStr();

 private:
  void createNewImage();

  void initImageColors();

  void drawRectangle(int x, int y, int w, int h, int num);
  void fillRectangle(int x, int y, int w, int h, int num);

  void drawLine(int x1, int y1, int x2, int y2, int num);

  void drawCircle(int x1, int y1, int x2, int y2, int num);
  void fillCircle(int x1, int y1, int x2, int y2, int num);

  void drawText(int x, int y, const std::string &text, int num);

  void drawPixelPoint(int x, int y, int num);

  void copyArea(const CIRange2D &from, const CIRange2D &to);
  void moveArea(const CIRange2D &from, const CIRange2D &to);

  void drawXorRectangle();
  void drawXorLine();
  void drawXorCircle();
  void fillXorCircle();
  void drawXorText();

  void setXorRectStart(int x, int y);
  void setXorRectEnd  (int x, int y);

  void pixelToWindow(int px, int py, int *wx, int *wy);
  void windowToPixel(int wx, int wy, int *px, int *py);
};

class CTclPixmapUndoImage : public CUndoData {
 private:
  CTclPixmap *pixmap_;
  int         x_, y_;
  CImagePtr   image_;

 public:
  CTclPixmapUndoImage(CTclPixmap *pixmap, int x, int y, CImagePtr &image) :
   pixmap_(pixmap), x_(x), y_(y), image_(image) {
  }

 ~CTclPixmapUndoImage() {
  }

  bool exec();
};

class CTclPixmapRenderer : public CPixelRenderer {
 private:
  CTclPixmap *pixmap_;
  int         color_num_;

 public:
  CTclPixmapRenderer(CTclPixmap *pixmap) : pixmap_(pixmap) { }

  CTclPixmapRenderer *dup() const { return new CTclPixmapRenderer(pixmap_); }

  virtual ~CTclPixmapRenderer() { }

  uint getWidth () const { return pixmap_->getWidth (); }
  uint getHeight() const { return pixmap_->getHeight(); }

  void setPixmap(CTclPixmap *pixmap) { pixmap_ = pixmap; }

  void setColorNum(int color_num) { color_num_ = color_num; }

  void drawPoint(int x, int y) {
    pixmap_->drawPoint(x, y, color_num_);
  }

  void drawClippedPoint(const CIPoint2D &p) {
    drawPoint(p.x, p.y);
  }
};

class CTclPixmapCanvasRenderer : public CPixelRenderer {
 private:
  CTclPixmap *pixmap_;

 public:
  CTclPixmapCanvasRenderer(CTclPixmap *pixmap) : pixmap_(pixmap) { }

  virtual ~CTclPixmapCanvasRenderer() { }

  CTclPixmapCanvasRenderer *dup() const { return new CTclPixmapCanvasRenderer(pixmap_); }

  uint getWidth () const { return pixmap_->getWidth (); }
  uint getHeight() const { return pixmap_->getHeight(); }

  void setPixmap(CTclPixmap *pixmap) { pixmap_ = pixmap; }

  void drawPoint(int x, int y) {
    pixmap_->drawCanvasPoint(x, y);
  }

  void drawClippedPoint(const CIPoint2D &p) {
    drawPoint(p.x, p.y);
  }
};

class CTclPixmapXorRenderer : public CPixelRenderer {
 private:
  CTclPixmap *pixmap_;

 public:
  CTclPixmapXorRenderer(CTclPixmap *pixmap) : pixmap_(pixmap) { }

  virtual ~CTclPixmapXorRenderer() { }

  CTclPixmapXorRenderer *dup() const { return new CTclPixmapXorRenderer(pixmap_); }

  uint getWidth () const { return pixmap_->getWidth (); }
  uint getHeight() const { return pixmap_->getHeight(); }

  void setPixmap(CTclPixmap *pixmap) { pixmap_ = pixmap; }

  void drawPoint(int x, int y) {
    pixmap_->fillXorPixel(x, y);
  }

  void drawClippedPoint(const CIPoint2D &p) {
    drawPoint(p.x, p.y);
  }
};
