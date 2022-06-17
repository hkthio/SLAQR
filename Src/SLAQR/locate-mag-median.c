#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "dcmath.h"
#include "Sphere.h"
#include "timefun.h"
#define DtR  .017453293
#define maxfft  1500
#define nt 10
#define maxdata 20000
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
   int h,i,j,k,kk,l,wtype,m;
   int np;
   int fd;
   char sta[8],stnm[maxsta][8];
   FILE *fp,*fp1,*fp2;

   /*input variables*/
   char t_start_char[19], t_end_char[19];

   /*xt variables*/
   float xor0,tor0,***xta,**dist;
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
   float ampl[maxfft],phase[maxfft],omg[maxfft],tw1,tw2;
   float tdelt,delta,t0,width,f;
   double ws[maxfft],wndw[maxfft];
   float T[nt],om0[nt],ww,***uta,umax[nt];
   dcomplex c[maxfft];

   /*x,y,t variables*/
   struct cubesphere cub;
   struct geographic *geo;
   int ncub,ncreg,ngrid,maxgrid,ll;
   float **F,*xm,*ym,*max;
   float Med[maxsta],Median;
   float *x,*y,dis,az,baz,maxall;
   float *cumul,dcub,cub0;
   int *ixm,*iym,imaxall,dims[2],icube;
   float **G;

   /*t,T variables*/
   float *vv,Tmax,Tmin,Tdelt,tlag;
   float tstart, u[nt],uu[100],Tu[100];

   /* timeslice variable */
   int skip, target, nslice,iskip,nsta1,nstamax;
   char gridout[80],stackout[80];
   int imed,nmed,true_med=0;
   float medrat;

   /* response variables */
   int ntot[2],ipz,idx,idc,ivel;
   char respfile[80],resp_path[80];
   float gain, amp[nt];
   dcomplex **pz, resp;

   /* previous results variables */
   int *yr9, *jday9, *mo9, *dy9, *hr9, *mn9, *sc9;
   float *la9,*lo9,*ma9,*cu9,*am9,*mw9;
   int *n9,np9,ic9,iow,nhead,iprev,nmax9=10000;
   char prev_file[120],head[10][120],cdum[120];

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
      sscanf(c_span,"%d%s",&idum,&unit);
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
            printf("D or d - days   \n",unit);
            printf("H or h - hours  \n",unit);
            printf("M or m - minutes\n",unit);
            printf("S or s - seconds\n",unit);
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

   /* existing output file        */
   /* iow = -1 - don't overwrite  */
   /* iow =  0 - overwite         */
   /* iow =  1 - add data to file */
   iow=0;
   iprev=0;
   iout=0;
   iprev=getpar("prev_file","s",prev_file);
   getpar("overwrite_out","d",&iow);
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
   getpar("ncub","d",&ncub);
   getpar("output_amp","d",&iout_amp);
   /*remove decon flag */
/* getpar("decon","d",&idc);*/
   endpar();

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
      tor[i]=tor0+i*tstep;
      ampt0[i]=0.0;
      }

/*  ---------- read and interpolate group velocities  ---------- */
   k=0;
   i=0;
   fp=fopen("groupvel","r");
   while(fscanf(fp,"%f %f",(Tu+i),(uu+i))==2) i++;
   fclose(fp);
   printf("%d group velocities read\n",i);

   fp=fopen("o_groupvel","w");
   for(i=0;i<nt;i++) 
      {
      j=0;
      T[i]=Tmin+i*Tdelt;
      while(Tu[j]>T[i]) j++;
      u[i]=uu[j-1]+(uu[j]-uu[j-1])*(T[i]-Tu[j-1])/(Tu[j]-Tu[j-1]);
      fprintf(fp,"%7.3f %5.3f\n",T[i],u[i]);
      }
   fclose(fp);

/*  ------------ setup the grid ----------------------------------*/
   maxgrid=ncreg*ncub*ncub;
   geo  =(struct geographic *) malloc(maxgrid*sizeof(struct geographic));
   dist =(float **) malloc(maxgrid*sizeof(float *));
   G    =(float **) malloc(maxgrid*sizeof(float *));
   x    =(float *)  malloc(maxgrid*sizeof(float));
   y    =(float *)  malloc(maxgrid*sizeof(float));
   cumul=(float *)  malloc(maxgrid*sizeof(float));
   F    =(float **) malloc(ntor*sizeof(float *));
   for(i=0;i<ntor;i++) 
      {
      F[i]=(float *) malloc(maxgrid*sizeof(float));
      }
 
   fp=fopen("o_grid","w");
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
   fclose(fp);

            
   printf("Grid setup: %d %d\n",ngrid,maxgrid);


