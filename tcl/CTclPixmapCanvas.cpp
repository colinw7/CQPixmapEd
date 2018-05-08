#include "std_c++.h"
#include "std_Xt.h"
#include "CTclCanvas/CTclCanvas.h"
#include "CTclPixmap/CTclPixmap.h"
#include "CTclPixmap/CTclPixmapCanvas.h"
#include "CStrUtil/CStrUtil.h"

CTclPixmapCanvasCmd::
CTclPixmapCanvasCmd(CTclPixmap *pixmap) :
 CTclAppCanvasCmd(pixmap), pixmap_(pixmap), canvas_(NULL)
{
  init();
}

CTclPixmapCanvasCmd::
~CTclPixmapCanvasCmd()
{
  delete canvas_;
}

CTclAppCanvas *
CTclPixmapCanvasCmd::
createInstance(CTclApp *)
{
  canvas_ = new CTclPixmapCanvas(pixmap_, false);

  canvas_->setForeground(CRGB(0.0));
  canvas_->setBackground(CRGB(1.0));

  pixmap_->addCanvas(canvas_);

  return (CTclAppCanvas *) canvas_->getCanvasWidget();
}

CTclPixmapThumbnailCanvasCmd::
CTclPixmapThumbnailCanvasCmd(CTclPixmap *pixmap) :
 CTclAppCanvasCmd(pixmap), pixmap_(pixmap), canvas_(NULL)
{
  init();
}

CTclPixmapThumbnailCanvasCmd::
~CTclPixmapThumbnailCanvasCmd()
{
  delete canvas_;
}

CTclAppCanvas *
CTclPixmapThumbnailCanvasCmd::
createInstance(CTclApp *)
{
  canvas_ = new CTclPixmapCanvas(pixmap_, true);

  canvas_->setForeground(CRGB(0.0));
  canvas_->setBackground(CRGB(1.0));

  pixmap_->addThumbnailCanvas(canvas_);

  return (CTclAppCanvas *) canvas_->getCanvasWidget();
}

CTclPixmapCanvas::
CTclPixmapCanvas(CTclPixmap *pixmap, bool thumbnail) :
 CTclCanvas(pixmap), pixmap_(pixmap), thumbnail_(thumbnail)
{
}

bool
CTclPixmapCanvas::
instanceCommand(int, const char **argv)
{
  if      (strcmp(argv[0], "xview") == 0) {
    if      (strcmp(argv[1], "moveto") == 0)
      pixmap_->scrollToX(CStrUtil::toReal(argv[2]));
    else if (strcmp(argv[1], "scroll") == 0)
      pixmap_->scrollByX(CStrUtil::toReal(argv[2]));
    else
      return false;
  }
  else if (strcmp(argv[0], "yview") == 0) {
    if      (strcmp(argv[1], "moveto") == 0)
      pixmap_->scrollToY(CStrUtil::toReal(argv[2]));
    else if (strcmp(argv[1], "scroll") == 0)
      pixmap_->scrollByY(CStrUtil::toReal(argv[2]));
    else
      return false;
  }
  else
    return false;

  return true;
}

bool
CTclPixmapCanvas::
instanceConfigure(int, const char **)
{
  return false;
}

void
CTclPixmapCanvas::
draw()
{
  if (! thumbnail_)
    pixmap_->draw();
  else
    pixmap_->drawThumbnail();
}

void
CTclPixmapCanvas::
drawOverlay()
{
  if (! thumbnail_)
    pixmap_->drawOverlay();
}

void
CTclPixmapCanvas::
mousePress()
{
  if (! thumbnail_) {
    mouse_x_   = getMouseX();
    mouse_y_   = getMouseY();
    mouse_num_ = getMouseButton();

    pixmap_->mousePress(mouse_x_, mouse_y_, mouse_num_);
  }
}

void
CTclPixmapCanvas::
mouseMotion()
{
  if (! thumbnail_) {
    int mouse_x   = getMouseX();
    int mouse_y   = getMouseY();
    int mouse_num = getMouseButton();

    if (mouse_x != mouse_x_ || mouse_y != mouse_y_)
      pixmap_->mouseMotion(mouse_x, mouse_y, mouse_num);

    mouse_x_   = mouse_x;
    mouse_y_   = mouse_y;
    mouse_num_ = mouse_num;
  }
}

void
CTclPixmapCanvas::
mouseRelease()
{
  if (! thumbnail_) {
    mouse_x_   = getMouseX();
    mouse_y_   = getMouseY();
    mouse_num_ = getMouseButton();

    pixmap_->mouseRelease(mouse_x_, mouse_y_, mouse_num_);
  }
}

void
CTclPixmapCanvas::
keyPress()
{
  if (! thumbnail_) {
    string key = getKeyStr();

    pixmap_->keyPress(key);
  }
}

void
CTclPixmapCanvas::
keyRelease()
{
}
