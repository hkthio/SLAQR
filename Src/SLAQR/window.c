/*Blackman-Harris(4),Blackman(3),Hann(ing)(1) and Hamming(2) windows */
#include <math.h>

#define PI   3.1415926535
#define TPI  6.2831853070
#define FPI 12.5663706140

window(sg,n,type)
double sg[];
int n,type;
   {
   int i,j,k;
   double a,b,c;
   if(type < 5)
      {
      a=0.5;
      b=0.5;
      c=0.0;
      if(type == 2)
         {
         a=0.54;
         b=0.46;
         }
      if(type == 3)
         {
         a=0.42;
         c=0.08;
         }
      if(type == 4)
         {
         a=0.42323;
         b=0.49755;
         c=0.07922;
         }
      for(i=0;i<n;i++) 
         {
         *(sg+i)=a-b*cos(TPI*i/(n-1))+c*cos(FPI*i/(n-1));
         if(sg[i]<0.0) sg[i]=0.0;
         }
      }
   if(type == 5)
      {
      k=n/20;
      for(i=0;i<n;i++)
	 {
	 *(sg+i)=1.;
	 if(i<= k) *(sg+i)=0.5-0.5*cos(i*TPI/(2*k));
	 if(i> n-1-k) *(sg+i)=0.5+0.5*cos((i-(n-1-k))*TPI/(2*k));
	 }
      }
   }
