all:
	cd src; qmake; make

clean:
	cd src; make clean
	rm src/Makefile
	rm bin/CQPixmapEd
