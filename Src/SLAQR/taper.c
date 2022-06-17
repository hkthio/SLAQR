/*	tp cosine taper at the both end    */
#include <math.h>

#define PI  3.1415926535

taper(sg,n,tp)
double sg[];
float tp;
int n;
{
int nt,i,j,ltp;
double dummy;

ltp=tp*n;

for(i=0,j=n-1;i<ltp;i++,j--)
    {
    dummy = cos( (1.+(float)i/ltp)*PI/2. );
    sg[i] *= dummy*dummy;
    dummy = cos( (1.-(float)(n-1-j)/ltp)*PI/2. );
    sg[j] *= dummy*dummy;
    }
}
