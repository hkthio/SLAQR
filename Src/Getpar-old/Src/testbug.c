#include	<stdio.h>
int nx	=13;
float dx	=0.01;
float dy;
char title[40]	="Nothing here";
char options[128];
int map	=1;
int boo	=1;
int vec[10];
main(ac,av,ae)
int ac; char **av, **ae;
   {
	int i, n, nopt;
	char *optlist[10];
	setpar(ac,av);
	getpar("nx","d",&nx);
	getpar("map","b",&map);
	getpar("dx","f",&dx);
	getpar("title","s",title);
	getpar("options","s",options);
	getpar("boo","b",&boo);
	n= getpar("vec","vd[6]",vec);
	fprintf(stderr,"n= %2d vec=",n);
	for(i=0; i<n; i++) fprintf(stderr,"%3d, ",vec[i]);
	fprintf(stderr,"\n");
	endpar();
	fprintf(stderr,"programe now executes\n");
	fprintf(stderr,"nx=%d dx=%f map=%d title=%s\n",nx,dx,map,title);
	fprintf(stderr,"boo=%d\n",boo);
	fprintf(stderr,"options= %s\n",options);
   }
