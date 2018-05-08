// TODO:
// All Functions support Image Windows
// Image Effects ????
// Color Wheel for non-colormap
// Middle Mouse invert (?)
// Handle Thumbnail Events Properly
// Zoom region
// Current Cursor Color on Toolbar
// Crop (Copy, Paste as New)
// Use Image Draw Code
// More efficient point drawing (no redraw)
// Undo for Resize/Rescale
// Hot Spot ?
// Fix insert Image (color remap)
// Keyboard Shortcuts
// buffer image (cut, copy, paste)

#include <CTclPixmap.h>
#include <CTclPixmapCanvas.h>
#include <CTclApp.h>
#include <CTclAppCommand.h>
#include <CImageLib.h>
#include <CImageXPM.h>
#include <CXrtFont.h>
#include <CXMachine.h>
#include <CXImage.h>
#include <CXFont.h>
#include <CRGBName.h>
#include <CFile.h>
#include <CStrUtil.h>
#include <CUtil.h>
#include <std_Xt.h>
#include <iostream>

#include "stipple64.h"

#include "CTclPixmap.k"

#define CTclPixmapAppCommand(N) \
class N : public CTclAppCommand { \
 private: \
  CTclPixmap *pixmap_; \
\
 public: \
  N(CTclPixmap *pixmap) : \
    CTclAppCommand(pixmap, #N), pixmap_(pixmap) { \
  } \
\
 protected: \
  bool proc(int argc, const char **argv); \
};

CTclPixmapAppCommand(CTclPixmapLoadFile)
CTclPixmapAppCommand(CTclPixmapInsertFile)
CTclPixmapAppCommand(CTclPixmapSaveFile)
CTclPixmapAppCommand(CTclPixmapUndo)
CTclPixmapAppCommand(CTclPixmapRedo)
CTclPixmapAppCommand(CTclPixmapClear)
CTclPixmapAppCommand(CTclPixmapSet)
CTclPixmapAppCommand(CTclPixmapInvert)
CTclPixmapAppCommand(CTclPixmapRedraw)
CTclPixmapAppCommand(CTclPixmapScrollX)
CTclPixmapAppCommand(CTclPixmapScrollY)
CTclPixmapAppCommand(CTclPixmapFlip)
CTclPixmapAppCommand(CTclPixmapRotate)
CTclPixmapAppCommand(CTclPixmapZoom)
CTclPixmapAppCommand(CTclPixmapSetGrid)
CTclPixmapAppCommand(CTclPixmapSetFunction)
CTclPixmapAppCommand(CTclPixmapClearHotSpot)
CTclPixmapAppCommand(CTclPixmapUnmark)
CTclPixmapAppCommand(CTclPixmapSetFgColorNum)
CTclPixmapAppCommand(CTclPixmapSetBgColorNum)
CTclPixmapAppCommand(CTclPixmapResize)
CTclPixmapAppCommand(CTclPixmapRescale)
CTclPixmapAppCommand(CTclPixmapSetTextStr)
CTclPixmapAppCommand(CTclPixmapSetTextFont)
CTclPixmapAppCommand(CTclPixmapAddColorName)
CTclPixmapAppCommand(CTclPixmapExit)

int
main(int argc, char **argv)
{
  CTclPixmap app(argc, argv);
}

CTclPixmap::
CTclPixmap(int argc, char **argv) :
 CTclApp(argc, argv), canvas_(NULL), thumbnail_canvas_(NULL),
 function_(CTCL_PIXMAP_FUNCTION_POINT), x_(0), y_(0), dx_(0), dy_(0),
 mark_(-1, -1, -1, -1), filename_("scratch"), grid_(true),
 grid_size_(16), colors_(NULL), num_colors_(0), bg_color_num_(0),
 bg_color_(CRGB(1,1,1)), fg_color_num_(0), fg_color_(CRGB(0,0,0)),
 transparent_color_num_(0), auto_load_(false), undo_(NULL),
 renderer_(NULL), xor_renderer_(NULL), canvas_renderer_(NULL),
 bresenham_(NULL), bresenham_xor_(NULL), bresenham_canvas_(NULL),
 font_name_("helvetica"), font_style_(CFONT_STYLE_NORMAL), font_size_(12),
 font_angle_(0), font_text_("Abc"), x_hot_(-1), y_hot_(-1)
{
  if (argc > 1) {
    filename_ = argv[1];

    auto_load_ = true;
  }

  undo_ = new CUndo;

  CImageNameSrc src("CTclPixmap/stipple_image");

  stipple_image_ = CImageMgrInst->createImage(src);

  if (! stipple_image_->read(stipple64_data, STIPPLE64_DATA_LEN))
    exit(1);

  renderer_        = new CTclPixmapRenderer(this);
  xor_renderer_    = new CTclPixmapXorRenderer(this);
  canvas_renderer_ = new CTclPixmapCanvasRenderer(this);

  bresenham_        = new CPixelRendererBresenham(renderer_);
  bresenham_xor_    = new CPixelRendererBresenham(xor_renderer_);
  bresenham_canvas_ = new CPixelRendererBresenham(canvas_renderer_);

  init();
}

CTclPixmap::
~CTclPixmap()
{
  delete bresenham_;
  delete bresenham_xor_;
  delete bresenham_canvas_;
  delete renderer_;
  delete xor_renderer_;
  delete canvas_renderer_;
  delete font_;
}

