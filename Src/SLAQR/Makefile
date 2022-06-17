CFLAGS = -g -lm -O -L../../Lib
OBJ1 = delaz.o spect.o taper.o cfft.o detrend.o window.o instresp.o timefun.o
OBJ3 = dcops.o Sphere.c
CC = gcc
F77 = gfortran

.c.o:	
	${CC} $*.c ${CFLAGS} -c
.f.o:
	${F77} $*.f ${FFLAGS} -c


all: locate-mag-median-dev 

locate-mag-median-dev: locate-mag-median-dev.o $(OBJ1) $(OBJ3) instresp.o
	${CC} locate-mag-median-dev.o -o locate-mag-median-dev -lget $(OBJ1) $(CFLAGS) $(OBJ3) 

addtime: addtime.o timefun.o
	${CC} addtime.o timefun.o -o addtime

clean:
	rm *.o

install:
	mv locate-median-dev ../../Bin
