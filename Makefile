all: patch.exe

hdrgen:
	gcc -Wall -O2 -g -o hdrgen hdrgen.c

patch: patcher.c pdata.h
	gcc -Wall -Os -o patch patcher.c pdata.h
	strip -s patch

patch.exe: patcher.c pdata.h
	i586-mingw32msvc-gcc -Wall -Os -o patch.exe patcher.c pdata.h
	i586-mingw32msvc-strip -s patch.exe

clean:
	rm -f hdrgen patch patch.exe