/* ---------- initialize xta ---------- */
   omax=(float *) malloc(maxsta*sizeof(float ));
   ys=(float *) malloc(maxsta*sizeof(float ));
   xs=(float *) malloc(maxsta*sizeof(float ));
   nodata=(int **) malloc(maxsta*sizeof(int *));
   xta=(float ***) malloc(maxsta*sizeof(float **));
   uta=(float ***) malloc(maxsta*sizeof(float **));
   for(i=0;i<maxsta;i++)
      {
      xta[i]=(float **) malloc(ntor*sizeof(float *));
      uta[i]=(float **) malloc(nv*sizeof(float *));
      nodata[i]=(int *) calloc(nv,sizeof(int));
      for(j=0;j<nv;j++)
         {
         uta[i][j]= (float *) malloc(nt*sizeof(float));
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
      G[j]=(float *) malloc(nsta*sizeof(float));
      }

/* ---------- read time info ---------- */
/* removed since start time is read in from par file
   printf("Reading time info from %s\n",sacfile[0]);
   fd=open(sacfile[0],O_RDONLY,0);
   read(fd,fhd,70*4);
   read(fd,ihd,40*4);
   read(fd,chd,24*8);
   close(fd);

   dt0=fhd[0];
   b0=fhd[5];
   e0=fhd[6];
   yr0=ihd[0];
   dy0=ihd[1];
   hr0=ihd[2];
   mn0=ihd[3];
   sc0=ihd[4];
   ms0=ihd[5];
 */
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
         }
   
      dif=dtime(yr1, jday1, hr1, mn1, sc1+ms1/1000., 
                   yr0, jday0, hr0, mn0, sc0+ms0/1000.);
  
      istart=dif/dt1;
      printf("Time difference = %8.2f %d\n",dif,istart);
