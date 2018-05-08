#include "CTclApp/CTclAppCanvas.h"
#include "CTclCanvas/CTclCanvas.h"

class CTclPixmap;
class CTclPixmapCanvas;

class CTclPixmapCanvasCmd : public CTclAppCanvasCmd {
 private:
  CTclPixmap       *pixmap_;
  CTclPixmapCanvas *canvas_;

 public:
  CTclPixmapCanvasCmd(CTclPixmap *pixmap);
 ~CTclPixmapCanvasCmd();

  std::string getCommandName () const { return "CTclPixmapCanvas"; }
  std::string getCommandClass() const { return "CTclPixmapCanvas"; }

  CTclAppCanvas *createInstance(CTclApp *app);
};

class CTclPixmapThumbnailCanvasCmd : public CTclAppCanvasCmd {
 private:
  CTclPixmap       *pixmap_;
  CTclPixmapCanvas *canvas_;

 public:
  CTclPixmapThumbnailCanvasCmd(CTclPixmap *pixmap);
 ~CTclPixmapThumbnailCanvasCmd();

  std::string getCommandName () const { return "CTclPixmapThumbnailCanvas"; }
  std::string getCommandClass() const { return "CTclPixmapThumbnailCanvas"; }

  CTclAppCanvas *createInstance(CTclApp *app);
};

class CTclPixmapCanvas : public CTclCanvas {
 private:
  CTclPixmap *pixmap_;
  int         mouse_x_, mouse_y_, mouse_num_;
  bool        thumbnail_;

 public:
  CTclPixmapCanvas(CTclPixmap *pixmap, bool thumbnail);

 protected:
  bool instanceCommand(int argc, const char **argv);
  bool instanceConfigure(int argc, const char **argv);

  void draw();
  void drawOverlay();

  void mousePress();
  void mouseMotion();
  void mouseRelease();
  void keyPress();
  void keyRelease();
};
