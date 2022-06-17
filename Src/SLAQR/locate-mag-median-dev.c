#include "../Getpar/libpar.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "dcmath.h"
#include "locate.h"
#include "Sphere.h"
#include "timefun.h"
#define DtR  .017453293
#define maxfft  1500
#define nt 10
#define maxdata 900000
#define maxsta  50
#define perms 0666
#define ELEM_SWAP(a,b) { register float t=(a);(a)=(b);(b)=t; }

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* USGS surface wave locator                                                  */
/*                                                                            */
/*   - version based om locate-mag-v061.c                                     */
/*                                                                            */
/* History:                                                                   */
/*  2008/08/05 changed output for new plotting scripts                        */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

int readsacpz(char *,dcomplex **, int *, float *);
dcomplex getresppz(float, dcomplex **, int *, float );
float quick_select(float *, int);
int quickSort(float *, int ) ;

int main(int ac, char **av)
   {
   /*general variables*/
   int h,i,j,k,kk,l,wtype,m,n,is;
   int np;
   int fd;
   char sta[8],stnm[maxsta][8];
   FILE *fp,*fp1,*fp2,*fp3;

   /*input variables*/
   char t_start_char[19], t_end_char[19];

   /*xt variables*/
   float xor0,tor0,***xta,**dist,**azim,**bazi;
   float *xor,*tor,dxor,tstep,*omax;
   float b,e,dt,*xs,*ys,ampt,*ampt0,amps;
   int idis,iort,nxor,ntor,ni,istart,**nodata;
   int *nstat;

   /*sac variables*/
   int ihd[40],nsta;
   char chd[24][8],list[80],sacfile[maxsta][80];
   float fhd[70],sg[maxdata];

   /*time variables*/
   float dt0, b0, e0;
   float dt1, b1, e1;
   /* yr0 etc is date/time of begiing of furst timeslice */
   /* yr1 etc is date/time of individual seismograms     */
   int yr0, jday0, mo0, dy0, hr0, mn0, sc0, ms0;
   int yr1, jday1, mo1, dy1, hr1, mn1, sc1, ms1;
   int yr2, jday2, mo2, dy2, hr2, mn2, sc2, ms2;
   int ayr, ajday, amo, ady, ahr, amn, asc, ams;
   int cmo, cdy,idum;
   float dif,tolerance,span;
   int t_hr, t_mn;
   float t_sc;
   char c_span[10],unit[1];

   /*fft variables*/
   int nwp,n1,n2,nfft,nv;
   float ampl[maxfft],phase_uw[maxfft],phase[maxfft],omg[maxfft],tw1,tw2;
   float tdelt,delta,t0,width,f,dphase;
   double ws[maxfft],wndw[maxfft];
   float T[nt],om0[nt],ww,***uta,***utp,umax[nt];
   dcomplex cs[maxfft];

   /*x,y,t variables*/
   struct cubesphere cub;
   struct geographic *geo;
   int ncub,ncreg,ngrid,maxgrid,ll;
   float lat0,lon0,lat1,lon1,dlat,dlon;
   int nlat,nlon;
   float **F,*xm,*ym,*max;
   float Med[maxsta],Median;
   float *x,*y,dis,az,baz,maxall;
   float *cumul,dcub,cub0;
   int *ixm,*iym,imaxall,dims[2],icube;
   float **G;

   /*t,T variables*/
   float *vv,Tmax,Tmin,Tdelt,tlag;
   float tstart, c[nt],u[nt],cc[100],uu[100],Tu[100];

   /* timeslice variable */
   int skip, target, nslice,iskip,nsta1,nstamax,nsta_g;
   char gridout[80],stackout[80],specout[80];
   int imed,nmed,true_med=0,ista[100],ista_g[100];
   float medrat;

   /* response variables */
   int ntot[2],ipz,idx,idc,ivel;
   char respfile[80],resp_path[80];
   float gain, amp[nt], pha[nt];
   dcomplex **pz, resp;

   /* output variables */
   char out_file[120],iout, amp_out[120];
   int iout_amp=0;

   pz = (dcomplex **) malloc(2*sizeof (dcomplex *));
   idc=1;
   ms0=0;
   /* default values */
   sprintf(resp_path,".");
   icube=1;

   setpar(ac,av);

   /* first and last timeslices, and slice step */
   mstpar("step","f",&tstep);
   mstpar("start","s",t_start_char);
   sscanf(t_start_char,"%d/%d/%2d-%2d:%2d:%2d",&yr0,&mo0,&dy0,&hr0,&mn0,&sc0);
   printf("%s\n",t_start_char);
   printf("%d %d %d %d %d %d\n",yr0,mo0,dy0,hr0,mn0,sc0);
   jday0=julian(yr0,mo0,dy0);
   printf("%d %d %d %d %d %d  %d\n",yr0,mo0,dy0,hr0,mn0,sc0,jday0);
   if(getpar("end","s",t_end_char) !=0)
      {
      sscanf(t_end_char,"%d/%d/%2d-%2d:%2d:%2d",&yr2,&mo2,&dy2,&hr2,&mn2,&sc2);
      jday2=julian(yr2,mo2,dy2);
      span=dtime(yr0,jday0,hr0,mn0,sc0,yr2,jday2,hr2,mn2,sc2);
      }
   else
      {
      idum=-999;
      *unit='s';
      mstpar("span","s",c_span);
      sscanf(c_span,"%d%s",&idum,unit);
      yr2=yr0;
      mo2=mo0;
      dy2=dy0;
      hr2=hr0;
      mn2=mn0;
      sc2=sc0;
      jday2=jday0;
      if(idum == -999)
         {
         printf("Invalid timespan: %s\n",c_span);
         exit(-1);
         }
      else
         {
         if(*unit == 'h' || *unit == 'H')
            {
            span=idum*3600;
            }
         else if(*unit == 'm' || *unit == 'M')
            {
            span=idum*60;
            }
         else if(*unit == 's' || *unit == 'S')
            {
            span=idum;
            }
         else if(*unit == 'd' || *unit == 'D')
            {
            span=idum*24*3600;
            }
         else 
            {
            printf("Invalid unit: %s\n",unit);
            printf("D or d - days   \n");
            printf("H or h - hours  \n");
            printf("M or m - minutes\n");
            printf("S or s - seconds\n");
            exit(-1);
            }
         jday2+=addtime(&hr2,&mn2,&sc2,0,0,span);
         jul2cal(&yr2,&jday2,&mo2,&dy2);
         }
      }
   printf("%d %d %d %d %d %d  %d\n",yr2,mo2,dy2,hr2,mn2,sc2,jday2);
   ntor=span/tstep;
   printf("Number of timeslices: %d\n",ntor);;

   getpar("list","s",list);
   /* periods and window width (times period) and window shape */
   getpar("maxperiod","f",&Tmax);
   getpar("minperiod","f",&Tmin);
   getpar("w","f",&width);
   getpar("window","d",&wtype);

   /* distance cut-off and step */
   getpar("xor0","f",&xor0);
   getpar("dxor","f",&dxor);
   getpar("nxor","d",&nxor);

   iout=0;
   iout=mstpar("out_file","s",out_file);
   
   /* lag time */
   getpar("tlag","f",&tlag);
 
   /* misc parameters */
   getpar("median","d",&true_med);
   getpar("resp_path","s",resp_path);
   getpar("velocity","d",&ivel);
   getpar("nslice","d",&nslice);
   getpar("target","d",&target);
   getpar("ncreg","d",&ncreg);
   getpar("icube","d",&icube);
   if(icube==0)
      {
      mstpar("latmax","f",&lat1);
      mstpar("lonmax","f",&lon1);
      mstpar("latmin","f",&lat0);
      mstpar("lonmin","f",&lon0);
      mstpar("dlat","f",&dlat);
      mstpar("dlon","f",&dlon);
      nlon=1+(lon1-lon0)/dlon;
      nlat=1+(lat1-lat0)/dlat;
      printf("Geographic grid: nlon= %d, nlat=%d\n",nlon,nlat);
      }
   getpar("ncub","d",&ncub);
   getpar("output_amp","d",&iout_amp);
   endpar();
   printf("tlag= %f\n",tlag);

   nslice=ntor;
   if(target <=0)
      {
      skip=ntor/nslice;
      }
   else
      {
      skip=target;
      }
  
   tolerance=10.;
   tstart=0.0;
   /* number of timewindows (per trace) equals number of timeslices + number of slices to cover lag times */
   nv=ntor+tlag/tstep;
   
   /* decode time info */
   Tdelt=(Tmax-Tmin)/nt;
   xor=(float *) malloc(nxor*sizeof(float));
   ampt0=(float *) malloc(ntor*sizeof(float));
   nstat=(int *) calloc(ntor,sizeof(int));
   tor=(float *) malloc(ntor*sizeof(float));
   max=(float *) malloc(ntor*sizeof(float));
   xm=(float *) malloc(ntor*sizeof(float));
   ym=(float *) malloc(ntor*sizeof(float));
   vv=(float *) malloc(nv*sizeof(float));
   ixm=(int *) malloc(ntor*sizeof(int));
   iym=(int *) malloc(ntor*sizeof(int));


   for(i=0;i<nv;i++) vv[i]=tstart+i*tstep;
   for(i=0;i<nxor;i++) xor[i]=xor0+i*dxor;
   for(i=0;i<ntor;i++) 
      {
      tor[i]=tstart+i*tstep;
      ampt0[i]=0.0;
      }

/*  ---------- read and interpolate group velocities  ---------- */
   k=0;
   i=0;
   fp=fopen("SLAQR-dispersion.par","r");
   while(fscanf(fp,"%f %f %f",(Tu+i),(cc+i),(uu+i))==3) i++;
   fclose(fp);
   printf("%d group and phase velocities read\n",i);

   fp=fopen("o_groupvel","w");
   for(i=0;i<nt;i++) 
      {
      j=0;
      T[i]=Tmin+i*Tdelt;
      while(Tu[j]>T[i]) j++;
      u[i]=uu[j-1]+(uu[j]-uu[j-1])*(T[i]-Tu[j-1])/(Tu[j]-Tu[j-1]);
      c[i]=cc[j-1]+(cc[j]-cc[j-1])*(T[i]-Tu[j-1])/(Tu[j]-Tu[j-1]);
      fprintf(fp,"%7.3f %5.3f %5.3f\n",T[i],c[i],u[i]);
      }
   fclose(fp);

/*  ------------ setup the grid ----------------------------------*/
   fp=fopen("o_grid","w");
   if(icube == 1)
      {
      maxgrid=ncreg*ncub*ncub;
      geo  =(struct geographic *) malloc(maxgrid*sizeof(struct geographic));

      l=0;
      dcub=PI/(2*ncub);
      cub0=dcub/2.-PI/4.;
      for(i=0;i<ncreg;i++) 
         {
         cub.reg=i+1;
         for(j=0;j<ncub;j++) 
            {
            cub.xi=cub0+j*dcub;
            for(k=0;k<ncub;k++) 
               {
               cub.eta=cub0+k*dcub;
               geo[l]=sphtogeo(cubtosph(cub));
               fprintf(fp,"%12.5f %12.5f\n",geo[l].lon,geo[l].lat);
               l++;
               }
            }
         }
      ngrid=l;
      }
   else
      /* geographic box */
      {
      l=0;
      maxgrid=nlon*nlat;
      geo  =(struct geographic *) malloc(maxgrid*sizeof(struct geographic));
      for(j=0;j<nlat;j++)
         {
         for(k=0;k<nlon;k++)
            {
            geo[l].lon=lon0+k*dlon;
            geo[l].lat=lat0+j*dlat;
            fprintf(fp,"%12.5f %12.5f\n",geo[l].lon,geo[l].lat);
            l++;
            }
         }
      ngrid=l;
      }
   fclose(fp);
   printf("Grid setup: %d %d\n",ngrid,maxgrid);

   dist =(float **) malloc(maxgrid*sizeof(float *));
   azim =(float **) malloc(maxgrid*sizeof(float *));
   bazi =(float **) malloc(maxgrid*sizeof(float *));
   G    =(float **) malloc(maxgrid*sizeof(float *));
   x    =(float *)  malloc(maxgrid*sizeof(float));
   y    =(float *)  malloc(maxgrid*sizeof(float));
   cumul=(float *)  malloc(maxgrid*sizeof(float));
   F    =(float **) malloc(ntor*sizeof(float *));
   for(i=0;i<ntor;i++) 
      {
      F[i]=(float *) malloc(maxgrid*sizeof(float));
      }
 


/* ---------- initialize xta ---------- */
   omax=(float *) malloc(maxsta*sizeof(float ));
   ys=(float *) malloc(maxsta*sizeof(float ));
   xs=(float *) malloc(maxsta*sizeof(float ));
   nodata=(int **) malloc(maxsta*sizeof(int *));
   xta=(float ***) malloc(maxsta*sizeof(float **));
   uta=(float ***) malloc(maxsta*sizeof(float **));
   utp=(float ***) malloc(maxsta*sizeof(float **));
   for(i=0;i<maxsta;i++)
      {
      xta[i]=(float **) malloc(ntor*sizeof(float *));
      uta[i]=(float **) malloc(nv*sizeof(float *));
      utp[i]=(float **) malloc(nv*sizeof(float *));
      nodata[i]=(int *) calloc(nv,sizeof(int));
      for(j=0;j<nv;j++)
         {
         uta[i][j]= (float *) malloc(nt*sizeof(float));
         utp[i][j]= (float *) malloc(nt*sizeof(float));
         }
      for(j=0;j<ntor;j++)
         {
         xta[i][j]= (float *) malloc(nxor*sizeof(float));
         }
      }

/* ---------- read station list ------- */

   k=0;
   fp=fopen(list,"r");
   while(fscanf(fp,"%s",sacfile[k])!=EOF && k< maxsta) k++;
   nsta=k;
   fclose(fp);

   for(j=0;j<ngrid;j++)
      {
      dist[j]=(float *) malloc(nsta*sizeof(float));
      azim[j]=(float *) malloc(nsta*sizeof(float));
      bazi[j]=(float *) malloc(nsta*sizeof(float));
      G[j]=(float *) malloc(nsta*sizeof(float));
      }

/* ---------- read sac-files ---------- */

   fp1=fopen("o_stations","w");
   k=0;
   for(kk=0;kk<nsta;kk++)
      {
      printf("Reading %s\n",sacfile[kk]);
      fd=open(sacfile[kk],O_RDONLY,0);
      read(fd,fhd,70*4);
      read(fd,ihd,40*4);
      read(fd,chd,24*8);
      np=ihd[9];
      if(np>maxdata) 
         {
         printf("Too many datapoints: %d\n",np);
         }
      else
         {
         printf("%d datapoints\n",np);
         }
      read(fd,sg,ihd[9]*4);
      close(fd);

      dt1=fhd[0];
      b1=fhd[5];
      e1=fhd[6];
      delta=fhd[50];
      ys[k]=fhd[31];
      xs[k]=fhd[32];
      yr1=ihd[0];
      jday1=ihd[1];
      hr1=ihd[2];
      mn1=ihd[3];
      sc1=ihd[4];
      ms1=ihd[5];

      /* get absoute time of beginning of trace, and correct for day skip */
      printf("%d %d %d %d %d\n",yr1,jday1,hr1,mn1,sc1);
      jday1+=addtime(&hr1,&mn1,&sc1, 0,0,(int) b1);
      jul2cal(&yr1,&jday1,&mo1,&dy1);
      printf("%d %d %d %d %d\n",yr1,jday1,hr1,mn1,sc1);

      for(j=0;j<ngrid;j++)
         {
         delaz(geo[j].lat,geo[j].lon,ys[k],xs[k],&dis,&az,&baz);
         dist[j][k]=dis;
         azim[j][k]=az ;
         bazi[j][k]=baz;
         }
   
      dif=dtime(yr1, jday1, hr1, mn1, sc1+ms1/1000., 
                   yr0, jday0, hr0, mn0, sc0+ms0/1000.);
  
      istart=dif/dt1;
      printf("Time difference = %8.2f %d\n",dif,istart);
      if(np>maxdata) printf("Too many datapoints\n");

      printf("%f\n",fhd[20]);
      /* response file name */
      strncpy(sta,chd[0],8);
      strncpy(stnm[k],chd[0],8);
      idx=strchr(sta,' ')-sta;
      sta[idx]='\0';
      stnm[k][4]='\0';
      sprintf(respfile,"%s/SAC_PZs_%s_LHZ_00",resp_path,sta);
      printf("%s\n",respfile);
/*    if(idc) readsacpz(respfile, pz, ntot, &gain);*/
      if(fhd[20] <  -12344.0000 && readsacpz(respfile, pz, ntot, &gain)!=-1) 
         {
         b=b1;
         e=e1;
         dt=dt1;


         /* ---------- begin loop over periods ---------- */
         for(j=0;j<nt;j++)
            {
            umax[j]=0.;
            nfft=1;
            om0[j]=TPI/T[j];
            nwp=T[j]*width/dt;
            window(wndw,nwp,wtype);
            for(i=0;i<nwp;i++) wndw[i]=sqrt(wndw[i]);
            if(idc)
               {
               resp=getresppz(1./T[j],pz,ntot,gain);
               amp[j]=dmodu(resp);
               pha[j]=atan2(resp.im,resp.re);
               }
            else
               {
               amp[j]=1.;
               }
            while(nfft<nwp)nfft*=2;

            /*----------- begin loop over timewindows ------*/
            /*  t0 - center of timewindow                   */
            /*                                              */
            /*----------------------------------------------*/

            for(h=0;h<nv;h++)
               {
               t0=tstart+h*tstep;
               tdelt=0.5*T[j]*width;
               tw1=t0-tdelt;
               tw2=t0+tdelt;
               n1=istart+tw1/dt;
               n2=istart+tw2/dt;
	       if(n1 >=0 && n2 < np) 
	          {
                  for(i=0;i<nwp;i++) ws[i]=sg[n1+i];
	          detrend(ws,nwp,1);
                  for(i=0;i<nwp;i++) ws[i]*=wndw[i];
	          detrend(ws,nwp,1);
                  for(i=0;i<nwp;i++) ws[i]*=wndw[i];
                  spect(ws,dt,omg,phase,cs,ampl,nwp,nfft);
	          i=0;
	          while(omg[i] < om0[j]) i++;
	          ww=ampl[i-1]+(om0[j]-omg[i-1])*
	  	       (ampl[i]-ampl[i-1])/(omg[i]-omg[i-1]);
                  uta[k][h][j]=ww/amp[j];
                  /* phase unwrapping */
                  dphase=0.0;
                  phase_uw[0]=phase[0];
                  for(m=1;m<nwp;m++)
                     {
                     if(phase[m] -phase[m-1] > 90.) dphase+=360.;
                     phase_uw[m]=phase[m]-dphase;
                     }
	          ww=phase_uw[i-1]+(om0[j]-omg[i-1])*
	  	       (phase_uw[i]-phase_uw[i-1])/(omg[i]-omg[i-1]);
                  utp[k][h][j]=PI*ww/180. - pha[j]+om0[j]*tdelt;
	          if(uta[k][h][j] > umax[j]) umax[j] = uta[k][h][j];
                  }
	       else 
                  {
                  uta[k][h][j]=-1.e-20;
                  nodata[k][h]=-1;
                  }
               }
	    }

         /* map from (t,T) space to (xor,tor) space */
         omax[k]=0.0;
         for(m=0;m<ntor;m++)
            {
            amps=0.0;
            for(l=0;l<nxor;l++)
	       {
               ni=0;
               ampt=0.0;
	       for(i=0;i<nt;i++)
	          {
	          j=0.5+(xor[l]/u[i]+tor[m]-tstart)/tstep;
                  if(nodata[k][j]!=-1) nstat[m]++;
	          if(j>=0 && j<nv && (xor[l]/u[i])<tlag) 
                     {
/* normalized        xta[k][m][l]+=uta[k][j][i]/umax[i];*/
                     if(ivel == 1) 
                        {
                        xta[k][m][l]+=uta[k][j][i]/(T[i]);
                        }
                     else
                        {
                        xta[k][m][l]+=uta[k][j][i]/60.; /*keep 60 sec cal*/
                        }
                     ampt+=uta[k][j][i];
                     ni++;
                     /*printf("%d %d %d %g\n",k,m,l,xta[k][m][l]);*/
                     }
	          }
               if(ni > 0)
                  {
                  xta[k][m][l]/=ni;
                  ampt+=ampt/ni;
                  }

               if(ampt > amps) amps=ampt;
	       if(xta[k][m][l]>omax[k])
	          {
	          idis=l;
	          iort=m;
	          omax[k]=xta[k][m][l];
	          }
	       }
            ampt0[m]+=amps/nsta;
            }
         printf("Distance to %d: %7.2f, origin time: %7.2f, amplitude %f\n"
	      ,k,xor[idis],tor[iort],omax[k]);
         fprintf(fp1,"Distance to %d: %7.2f, origin time: %7.2f, amplitude %f\n"
	      ,k,xor[idis],tor[iort],omax[k]);
         free(pz[0]);
         free(pz[1]);
         printf("Done station %s\n",sta);
         k++;
         }
      else
         {
         printf("Skipped trace: %s\n",sacfile[kk]);
         }
      }
   close(fp1);
   nsta=k;

   maxall=.0;

   /* prepare the stackamp outputfile */
   fp1=fopen(out_file,"w");
   julmd(yr0,dy0,&cmo,&cdy);
   fprintf(fp1,"2\n");
   fprintf(fp1,"%02d%02d%02d%02d%02d%02d\n",yr0-2000,cmo,cdy,hr0,mn0,sc0);
          
   if(iout_amp==1)  fp2=fopen("o_amp","w");

   ayr=yr0;
   ajday=jday0;
   ahr=hr0;
   amn=mn0;
   asc=sc0;
   fp3=fopen("o_SLAQR","w");
   for(k=0;k<ntor;k++)
      {
      nstat[k]/=(nt*nxor);
      ajday+=addtime(&ahr,&amn,&asc,0,0,tstep);
      jul2cal(&ayr,&ajday,&amo,&ady);
      iskip=k%skip;
      if(iskip==0) 
         {
         sprintf(gridout,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d.xyz",ayr,amo,ady,ahr,amn,asc); 
         fp=fopen(gridout,"w");
         fprintf(fp,"2\n");
/*       fprintf(fp,"%02d%02d%02d%02d%02d%02d\n",yr0-2000,cmo,cdy,hr0,mn0,sc0);*/
         fprintf(fp,"%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d ",ayr,amo,ady,ahr,amn,asc);
         fprintf(fp,"%d %06d\n", ngrid, (int) tor[k]);
         }
      max[k]=0.0;
      printf("Working on tor: %d %f\n",k,tor[k]);
      for(j=0;j<ngrid;j++)
	 {
         ll=0;
	 for(l=0;l<nsta;l++)
	    {
            G[j][l]=-1.;
	    m=(dist[j][l]-xor0)*111.1/dxor;
/*          printf("%d %g %g\n",m,dist[j][l],xta[l][k][m]);*/
            if(m<nxor && xta[l][k][m] > 0.0) 
               {
               G[j][l]=xta[l][k][m];
               F[k][j]+=xta[l][k][m];
               Med[ll]=xta[l][k][m];
               ista[ll]=l;
/*             printf("%d %d %d %d %12.6e %12.6e\n",ll,k,m,nxor,xta[l][k][m],Med[ll]); */
               ll++;
               }
	    }
         nsta1=ll;
/*       Median=quick_select(Med,nsta1);*/
         quickSort(Med,nsta1);
/*       printf("%12.6e %12.6e %12.6e\n",Med[0],Med[nsta1/2],Med[nsta1-1]);*/

         Median=0.0;
         if(true_med==0)
            {
            for(l=0;l<nsta1-1;l++) Median+=Med[l];
            Median/=(nsta1-1);
            }
         else
            {
            imed=nsta1/2;
            nmed=0;
            for(l=0;l<nsta1;l++) 
               {
               medrat=Med[l]/Med[imed];
               if(medrat < 100. && medrat > .01)
                  {
                  Median+=Med[l];
                  nmed++;
                  }
               else
                  {
                  ista[l]=-1;
                  }
               }
            Median/=nmed;
/*          for(l=2;l<nsta1-2;l++) Median+=Med[l];
            Median/=(nsta1-4);*/
/*          imed=nsta1/2;
            Median=(Med[imed]+Med[imed+1])/2.;*/
            }
/*       Temporary fix to correct scaling*/
         if(true_med== 0)
            {
            F[k][j]=Median/3.50;
            }
         else
            {
            F[k][j]=Median/3.5 ;
            }
/*       F[k][j]=Median/4.8;*/
	 if(F[k][j]>max[k])
	    {
            nstamax=nsta1;
	    max[k]=F[k][j];
	    xm[k]=geo[j].lon;
	    ym[k]=geo[j].lat;
	    ixm[k]=j;
	    iym[k]=j;
            nsta_g=nsta1;
            for(l=0;l<nsta1;l++)
               {
               ista_g[l]=ista[l];
               }
	    if(max[k]>maxall)
	       {
	       maxall=max[k];
	       imaxall=k;
	       }
	    }
         if(iskip==0) fprintf(fp,"%12.5f %12.5f %12.5e %3d\n", 
                               geo[j].lon,geo[j].lat,F[k][j],nsta1);
         cumul[k]+=F[k][j];
	 }
      /* -- End loop over grid -- */
      if(iout_amp==1)
         {
         fprintf(fp2,"> %4.4d%2.2d%2.2d%2.2d%2.2d%2.2d \n",ayr,amo,ady,ahr,amn,asc);
         for(l=0;l<nsta;l++)  fprintf(fp2,"%8.2f %12.6e %s\n",dist[ixm[k]][l],G[ixm[k]][l],stnm[l]);
         }
/*    write out the spectra for the max amp */
      sprintf(specout,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d.spc",ayr,amo,ady,ahr,amn,asc);
      fp3=fopen(specout,"w");

      fprintf(fp3,"> %4.4d%2.2d%2.2d%2.2d%2.2d%2.2d \n",ayr,amo,ady,ahr,amn,asc);
      fprintf(fp3,"%8.3f %7.3f 15. %4d %3d %2d %2d %2d\n",geo[ixm[k]].lat,geo[ixm[k]].lon,ayr,amo,ady,ahr,amn);
      for(l=0;l<nsta_g;l++)
         {
         if(ista_g[l] >= 0)
            {
            is=ista_g[l];
            fprintf(fp3,"%4s 1 1  %8.2f   %7.2f      0.0   1  %2d\n",stnm[is],111.*dist[ixm[k]][is],azim[ixm[k]][is],nt);
            for(m=0;m<nt;m++)
               {
               ll=0.5+(111.*dist[ixm[k]][is]/u[m]+tor[k]-tstart)/tstep;
               dphase=(ll*tstep -tor[k]-tstart)*om0[m];
               fprintf(fp3,"%6.2f %12.6e %12.6e %5.3f\n",T[m],uta[is][ll][m]*1.e6,utp[is][ll][m]-dphase,u[m]);
               }
            }
         }
      fclose(fp3);

      /* print new results to stackamp file */
      fprintf(fp1,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d ",ayr,amo,ady,ahr,amn,asc); 
      fprintf(fp1,"%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d ",ayr,amo,ady,ahr,amn,asc);
      fprintf(fp1,"%8.3f %8.3f %12.5e %12.5e %12.5e %4.2f %3.3d %3.3d\n",
              geo[ixm[k]].lon,geo[iym[k]].lat,max[k],cumul[k],
              ampt0[k],.75*log10(max[k])+10.6,nstat[k],nstamax);
      if(iskip==0) fclose(fp);
      }

   if(iout_amp==1) fclose(fp2);

   fclose(fp1);
   tor[imaxall]-=tstart;
   t_hr=tor[imaxall]/3600;
   t_mn=(tor[imaxall]-t_hr*3600)/60;
   t_sc=(tor[imaxall]-t_hr*3600-t_mn*60);
   printf("Location: lon=%7.2f, lat=%7.2f, mag=%4.2f, time=%02d:%02d:%04.1f\n",
	   xm[imaxall],ym[imaxall],.75*log10(maxall)+10.6,t_hr,t_mn,t_sc);
   return 1;
   }




float quick_select(float arr[], int n) 
{
    int low, high ;
    int median;
    int middle, ll, hh;

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return arr[median] ;

        if (high == low + 1) {  /* Two elements only */
            if (arr[low] > arr[high])
                ELEM_SWAP(arr[low], arr[high]) ;
            return arr[median] ;
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    ELEM_SWAP(arr[middle], arr[high]) ;
    if (arr[low] > arr[high])       ELEM_SWAP(arr[low], arr[high]) ;
    if (arr[middle] > arr[low])     ELEM_SWAP(arr[middle], arr[low]) ;

    /* Swap low item (now in position middle) into position (low+1) */
    ELEM_SWAP(arr[middle], arr[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
        break;

        ELEM_SWAP(arr[ll], arr[hh]) ;
    }

    /* Swap middle item (in position low) back into correct position */
    ELEM_SWAP(arr[low], arr[hh]) ;

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
}


/*  quickSort
//
//  This public-domain C implementation by Darel Rex Finley.
//
//  * Returns YES if sort was successful, or NO if the nested
//    pivots went too deep, in which case your array will have
//    been re-ordered, but probably not sorted correctly.
//
//  * This function assumes it is called with valid parameters.
//
//  * Example calls:
//    quickSort(&myArray[0],5); // sorts elements 0, 1, 2, 3, and 4
//    quickSort(&myArray[3],5); // sorts elements 3, 4, 5, 6, and 7
*/
int quickSort(float *arr, int elements) {

  #define  MAX_LEVELS  1000

  int  beg[MAX_LEVELS], end[MAX_LEVELS], i=0, L, R ;
  float piv;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; if (i==MAX_LEVELS-1) return 0;
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; }
    else {
      i--; }}
  return 1  ; }
