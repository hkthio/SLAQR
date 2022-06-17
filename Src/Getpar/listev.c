#include	<stdio.h>
main(ac,av,ae)
int ac; char **av, **ae;
   {
	int i;
	int testav, testae;
	testav= (unsigned int)(av);
	testae= (unsigned int)(ae);
	if( testae != (testav + 4 + 4*ac)) fprintf(stdout, "not OK\n"); 
	 else fprintf(stdout,"OK\n");
	fprintf(stdout,"ac=%d av=%x ae=%x\n",ac,av,ae);
	for(i=0; i<ac; i++) fprintf(stdout,"av[%d]= %x\n",i,av[i]);
	i=0;
	while(ae[i]) { fprintf(stdout,"ae[%d]= %x\n",i,ae[i]); i++; }
   }
