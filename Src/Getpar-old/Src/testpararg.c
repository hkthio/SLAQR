#include	<stdio.h>
main(ac,av)
int ac; char **av;
   {
	float x[10];
	int lx;
	int junk;

	setpar(ac,av);
	getpar("junk","d",&junk);
	endpar();
	lx=10;

	fprintf(stdout,"call with NULL\n");
	sub(x,lx,NULL);

	fprintf(stdout,"call with 0 len string\n");
	sub(x,lx,"");

	fprintf(stdout,"call with options\n");
	sub(x,lx,"opt1=100 fopt2=2.0 title='new title' noboo y=1,3*5,6,7,8*9");
   }
sub(x,lx,list)
float *x;
int lx;
char *list;
   {
	int opt1, boo;
	float fopt2;
	char title[40];
	int y[20], n, i;

	opt1=10;
	fopt2= 5.0;
	title[0]= '\0';
	boo= 1;

	setarg(list,"sub");
	getarg("opt1","d",&opt1);
	getarg("fopt2","f",&fopt2);
	getarg("title","s",title);
	getarg("boo","b",&boo);
	n=getarg("y","vd[20]",y);
	endarg();

	fprintf(stdout,"opt1=%d fopt2=%6.2f title=%s boo=%d\n",
		opt1,fopt2,title,boo);
	fprintf(stdout,"y= ");
	for(i=0; i<n; i++) fprintf(stderr,"%3d ",y[i]);
	fprintf(stderr,"\n");
   }