void
CTclPixmap::
addCommands()
{
  new CTclPixmapCanvasCmd         (this);
  new CTclPixmapThumbnailCanvasCmd(this);

  new CTclPixmapLoadFile     (this);
  new CTclPixmapInsertFile   (this);
  new CTclPixmapSaveFile     (this);
  new CTclPixmapUndo         (this);
  new CTclPixmapRedo         (this);
  new CTclPixmapClear        (this);
  new CTclPixmapSet          (this);
  new CTclPixmapInvert       (this);
  new CTclPixmapRedraw       (this);
  new CTclPixmapScrollX      (this);
  new CTclPixmapScrollY      (this);
  new CTclPixmapFlip         (this);
  new CTclPixmapRotate       (this);
  new CTclPixmapZoom         (this);
  new CTclPixmapSetGrid      (this);
  new CTclPixmapSetFunction  (this);
  new CTclPixmapClearHotSpot (this);
  new CTclPixmapUnmark       (this);
  new CTclPixmapSetFgColorNum(this);
  new CTclPixmapSetBgColorNum(this);
  new CTclPixmapResize       (this);
  new CTclPixmapRescale      (this);
  new CTclPixmapSetTextStr   (this);
  new CTclPixmapSetTextFont  (this);
  new CTclPixmapAddColorName (this);
  new CTclPixmapExit         (this);
}

uint
CTclPixmap::
getWidth()
{
  return renderer_->getWidth();
}

uint
CTclPixmap::
getHeight()
{
  return renderer_->getHeight();
}

void
CTclPixmap::
startup()
{
  if (auto_load_)
    loadFile(filename_);

  if (! image_)
    createNewImage();

  eval("gset ctcl_pixmap_font_name  {%s}", font_name_.c_str());
  eval("gset ctcl_pixmap_font_style {%s}", CFont::fontStyleToString(font_style_).c_str());
  eval("gset ctcl_pixmap_font_size  {%d}", font_size_);
  eval("gset ctcl_pixmap_font_angle {%d}", font_angle_);
  eval("gset ctcl_pixmap_font_text  {%s}", font_text_.c_str());
}

void
CTclPixmap::
createNewImage()
{
  CImageNameSrc src("CTclPixmap::new_image");

  image_ = CImageMgrInst->createImage(src);

  image_->setDataSize(32, 32);

  image_->addColor(CRGB(0));
  image_->addColor(CRGB(1));
  image_->addColor(CRGBA(0,0,0,0));

  initImageColors();

  if (image_->hasColormap()) {
    if (bg_color_num_ >= 0)
      image_->setColorIndexData(bg_color_num_);
  }
  else
    image_->setRGBAData(bg_color_);
}

std::string
CTclPixmap::
getTclStr()
{
  return CTclPixmap_TK;
}

void
CTclPixmap::
addCanvas(CTclPixmapCanvas *canvas)
{
  canvas_ = canvas;
}

void
CTclPixmap::
addThumbnailCanvas(CTclPixmapCanvas *canvas)
{
  thumbnail_canvas_ = canvas;
}

void
CTclPixmap::
loadFile(const std::string &filename)
{
  filename_ = filename;

  CFile file(filename_);

  CImageFileSrc src(file);

  image_ = CImageMgrInst->createImage(src);

  if (! image_->read(filename))
    createNewImage();
  else
    initImageColors();

  redraw();
}

void
CTclPixmap::
initImageColors()
{
  eval("CTclPixmapClearColors");

  int r, g, b, a;

  if (image_->hasColormap()) {
    int num_colors = image_->getNumColors();

    int black_color_num       = -1;
    int white_color_num       = -1;
    int transparent_color_num = -1;

    for (int i = 0; i < num_colors; ++i) {
      CRGBA color = image_->getColor(i);

      r = int(color.getRed  ()*255);
      g = int(color.getGreen()*255);
      b = int(color.getBlue ()*255);
      a = int(color.getAlpha()*255);

      if      (r ==   0 && g ==   0 && b ==   0 && a > 0)
        black_color_num = i;
      else if (r == 255 && g == 255 && b == 255 && a > 0)
        white_color_num = i;
      else if (a < 127)
        transparent_color_num = i;
    }

    if (black_color_num == -1)
      black_color_num = image_->addColor(CRGB(0));

    if (white_color_num == -1)
      white_color_num = image_->addColor(CRGB(1));

    if (transparent_color_num == -1)
      transparent_color_num = image_->addColor(CRGBA(0,0,0,0));

    eval("CTclPixmapAddColor none    %d", transparent_color_num);
    eval("CTclPixmapAddColor #000000 %d", black_color_num);
    eval("CTclPixmapAddColor #FFFFFF %d", white_color_num);

    num_colors = image_->getNumColors();

    for (int i = 0; i < num_colors; ++i) {
      CRGBA color = image_->getColor(i);

      r = int(color.getRed  ()*255);
      g = int(color.getGreen()*255);
      b = int(color.getBlue ()*255);
      a = int(color.getAlpha()*255);

      if      (r ==   0 && g ==   0 && b ==   0 && a > 0)
        continue;
      else if (r == 255 && g == 255 && b == 255 && a > 0)
        continue;
      else if (a < 127)
        continue;

      eval("CTclPixmapAddColor #%02x%02x%02x %d", r, g, b, i);
    }

    setFgColorNum(black_color_num);
    setBgColorNum(white_color_num);

    transparent_color_num_ = transparent_color_num;
  }
}

void
CTclPixmap::
insertFile(const std::string &filename)
{
  CFile file(filename);

  CImageFileSrc src(file);

  CImagePtr image = CImageMgrInst->createImage(src);

  if (! image->read(filename))
    return;

  int num_colors = image->getNumColors();

  for (int i = 0; i < num_colors; ++i)
    image_->addColor(image->getColor(i));

  initImageColors();

  int x1, y1, x2, y2;

  image_->getWindow(&x1, &y1, &x2, &y2);

  drawImage(x1, y1, image);
}

void
CTclPixmap::
saveFile(const std::string &filename)
{
  filename_ = filename;

  saveFile();
}

void
CTclPixmap::
saveFile()
{
  image_->write(filename_, CFILE_TYPE_IMAGE_XPM);
}

void
CTclPixmap::
undo()
{
  undo_->undo();

  redraw();
}

void
CTclPixmap::
redo()
{
  undo_->redo();

  redraw();
}

void
CTclPixmap::
clear()
{
  addUndoImage();

  if (image_->hasColormap()) {
    if (bg_color_num_ >= 0)
      image_->setColorIndexData(bg_color_num_);
  }
  else
    image_->setRGBAData(bg_color_);

  redraw();
}

