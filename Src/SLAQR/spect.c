#include <math.h>
#include "dcmath.h"
#define PI   3.141592653589793
#define TPI  6.283185307179586
#define FPI 12.566370614359172

/*   calculate Fourier spectrum   */
spect(sg,dt,omg,phase,x,ampl,nt,nfft)

double sg[];
float phase[],ampl[],omg[],dt;
int nt,nfft;
dcomplex x[];
{
   int cfft(dcomplex *, int , int );
   int i,j,nfft2;
   dcomplex res[2],fil;
   dcomplex iom;

   nfft2=nfft/2;

   for( i=0; i<nt; i++)         {
      x[i].re = sg[i]; 
      x[i].im = 0.;
   }
   for( i=nt; i<nfft; i++)      {
      x[i].re = x[i].im = 0.;
   }

   cfft( x,nfft,-1 );

   for( i=1; i<=nfft2; i++ )
   {
      x[i].re *= dt;  /* scaling */
      x[i].im *= dt;  /* scaling */
      omg[i] = TPI*((double)i)/(nfft*dt);
      phase[i]= atan2(x[i].im,x[i].re)*180./PI;
      ampl[i] = dmodu(x[i]);
   }

   x[0].re=x[0].im=phase[0]=ampl[0]=0.;

}