/*
      if(abs(dif) > tolerance)
         {
         printf("Time difference exceeds tolerance: %s\n",sacfile[k]);
         }
*/
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
                  spect(ws,dt,omg,phase,c,ampl,nwp,nfft);
	          i=0;
	          while(omg[i] < om0[j]) i++;

	          ww=ampl[i-1]+(om0[j]-omg[i-1])*
	  	       (ampl[i]-ampl[i-1])/(omg[i]-omg[i-1]);
                  uta[k][h][j]=ww;
	          if(uta[k][h][j] > umax[j]) umax[j] = uta[k][h][j];
                  }
	       else 
                  {
                  uta[k][h][j]=-1.;
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
                        xta[k][m][l]+=uta[k][j][i]/(T[i]*amp[i]);
                        }
                     else
                        {
                        xta[k][m][l]+=uta[k][j][i]/(60.*amp[i]); /*keep 60 sec cal*/
                        }
                     ampt+=uta[k][j][i]/amp[i];
                     ni++;
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
   np9=0;
   if(iprev==1)
      {
      if((fp1=fopen(prev_file,"r")) != NULL)
         {
         fgets(head[0],80,fp1);
         if(sscanf(head[0],"%d",&nhead) >0)
            {
            for(k=1;k<nhead;k++)
               {
               fgets(head[k],80,fp1);
               printf("%s\n",head[k]);
               }
            yr9=(int *) malloc(nmax9*sizeof(int));
            mo9=(int *) malloc(nmax9*sizeof(int));
            jday9=(int *) malloc(nmax9*sizeof(int));
            dy9=(int *) malloc(nmax9*sizeof(int));
            hr9=(int *) malloc(nmax9*sizeof(int));
            mn9=(int *) malloc(nmax9*sizeof(int));
            sc9=(int *) malloc(nmax9*sizeof(int));
            la9=(float *) malloc(nmax9*sizeof(float));
            lo9=(float *) malloc(nmax9*sizeof(float));
            ma9=(float *) malloc(nmax9*sizeof(float));
            cu9=(float *) malloc(nmax9*sizeof(float));
            am9=(float *) malloc(nmax9*sizeof(float));
            mw9=(float *) malloc(nmax9*sizeof(float));
            n9=(int *) malloc(nmax9*sizeof(int));
           
            k=0;
            while(fscanf(fp1,"%4d%2d%2d%2d%2d%2d %s %f %f %f %f %f %f %d", 
                              (yr9+k),(mo9+k),(dy9+k),(hr9+k),(mn9+k),(sc9+k), cdum, 
                              (la9+k),(lo9+k),(ma9+k),(cu9+k),(am9+k),(mw9+k),(n9+k)) == 14)
               {
               jday9[k]=julian(yr9[k],mo9[k],dy9[k]);
               k++;
               }
            fclose(fp1);
            np9=k;
            printf("%d slices read\n",np9);

            fp1=fopen(out_file,"w");
            for(k=0;k<nhead;k++)
               {
               fputs(head[k],fp1);
               }
            
            ic9=0;
            while(dtime(*yr9, *jday9, *hr9, *mn9, *sc9, yr0, jday0, hr0, mn0, sc0) > 0 && ic9 <np9) 
               {
               fprintf(fp1,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d ",*yr9,*mo9,*dy9,*hr9,*mn9,*sc9); 
               fprintf(fp1,"%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d ",*yr9,*mo9,*dy9,*hr9,*mn9,*sc9);
               fprintf(fp1,"%8.3f %8.3f %12.5e %12.5e %12.5e %4.2f %3.3d\n",*la9,*lo9,*ma9,*cu9,*am9,*mw9,*n9);
               yr9++;jday9++;mo9++;dy9++;hr9++;mn9++;sc9++; la9++;lo9++;ma9++;cu9++;am9++;mw9++;n9++; ic9++;
               }
            }
         else
         /* incorrect header - create new header */
            {
            fp1=fopen(out_file,"w");
            fprintf(fp1,"2\n");
            fprintf(fp1,"%02d%02d%02d%02d%02d%02d\n",yr0-2000,cmo,cdy,hr0,mn0,sc0);
            iprev=0;
            }
         }
      else
         {
         /* file does not exist - create file */
         iow=0;
         fp1=fopen(out_file,"w");
         fprintf(fp1,"2\n");
         fprintf(fp1,"%02d%02d%02d%02d%02d%02d\n",yr0-2000,cmo,cdy,hr0,mn0,sc0);
         iprev=0;
         }
      }
   else
      {
/*    sprintf(stackout,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d.%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d.stackamp",
                                             yr0,mo0,dy0,hr0,mn0,sc0,yr2,mo2,dy2,hr2,mn2,sc2);*/
      fp1=fopen(out_file,"w");
      julmd(yr0,dy0,&cmo,&cdy);
      fprintf(fp1,"2\n");
      fprintf(fp1,"%02d%02d%02d%02d%02d%02d\n",yr0-2000,cmo,cdy,hr0,mn0,sc0);
      }
           
          
   if(iout_amp==1)  fp2=fopen("o_amp","w");

   ayr=yr0;
   ajday=jday0;
   ahr=hr0;
   amn=mn0;
   asc=sc0;
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
            if(m<nxor && xta[l][k][m] > 0.0) 
               {
               G[j][l]=xta[l][k][m];
               F[k][j]+=xta[l][k][m];
               Med[ll]=xta[l][k][m];
/*             printf("%d %d %d %12.6e %12.6e\n",ll,k,m,xta[l][k][m],Med[ll]);*/
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
               if(medrat < 10. && medrat > .1)
                  {
                  Median+=Med[l];
                  nmed++;
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
            F[k][j]=Median/1.25;
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
      if(iout_amp==1)
         {
         fprintf(fp2,"> %4.4d%2.2d%2.2d%2.2d%2.2d%2.2d \n",ayr,amo,ady,ahr,amn,asc);
         for(l=0;l<nsta;l++)  fprintf(fp2,"%8.2f %12.6e %s\n",dist[ixm[k]][l],G[ixm[k]][l],stnm[l]);
         }
      if(iprev==1)
         {
         while(dtime(*yr9, *jday9, *hr9, *mn9, *sc9, ayr, ajday, ahr, amn, asc) > 0 && ic9 <np9) 
            {
            fprintf(fp1,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d ",*yr9,*mo9,*dy9,*hr9,*mn9,*sc9); 
            fprintf(fp1,"%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d ",*yr9,*mo9,*dy9,*hr9,*mn9,*sc9);
            fprintf(fp1,"%8.3f %8.3f %12.5e %12.5e %12.5e %4.2f %3.3d\n",*la9,*lo9,*ma9,*cu9,*am9,*mw9,*n9);
            yr9++;jday9++;mo9++;dy9++;hr9++;mn9++;sc9++; la9++;lo9++;ma9++;cu9++;am9++;mw9++;n9++;ic9++;
            }
         }
      fprintf(fp1,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d ",ayr,amo,ady,ahr,amn,asc); 
      fprintf(fp1,"%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d ",ayr,amo,ady,ahr,amn,asc);
      fprintf(fp1,"%8.3f %8.3f %12.5e %12.5e %12.5e %4.2f %3.3d %3.3d\n",
              geo[ixm[k]].lon,geo[iym[k]].lat,max[k],cumul[k],
              ampt0[k],.75*log10(max[k])+10.6,nstat[k],nstamax);
      if(iskip==0) fclose(fp);
      }

      while(ic9 <np9) 
         {
         fprintf(fp1,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d ",*yr9,*mo9,*dy9,*hr9,*mn9,*sc9); 
         fprintf(fp1,"%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d ",*yr9,*mo9,*dy9,*hr9,*mn9,*sc9);
         fprintf(fp1,"%8.3f %8.3f %12.5e %12.5e %12.5e %4.2f %3.3d\n",*la9,*lo9,*ma9,*cu9,*am9,*mw9,*n9);
         yr9++;jday9++;mo9++;dy9++;hr9++;mn9++;sc9++; la9++;lo9++;ma9++;cu9++;am9++;mw9++;n9++;ic9++;
         }

   if(iout_amp==1) fclose(fp2);

   fclose(fp1);
   tor[imaxall]-=tstart;
   t_hr=tor[imaxall]/3600;
   t_mn=(tor[imaxall]-t_hr*3600)/60;
   t_sc=(tor[imaxall]-t_hr*3600-t_mn*60);
   printf("Location: lon=%7.2f, lat=%7.2f, mag=%4.2f, time=%02d:%02d:%04.1f\n",
	   xm[imaxall],ym[imaxall],.75*log10(maxall)+10.6,t_hr,t_mn,t_sc);
   return;
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