void
CTclPixmap::
clear(int left, int bottom, int right, int top)
{
  if (image_->hasColormap()) {
    if (bg_color_num_ >= 0)
      image_->setColorIndexData(bg_color_num_, left, bottom, right, top);
  }
  else
    image_->setRGBAData(bg_color_, left, bottom, right, top);

  redraw();
}

void
CTclPixmap::
set()
{
  addUndoImage();

  if (image_->hasColormap()) {
    if (fg_color_num_ >= 0)
      image_->setColorIndexData(fg_color_num_);
  }
  else
    image_->setRGBAData(fg_color_);

  redraw();
}

void
CTclPixmap::
invert()
{
  addUndoImage();

  image_->invert();

  redraw();
}

void
CTclPixmap::
redraw()
{
  if (canvas_ != NULL)
    canvas_->redraw();

  if (thumbnail_canvas_ != NULL)
    thumbnail_canvas_->redraw();
}

void
CTclPixmap::
scrollX(int offset)
{
  addUndoImage();

  image_->scrollX(offset);

  redraw();
}

void
CTclPixmap::
scrollY(int offset)
{
  addUndoImage();

  image_->scrollY(offset);

  redraw();
}

void
CTclPixmap::
flipH()
{
  addUndoImage();

  image_->flipH();

  redraw();
}

void
CTclPixmap::
flipV()
{
  addUndoImage();

  image_->flipV();

  redraw();
}

void
CTclPixmap::
flipHV()
{
  addUndoImage();

  image_->flipHV();

  redraw();
}

void
CTclPixmap::
rotate(int angle)
{
  addUndoImage();

  int x1, y1, x2, y2;

  image_->getWindow(&x1, &y1, &x2, &y2);

  int width  = x2 - x1;
  int height = y2 - y1;

  int size = std::min(width, height);

  int xx1 = (x1 + x2 - size)/2;
  int yy1 = (y1 + y2 - size)/2;
  int xx2 = (x1 + x2 + size)/2;
  int yy2 = (y1 + y2 + size)/2;

  image_->setWindow(xx1, yy1, xx2, yy2);

  CImagePtr image = image_->rotate(angle);

  drawImage(xx1, yy1, image);

  redraw();
}

void
CTclPixmap::
floodFill(int x, int y)
{
  addUndoImage();

  if (image_->hasColormap()) {
    if (fg_color_num_ >= 0)
      image_->floodFill(x, y, fg_color_num_);
  }
  else
    image_->floodFill(x, y, fg_color_);

  redraw();
}

void
CTclPixmap::
zoom(int scale)
{
  if (scale > 0)
    ++grid_size_;
  else
    --grid_size_;

  grid_size_ = std::min(std::max(grid_size_, 1), 64);

  canvas_->redraw();
}

void
CTclPixmap::
setGrid(int flag)
{
  if ((flag && grid_) || (! flag && ! grid_))
    return;

  grid_ = flag;

  canvas_->redraw();
}

