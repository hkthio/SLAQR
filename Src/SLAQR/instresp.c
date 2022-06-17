#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dcmath.h"

int readsacpz(char *respfile, dcomplex **pz, int *ntot, float *gain)
   {
   FILE *fp;
   char type[80],line[80];
   int nel[2]={0,0},ipz,i,j;
   float dum1, dum2;

   printf("%s\n",respfile);
   if((fp=fopen(respfile,"r")) == NULL)
      {
      printf("Cannot open response file: %s\n",respfile);
      return -1;
      }
   while(fgets(line,80,fp) != NULL)
      {
      if(strchr(line,'p') !=NULL || strchr(line,'P') != NULL)
         {
         ipz=0;
         sscanf(line,"%s %d",type,(ntot+ipz));
         pz[ipz] = (dcomplex *) malloc(ntot[ipz]*sizeof(dcomplex));
         }
      else if(strchr(line,'z') !=NULL || strchr(line,'Z') != NULL)
         {
         ipz=1;
         sscanf(line,"%s %d",type,(ntot+ipz));
         pz[ipz] = (dcomplex *) malloc(ntot[ipz]*sizeof(dcomplex));
         }
      else if(strchr(line,'c') !=NULL || strchr(line,'C') != NULL)
         {
         sscanf(line,"%s %f",type,gain);
         }
      else
         {
         sscanf(line,"%f %f",&dum1,&dum2);
         pz[ipz][nel[ipz]].re=dum1;
         pz[ipz][nel[ipz]].im=dum2;
         nel[ipz]++;
         }
      }
   for(i=0;i<2;i++)
      {
      for(j=nel[i];j<ntot[i];j++)
         {
         pz[i][j].re=0.0;
         pz[i][j].im=0.0;
         }                                                             
      }
   }

dcomplex getresppz(float f, dcomplex **pz, int *npz, float gain)
   {
   dcomplex resp, om;
   int j;
   om.re=0.0;
   om.im=f*2.*3.1415927;
   resp.re=gain;
   resp.im=0.0;
   for(j=0;j<npz[0];j++)
      {
      resp=dcdiv(resp,dcdiff(om,pz[0][j]));
      }                                                             
   for(j=0;j<npz[1];j++)
      {
      resp=dcmult(resp,dcdiff(om,pz[1][j]));
      }                                                             
   return(resp);
   }
