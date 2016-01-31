#include <CQPixmapEd.h>

#include <CQApp.h>
#include <iostream>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  CQPixmap *pixmap = new CQPixmap;

  pixmap->createCentralWidget();

  pixmap->createMenus    ();
  pixmap->createToolBars ();
  pixmap->createStatusBar();

  bool loaded = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-')
      std::cerr << "Invalid option '" << argv[i] << "'" << std::endl;
    else {
      pixmap->loadImage(argv[i]);

      loaded = true;
    }
  }

  if (! loaded)
    pixmap->newImage();

  pixmap->show();

  return app.exec();
}
