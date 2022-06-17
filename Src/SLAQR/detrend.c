detrend(sg,n,type)
double sg[];
int n,type;
   {
   int i;
   double a,b,n0,mean,trend,an;
   a=b=trend=0.0;
   an=(double)n;
   for(i=0;i<n;i++)
      {
      a+=sg[i];
      b+=a;
      }
   n0=0.5*(an+1.0);
   mean=a/an;
   if(type==1) trend=-12.*(b-n0*a)/(an*(an*an-1.0));
   for(i=0;i<n;i++) *(sg+i)-=mean-trend*((double)i-n0);
   }
