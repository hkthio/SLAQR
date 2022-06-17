#include	<stdio.h>
main(ac,av)
int ac; char **av;
   {
	char file[40];
	int fd;
	setpar(ac,av);
	getpar("file","s",file);
	endpar();
	fd= open(file,0,0);
	fprintf(stderr,"fd=%d  file=%s\n",fd,file);
   }
