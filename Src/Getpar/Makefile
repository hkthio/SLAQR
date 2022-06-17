CFLAGS=-O
#CFLAGS=-O -Xt -DENVIRONMENT

SOURCE = getpar.c getarg.c
CFILES = getpar.o getarg.o
FFILES = fgetpar.o fgetarg.o

get:	$(CFILES)
	-rm get
	ar r get $(CFILES)
	ar t get

fget:	$(FFILES)
	-rm fget
	ar r fget $(FFILES)
	ar t fget

fgetpar.o: getpar.c
	cp getpar.c fgetpar.c
	gcc $(CFLAGS) -DFORTRAN -c fgetpar.c
	rm fgetpar.c

fgetarg.o: getarg.c
	cp getarg.c fgetarg.c
	gcc $(CFLAGS) -DFORTRAN -c fgetarg.c
	rm fgetarg.c

install:
#	mv get /usr/lib/libget.a
#	mv fget /usr/lib/libfget.a
	mv get ../../Lib/libget.a
	mv fget ../../Lib/libfget.a

cleanup:
	rm get $(CFILES) fget $(FFILES)
list:
	imprint -2 $(SOURCE) Makefile

doc:
	itroff -man getpar.3
	itroff -man getarg.3