void
CTclPixmap::
setFunction(const char *fname)
{
  if      (CStrUtil::casecmp(fname, "copy") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_COPY;
  else if (CStrUtil::casecmp(fname, "move") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_MOVE;
  else if (CStrUtil::casecmp(fname, "mark") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_MARK;
  else if (CStrUtil::casecmp(fname, "point") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_POINT;
  else if (CStrUtil::casecmp(fname, "curve") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_CURVE;
  else if (CStrUtil::casecmp(fname, "line") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_LINE;
  else if (CStrUtil::casecmp(fname, "rectangle") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_RECTANGLE;
  else if (CStrUtil::casecmp(fname, "filled_rectangle") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_FILLED_RECTANGLE;
  else if (CStrUtil::casecmp(fname, "circle") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_CIRCLE;
  else if (CStrUtil::casecmp(fname, "filled_circle") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_FILLED_CIRCLE;
  else if (CStrUtil::casecmp(fname, "flood_fill") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_FLOOD_FILL;
  else if (CStrUtil::casecmp(fname, "text") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_TEXT;
  else if (CStrUtil::casecmp(fname, "set_hot_spot") == 0)
    function_ = CTCL_PIXMAP_FUNCTION_SET_HOT_SPOT;
  else
    std::cerr << "Invalid function name " << fname << "\n";

  eval("gset ctcl_pixmap_function {%s}", fname);
}

void
CTclPixmap::
clearHotSpot()
{
  x_hot_ = -1;
  y_hot_ = -1;

  CImageXPMInst->setHotSpot(0, 0);

  canvas_->redraw();
}

void
CTclPixmap::
unmark()
{
  mark_ = CIRange2D(-1, -1, -1, -1);

  canvas_->redraw();

  image_->resetWindow();
}

void
CTclPixmap::
setFgColorNum(int num)
{
  fg_color_num_ = num;

  if (fg_color_num_ >= 0) {
    CRGBA color = image_->getColor(fg_color_num_);

    int r = int(color.getRed  ()*255);
    int g = int(color.getGreen()*255);
    int b = int(color.getBlue ()*255);
    int a = int(color.getAlpha()*255);

    if (a < 127)
      eval("CTclPixmapSetFgColor none");
    else
      eval("CTclPixmapSetFgColor #%02x%02x%02x", r, g, b);
  }
}

void
CTclPixmap::
setBgColorNum(int num)
{
  bg_color_num_ = num;

  if (bg_color_num_ >= 0) {
    CRGBA color = image_->getColor(bg_color_num_);

    int r = int(color.getRed  ()*255);
    int g = int(color.getGreen()*255);
    int b = int(color.getBlue ()*255);
    int a = int(color.getAlpha()*255);

    if (a < 127)
      eval("CTclPixmapSetBgColor none");
    else
      eval("CTclPixmapSetBgColor #%02x%02x%02x", r, g, b);
  }
}

void
CTclPixmap::
resizeImage(int width, int height)
{
  CImageNameSrc url("CTclPixmap/resizeImage");

  CImagePtr image = CImageMgrInst->createImage(url);

  image->setDataSize(width, height);

  image->subCopyFrom(image_, 0, 0, width, height);

  image_ = image;

  redraw();
}

void
CTclPixmap::
rescaleImage(int width, int height)
{
  CImagePtr image = image_->resize(width, height);

  image_ = image;

  redraw();
}

void
CTclPixmap::
setTextStr(const std::string &str)
{
  font_text_ = str;
}

void
CTclPixmap::
setTextFont(const std::string &name, CFontStyle style, int size, int angle)
{
  font_name_  = name;
  font_style_ = style;
  font_size_  = size;
  font_angle_ = angle;

  setTextFont();
}

void
CTclPixmap::
setTextFont()
{
  delete font_;

  Display *display = canvas_->getXDisplay();

  if (display == NULL)
    return;

  CFontPtr font = CFontMgrInst->lookupFont(font_name_, font_style_, font_size_);

  std::string xfont_name = font->getXFontName();

  font_ = new CXrtFont(display, xfont_name, font_angle_);
}

void
CTclPixmap::
addColor(const std::string &name)
{
  double r, g, b;

  CRGBName::lookup(name, &r, &g, &b);

  int ind = image_->addColor(CRGB(r, g, b));

  int r1 = int(r*255);
  int g1 = int(g*255);
  int b1 = int(b*255);

  eval("CTclPixmapAddColor #%02x%02x%02x %d", r1, g1, b1, ind);
}

void
CTclPixmap::
draw()
{
  if (canvas_ == NULL)
    return;

  if (font_ == NULL)
    setTextFont();

  int canvas_width  = canvas_->getWidth ();
  int canvas_height = canvas_->getHeight();

  int width  = image_->getWidth ();
  int height = image_->getHeight();

  eval("CTclPixmapSetTitle {CTclPixmap}");

  eval("CTclPixmapSetStatus {%s} {%d} {%d}", filename_.c_str(), width, height);

  canvas_->setBackground(CRGB(.7));

  canvas_->clear();

  int x, y;

  y = dy_;

  for (int j = 0; j < height; ++j, y += grid_size_) {
    if (y >= canvas_height || y + grid_size_ < 0)
      continue;

    x = dx_;

    for (int i = 0; i < width; ++i, x += grid_size_) {
      if (x >= canvas_width || x + grid_size_ < 0)
        continue;

      drawCanvasPoint(i, j, x, y);
    }
  }

  if (x_hot_ >= 0 && y_hot_ >= 0) {
    int px, py;

    windowToPixel(x_hot_, y_hot_, &px, &py);

    canvas_->setForeground(CRGB(0));

    canvas_->fillRectangle(px, py, grid_size_, grid_size_);
  }

  setScrollbars();
}

void
CTclPixmap::
drawCanvasPoint(int x, int y)
{
  int px, py;

  windowToPixel(x, y, &px, &py);

  drawCanvasPoint(x, y, px, py);
}

void
CTclPixmap::
drawCanvasPoint(int x, int y, int px, int py)
{
  CRGB rgb;

  int width  = image_->getWidth ();
  int height = image_->getHeight();

  if (x < 0 || x >= width || y < 0 || y >= height)
    return;

  if (! image_->isTransparent(x, y)) {
    image_->getRGBPixel(x, y, rgb);

    canvas_->setForeground(rgb);

    canvas_->fillRectangle(px, py, grid_size_, grid_size_);
  }
  else {
    rgb = CRGB(1);

    canvas_->setForeground(rgb);

    canvas_->fillRectangle(px, py, grid_size_, grid_size_);

    canvas_->drawSubAlphaImage(stipple_image_, 0, 0, px, py, grid_size_, grid_size_);
  }

  if (grid_ && grid_size_ > 4) {
    if (! image_->isTransparent(x, y)) {
      if (rgb.getGray() > 0.5)
        canvas_->setForeground(CRGB(0));
      else
        canvas_->setForeground(CRGB(1));
    }
    else
      canvas_->setForeground(CRGB(1));

    canvas_->drawRectangle(px, py, grid_size_, grid_size_);
  }
}

void
CTclPixmap::
drawCanvasLine(int x1, int y1, int x2, int y2)
{
  bresenham_canvas_->drawLine(x1, y1, x2, y2);
}

void
CTclPixmap::
drawThumbnail()
{
  if (thumbnail_canvas_ == NULL)
    return;

  thumbnail_canvas_->setBackground(CRGB(.7));

  thumbnail_canvas_->clear();

  int width  = image_->getWidth ();
  int height = image_->getHeight();

  double dx =  width/64.0;
  double dy = height/64.0;

  if (dx < 1) dx = 1;
  if (dy < 1) dy = 1;

  if (dx < dy)
    dx = dy;

  CRGB rgb;

  int    x, y;
  double xx, yy;

  yy = 0;
  y  = 0;

  for (int j = 0; j < 64 && j < height && y < height; ++j) {
    xx = 0;
    x  = 0;

    for (int i = 0; i < 64 && i < width && x < width; ++i) {
      x = int(xx);

      if (! image_->isTransparent(x, y)) {
        image_->getRGBPixel(x, y, rgb);

        thumbnail_canvas_->setForeground(rgb);
      }
      else
        thumbnail_canvas_->setForeground(CRGB(0.7));

      thumbnail_canvas_->drawPoint(i, j);

      xx += dx;

      x = int(xx);
    }

    yy += dx;

    y = int(yy);
  }
}

void
CTclPixmap::
drawOverlay()
{
  if (mark_.x1 >= 0 && mark_.y1 >= 0 && mark_.x2 >= 0 && mark_.y2 >= 0 ) {
    for (int i = mark_.x1; i <= mark_.x2; ++i) {
      for (int j = mark_.y1; j <= mark_.y2; ++j) {
        fillXorPixel(i, j);
      }
    }
  }
}

void
CTclPixmap::
fillXorPixel(int x, int y)
{
  int px, py;

  int width  = image_->getWidth ();
  int height = image_->getHeight();

  if (x < 0 || y < 0 || x >= width || y >= height)
    return;

  windowToPixel(x, y, &px, &py);

  canvas_->setForeground(CRGB(0));

  canvas_->fillXorRectangle(px, py, grid_size_, grid_size_);
}

void
CTclPixmap::
setScrollbars()
{
  double start, end;

  double width  = image_->getWidth ()*grid_size_;
  double height = image_->getHeight()*grid_size_;

  if (width < canvas_->getWidth())
    dx_ = 0;

  if (height < canvas_->getHeight())
    dy_ = 0;

  if (width > 0) {
    start = -dx_/width;
    end   = (-dx_ + canvas_->getWidth ())/width ;
  }
  else {
    start = 0.0;
    end   = 1.0;
  }

  start = std::min(std::max(start, 0.0  ), 1.0);
  end   = std::min(std::max(end  , start), 1.0);

  eval("CTclPixmapCanvasSetXScrollPercent %g %g", start, end);

  if (height > 0) {
    start = -dy_/height;
    end   = (-dy_ + canvas_->getHeight())/height;
  }
  else {
    start = 0.0;
    end   = 1.0;
  }

  start = std::min(std::max(start, 0.0  ), 1.0);
  end   = std::min(std::max(end  , start), 1.0);

  eval("CTclPixmapCanvasSetYScrollPercent %g %g", start, end);
}

void
CTclPixmap::
scrollToX(double x)
{
  double width = image_->getWidth()*grid_size_;

  if (width == 0)
    return;

  double max_x = 1.0 - canvas_->getWidth()/width;

  x = std::min(std::max(x, 0.0), max_x);

  dx_ = -int(x*width);

  canvas_->redraw();
}

void
CTclPixmap::
scrollByX(double x)
{
  dx_ -= int(x*canvas_->getWidth()/10);

  canvas_->redraw();
}

void
CTclPixmap::
scrollToY(double y)
{
  double height = image_->getHeight()*grid_size_;

  if (height == 0)
    return;

  double max_y = 1.0 - canvas_->getHeight()/height;

  y = std::min(std::max(y, 0.0), max_y);

  dy_ = -int(y*height);

  canvas_->redraw();
}

void
CTclPixmap::
scrollByY(double y)
{
  dy_ -= int(y*canvas_->getHeight()/10);

  canvas_->redraw();
}

void
CTclPixmap::
mousePress(int x, int y, int num)
{
  if      (function_ == CTCL_PIXMAP_FUNCTION_COPY) {
    int x1, y1;

    pixelToWindow(x, y, &x1, &y1);

    if (x1 >= mark_.xmin() && x1 <= mark_.xmax() &&
        y1 >= mark_.ymin() && y1 <= mark_.ymax()) {
      mark_motion_ = true;

      rect_ = mark_;

      mark_sx_ = x1; mark_sy_ = y1;
      mark_ex_ = x1; mark_ey_ = y1;
    }
    else {
      mark_motion_ = false;

      setXorRectStart(x, y);
    }

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_MOVE) {
    int x1, y1;

    pixelToWindow(x, y, &x1, &y1);

    if (x1 >= mark_.xmin() && x1 <= mark_.xmax() &&
        y1 >= mark_.ymin() && y1 <= mark_.ymax()) {
      mark_motion_ = true;

      rect_ = mark_;

      mark_sx_ = x1; mark_sy_ = y1;
      mark_ex_ = x1; mark_ey_ = y1;
    }
    else {
      mark_motion_ = false;

      setXorRectStart(x, y);
    }

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_MARK) {
    setXorRectStart(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_POINT) {
    int x1, y1;

    undo_->startGroup();

    pixelToWindow(x, y, &x1, &y1);

    addUndoImage(CIRange2D(x1, y1, x1, y1));

    drawPoint(x1, y1, num);

    drawCanvasPoint(x1, y1);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_CURVE) {
    int x1, y1;

    undo_->startGroup();

    pixelToWindow(x, y, &x1, &y1);

    addUndoImage(CIRange2D(x1, y1, x1, y1));

    drawPoint(x1, y1, num);

    drawCanvasPoint(x1, y1);

    last_x_ = x;
    last_y_ = y;
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_LINE) {
    setXorRectStart(x, y);

    drawXorLine();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_RECTANGLE) {
    setXorRectStart(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_FILLED_RECTANGLE) {
    setXorRectStart(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_CIRCLE) {
    setXorRectStart(x, y);

    drawXorCircle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_FILLED_CIRCLE) {
    setXorRectStart(x, y);

    drawXorCircle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_FLOOD_FILL) {
    int x1, y1;

    pixelToWindow(x, y, &x1, &y1);

    floodFill(x1, y1);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_TEXT) {
    setXorRectStart(x, y);

    drawXorText();
  }

  canvas_->flush();
}

void
CTclPixmap::
mouseMotion(int x, int y, int num)
{
  if      (function_ == CTCL_PIXMAP_FUNCTION_COPY) {
    drawXorRectangle();

    if (mark_motion_) {
      int x1, y1;

      pixelToWindow(x, y, &x1, &y1);

      int dx = x1 - mark_ex_;
      int dy = y1 - mark_ey_;

      rect_.inc(dx, dy);

      mark_ex_ = x1;
      mark_ey_ = y1;
    }
    else
      setXorRectEnd(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_MOVE) {
    drawXorRectangle();

    if (mark_motion_) {
      int x1, y1;

      pixelToWindow(x, y, &x1, &y1);

      int dx = x1 - mark_ex_;
      int dy = y1 - mark_ey_;

      rect_.inc(dx, dy);

      mark_ex_ = x1;
      mark_ey_ = y1;
    }
    else
      setXorRectEnd(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_MARK) {
    drawXorRectangle();

    setXorRectEnd(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_POINT) {
    int x1, y1;

    pixelToWindow(x, y, &x1, &y1);

    addUndoImage(CIRange2D(x1, y1, x1, y1));

    drawPoint(x1, y1, num);

    drawCanvasPoint(x1, y1);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_CURVE) {
    int x1, y1, x2, y2;

    pixelToWindow(last_x_, last_y_, &x1, &y1);
    pixelToWindow(     x ,      y , &x2, &y2);

    addUndoImage(CIRange2D(x1, y1, x2, y2));

    drawLine(x1, y1, x2, y2, num);

    drawCanvasLine(x1, y1, x2, y2);

    last_x_ = x;
    last_y_ = y;
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_LINE) {
    drawXorLine();

    setXorRectEnd(x, y);

    drawXorLine();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_RECTANGLE) {
    drawXorRectangle();

    setXorRectEnd(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_FILLED_RECTANGLE) {
    drawXorRectangle();

    setXorRectEnd(x, y);

    drawXorRectangle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_CIRCLE) {
    drawXorCircle();

    setXorRectEnd(x, y);

    drawXorCircle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_FILLED_CIRCLE) {
    drawXorCircle();

    setXorRectEnd(x, y);

    drawXorCircle();
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_TEXT) {
    drawXorText();

    setXorRectStart(x, y);

    drawXorText();
  }

  canvas_->flush();
}

void
CTclPixmap::
mouseRelease(int x, int y, int num)
{
  if      (function_ == CTCL_PIXMAP_FUNCTION_COPY) {
    drawXorRectangle();

    if (mark_motion_) {
      int x1, y1;

      pixelToWindow(x, y, &x1, &y1);

      int dx = x1 - mark_ex_;
      int dy = y1 - mark_ey_;

      rect_.inc(dx, dy);

      mark_ex_ = x1;
      mark_ey_ = y1;

      copyArea(mark_, rect_);
    }
    else
      setXorRectEnd(x, y);

    mark_ = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    image_->setWindow(mark_.x1, mark_.y1, mark_.x2, mark_.y2);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_MOVE) {
    drawXorRectangle();

    if (mark_motion_) {
      int x1, y1;

      pixelToWindow(x, y, &x1, &y1);

      int dx = x1 - mark_ex_;
      int dy = y1 - mark_ey_;

      rect_.inc(dx, dy);

      mark_ex_ = x1;
      mark_ey_ = y1;

      moveArea(mark_, rect_);
    }
    else
      setXorRectEnd(x, y);

    mark_ = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    image_->setWindow(mark_.x1, mark_.y1, mark_.x2, mark_.y2);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_MARK) {
    drawXorRectangle();

    setXorRectEnd(x, y);

    mark_ = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    image_->setWindow(mark_.x1, mark_.y1, mark_.x2, mark_.y2);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_POINT) {
    int x1, y1;

    pixelToWindow(x, y, &x1, &y1);

    addUndoImage(CIRange2D(x1, y1, x1, y1));

    undo_->endGroup();

    drawPoint(x1, y1, num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_CURVE) {
    int x1, y1, x2, y2;

    pixelToWindow(last_x_, last_y_, &x1, &y1);
    pixelToWindow(     x ,      y , &x2, &y2);

    addUndoImage(CIRange2D(x1, y1, x1, y1));

    undo_->endGroup();

    drawLine(x1, y1, x2, y2, num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_LINE) {
    drawXorLine();

    setXorRectEnd(x, y);

    CIRange2D rect = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    addUndoImage(rect);

    drawLine(rect_.x1, rect_.y1, rect_.x2, rect_.y2, num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_RECTANGLE) {
    drawXorRectangle();

    setXorRectEnd(x, y);

    CIRange2D rect = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    addUndoImage(rect);

    drawRectangle(rect_.xmin (), rect_.ymin (), rect_.xsize(), rect_.ysize(), num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_FILLED_RECTANGLE) {
    drawXorRectangle();

    setXorRectEnd(x, y);

    CIRange2D rect = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    addUndoImage(rect);

    fillRectangle(rect_.xmin (), rect_.ymin (), rect_.xsize(), rect_.ysize(), num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_CIRCLE) {
    drawXorCircle();

    setXorRectEnd(x, y);

    CIRange2D rect = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    addUndoImage(rect);

    drawCircle(rect_.x1, rect_.y1, rect_.x2, rect_.y2, num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_FILLED_CIRCLE) {
    drawXorCircle();

    setXorRectEnd(x, y);

    CIRange2D rect = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

    addUndoImage(rect);

    fillCircle(rect_.x1, rect_.y1, rect_.x2, rect_.y2, num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_TEXT) {
    drawXorText();

    setXorRectStart(x, y);

    drawText(rect_.xmin(), rect_.ymin(), font_text_, num);
  }
  else if (function_ == CTCL_PIXMAP_FUNCTION_SET_HOT_SPOT) {
    int x1, y1;

    pixelToWindow(x, y, &x1, &y1);

    x_hot_ = x1;
    y_hot_ = y1;

    CImageXPMInst->setHotSpot(x_hot_, y_hot_);
  }

  redraw();
}

void
CTclPixmap::
copyArea(const CIRange2D &from, const CIRange2D &to)
{
  int x = from.xmin();
  int y = from.ymin();
  int w = from.xsize();
  int h = from.ysize();

  CImagePtr image = getImage(x, y, w, h);

  x = to.xmin();
  y = to.ymin();

  drawImage(x, y, image);
}

void
CTclPixmap::
moveArea(const CIRange2D &from, const CIRange2D &to)
{
  int x = from.xmin();
  int y = from.ymin();
  int w = from.xsize();
  int h = from.ysize();

  CImagePtr image = getImage(x, y, w, h);

  clear(x, y, x + w - 1, y + h - 1);

  x = to.xmin();
  y = to.ymin();

  drawImage(x, y, image);
}

void
CTclPixmap::
drawXorRectangle()
{
  CIRange2D rect = CIRange2D(rect_.xmin(), rect_.ymin(), rect_.xmax(), rect_.ymax());

  int px1, py1, px2, py2;

  windowToPixel(rect.x1, rect.y1, &px1, &py1);
  windowToPixel(rect.x2, rect.y2, &px2, &py2);

  int w = px2 - px1 + 1;
  int h = py2 - py1 + 1;

  px1 += grid_size_/2;
  py1 += grid_size_/2;

  canvas_->setXorLineWidth(grid_size_);

  canvas_->drawXorRectangle(px1, py1, w, h);

  canvas_->flush();
}

void
CTclPixmap::
drawXorLine()
{
  bresenham_xor_->drawLine(rect_.x1, rect_.y1, rect_.x2, rect_.y2);

  canvas_->flush();
}

void
CTclPixmap::
drawXorCircle()
{
  int xc = rect_.x1;
  int yc = rect_.y1;

  int dx = rect_.xsize() - 1;
  int dy = rect_.ysize() - 1;

  int r = int(sqrt(dx*dx + dy*dy) + 0.5);

  bresenham_xor_->drawCircle(xc, yc, r);

  canvas_->flush();
}

void
CTclPixmap::
drawXorText()
{
  int width, ascent, descent;

  font_->textExtents(font_text_, &width, &ascent, &descent);

  font_->draw(xor_renderer_, rect_.xmin(), rect_.ymin() - ascent, font_text_);
}

void
CTclPixmap::
fillXorCircle()
{
  int xc = rect_.x1;
  int yc = rect_.y1;

  int dx = rect_.xsize() - 1;
  int dy = rect_.ysize() - 1;

  int r = int(sqrt(dx*dx + dy*dy) + 0.5);

  bresenham_xor_->fillCircle(xc, yc, r);

  canvas_->flush();
}

void
CTclPixmap::
drawLine(int x1, int y1, int x2, int y2, int num)
{
  renderer_->setColorNum(num);

  bresenham_->drawLine(x1, y1, x2, y2);
}

void
CTclPixmap::
drawCircle(int x1, int y1, int x2, int y2, int num)
{
  int xc = x1;
  int yc = y1;

  int dx = x2 - x1 - 1;
  int dy = y2 - y1 - 1;

  int r = int(sqrt(dx*dx + dy*dy) + 0.5);

  renderer_->setColorNum(num);

  bresenham_->drawCircle(xc, yc, r);
}

void
CTclPixmap::
fillCircle(int x1, int y1, int x2, int y2, int num)
{
  int xc = x1;
  int yc = y1;

  int dx = x2 - x1 - 1;
  int dy = y2 - y1 - 1;

  int r = int(sqrt(dx*dx + dy*dy) + 0.5);

  renderer_->setColorNum(num);

  bresenham_->fillCircle(xc, yc, r);
}

void
CTclPixmap::
drawRectangle(int x, int y, int w, int h, int num)
{
  int xs = x;
  int ys = y;
  int xe = x + w - 1;
  int ye = y + h - 1;

  for (x = xs; x <= xe; ++x) {
    drawPoint(x, ys, num);
    drawPoint(x, ye, num);
  }

  for (y = ys; y <= ye; ++y) {
    drawPoint(xs, y, num);
    drawPoint(xe, y, num);
  }
}

void
CTclPixmap::
fillRectangle(int x, int y, int w, int h, int num)
{
  int xs = x;
  int ys = y;
  int xe = x + w - 1;
  int ye = y + h - 1;

  for (x = xs; x <= xe; ++x)
    for (y = ys; y <= ye; ++y)
      drawPoint(x, y, num);
}

void
CTclPixmap::
drawText(int x, int y, const std::string &text, int num)
{
  int width, ascent, descent;

  font_->textExtents(font_text_, &width, &ascent, &descent);

  renderer_->setColorNum(num);

  font_->draw(renderer_, x, y - ascent, text);
}

void
CTclPixmap::
drawPixelPoint(int x, int y, int num)
{
  int x1, y1;

  pixelToWindow(x, y, &x1, &y1);

  drawPoint(x1, y1, num);
}

void
CTclPixmap::
drawPoint(int x, int y, int num)
{
  int width  = image_->getWidth ();
  int height = image_->getHeight();

  if (x < 0 || x >= width ) return;
  if (y < 0 || y >= height) return;

  if (image_->hasColormap()) {
    int color_num;

    if      (num == 1)
      color_num = fg_color_num_;
    else if (num == 3)
      color_num = bg_color_num_;
    else
      return;

    if (color_num >= 0) {
      if (! image_->setColorIndexPixel(x, y, color_num))
        return;
    }
  }
  else {
    CRGB color;

    if      (num == 1)
      color = fg_color_;
    else if (num == 3)
      color = bg_color_;
    else
      return;

    if (! image_->setRGBAPixel(x, y, color))
      return;
  }
}

void
CTclPixmap::
setXorRectStart(int x, int y)
{
  int x1, y1;

  pixelToWindow(x, y, &x1, &y1);

  rect_ = CIRange2D(x1, y1, x1, y1);
}

void
CTclPixmap::
setXorRectEnd(int x, int y)
{
  int x1, y1;

  pixelToWindow(x, y, &x1, &y1);

  rect_.x2 = x1;
  rect_.y2 = y1;
}

void
CTclPixmap::
pixelToWindow(int px, int py, int *wx, int *wy)
{
  *wx = (px - dx_)/grid_size_;
  *wy = (py - dy_)/grid_size_;
}

void
CTclPixmap::
windowToPixel(int wx, int wy, int *px, int *py)
{
  *px = wx*grid_size_ + dx_;
  *py = wy*grid_size_ + dy_;
}

void
CTclPixmap::
keyPress(const std::string &key)
{
  if      (key == "c")
    setFunction("circle");
  else if (key == "C")
    setFunction("filled_circle");
  else if (key == "g")
    setGrid(! grid_);
  else if (key == "l")
    setFunction("line");
  else if (key == "m")
    setFunction("mark");
  else if (key == "M")
    unmark();
  else if (key == "p")
    setFunction("point");
  else if (key == "P")
    setFunction("curve");
  else if (key == "r")
    setFunction("rectangle");
  else if (key == "R")
    setFunction("filled_rectangle");
  else if (key == "t")
    setFunction("text");
  else if (key == "u")
    undo();
  else if (key == "U")
    redo();
  else if (key == "z")
    zoom(1);
  else if (key == "Z")
    zoom(-1);
}

void
CTclPixmap::
addUndoImage()
{
  int x1, y1, x2, y2;

  image_->getWindow(&x1, &y1, &x2, &y2);

  addUndoImage(CIRange2D(x1, y1, x2, y2));
}

void
CTclPixmap::
addUndoImage(const CIRange2D &rect)
{
  int x = rect.xmin();
  int y = rect.ymin();
  int w = rect.xsize();
  int h = rect.ysize();

  CImagePtr image = getImage(x, y, w, h);

  undo_->addUndo(new CTclPixmapUndoImage(this, x, y, image));
}

CImagePtr
CTclPixmap::
getImage(int x, int y, int width, int height)
{
  return image_->subImage(x, y, width, height);
}

void
CTclPixmap::
drawImage(int x, int y, CImagePtr &image)
{
  image_->subCopyFrom(image, 0, 0, -1, -1, x, y);
}

bool
CTclPixmapLoadFile::
proc(int, const char **argv)
{
  pixmap_->loadFile(argv[1]);

  return true;
}

bool
CTclPixmapInsertFile::
proc(int, const char **argv)
{
  pixmap_->insertFile(argv[1]);

  return true;
}

bool
CTclPixmapSaveFile::
proc(int argc, const char **argv)
{
  if (argc > 1)
    pixmap_->saveFile(argv[1]);
  else
    pixmap_->saveFile();

  return true;
}

bool
CTclPixmapUndo::
proc(int, const char **)
{
  pixmap_->undo();

  return true;
}

bool
CTclPixmapRedo::
proc(int, const char **)
{
  pixmap_->redo();

  return true;
}

bool
CTclPixmapClear::
proc(int, const char **)
{
  pixmap_->clear();

  return true;
}

bool
CTclPixmapSet::
proc(int, const char **)
{
  pixmap_->set();

  return true;
}

bool
CTclPixmapInvert::
proc(int, const char **)
{
  pixmap_->invert();

  return true;
}

bool
CTclPixmapRedraw::
proc(int, const char **)
{
  pixmap_->redraw();

  return true;
}

bool
CTclPixmapScrollX::
proc(int, const char **argv)
{
  int offset = CStrUtil::toInteger(argv[1]);

  pixmap_->scrollX(offset);

  return true;
}

bool
CTclPixmapScrollY::
proc(int, const char **argv)
{
  int offset = CStrUtil::toInteger(argv[1]);

  pixmap_->scrollY(offset);

  return true;
}

bool
CTclPixmapFlip::
proc(int, const char **argv)
{
  char dir1 = argv[1][0];
  char dir2 = argv[1][1];

  if (dir1 == 'H') {
    if (dir2 == 'V')
      pixmap_->flipHV();
    else
      pixmap_->flipH();
  }
  else
    pixmap_->flipV();

  return true;
}

bool
CTclPixmapRotate::
proc(int, const char **argv)
{
  int angle = CStrUtil::toInteger(argv[1]);

  pixmap_->rotate(angle);

  return true;
}

bool
CTclPixmapZoom::
proc(int, const char **argv)
{
  int scale = CStrUtil::toInteger(argv[1]);

  pixmap_->zoom(scale);

  return true;
}

bool
CTclPixmapSetGrid::
proc(int, const char **argv)
{
  int flag = CStrUtil::toInteger(argv[1]);

  pixmap_->setGrid(flag);

  return true;
}

bool
CTclPixmapSetFunction::
proc(int, const char **argv)
{
  pixmap_->setFunction(argv[1]);

  return true;
}

bool
CTclPixmapClearHotSpot::
proc(int, const char **)
{
  pixmap_->clearHotSpot();

  return true;
}

bool
CTclPixmapUnmark::
proc(int, const char **)
{
  pixmap_->unmark();

  return true;
}

bool
CTclPixmapSetFgColorNum::
proc(int, const char **argv)
{
  int num = CStrUtil::toInteger(argv[1]);

  pixmap_->setFgColorNum(num);

  return true;
}

bool
CTclPixmapSetBgColorNum::
proc(int, const char **argv)
{
  int num = CStrUtil::toInteger(argv[1]);

  pixmap_->setBgColorNum(num);

  return true;
}

bool
CTclPixmapResize::
proc(int, const char **argv)
{
  int width  = CStrUtil::toInteger(argv[1]);
  int height = CStrUtil::toInteger(argv[2]);

  pixmap_->resizeImage(width, height);

  return true;
}

bool
CTclPixmapRescale::
proc(int, const char **argv)
{
  int width  = CStrUtil::toInteger(argv[1]);
  int height = CStrUtil::toInteger(argv[2]);

  pixmap_->rescaleImage(width, height);

  return true;
}

bool
CTclPixmapSetTextStr::
proc(int, const char **argv)
{
  pixmap_->setTextStr(argv[1]);

  return true;
}

bool
CTclPixmapSetTextFont::
proc(int, const char **argv)
{
  CFontStyle style = CFont::stringToFontStyle(argv[2]);

  int size = CStrUtil::toInteger(argv[3]);

  int angle = CStrUtil::toInteger(argv[4]);

  pixmap_->setTextFont(argv[1], style, size, angle);

  return true;
}

bool
CTclPixmapAddColorName::
proc(int, const char **argv)
{
  pixmap_->addColor(argv[1]);

  return true;
}

bool
CTclPixmapExit::
proc(int, const char **)
{
  exit(0);
}

bool
CTclPixmapUndoImage::
exec()
{
  CImagePtr image =
    pixmap_->getImage(x_, y_, image_->getWidth(), image_->getHeight());

  pixmap_->drawImage(x_, y_, image_);

  image_ = image;

  return true;
}
