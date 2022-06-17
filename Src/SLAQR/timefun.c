/*
#include <stdio.h>
main()
{
int yr1,mo1,dy1,hr1,mi1,sc1;
int yr2,mo2,dy2,hr2,mi2,sc2;
int jday1, jday2;
int i;
char  timestring[20];
sprintf(timestring,"2008-03-14T23:59:12");
printf("%s\n",timestring);
decode_time_string(timestring,&yr1,&mo1,&dy1,&hr1,&mi1,&sc1);
printf("%d %d %d  %d %d %d\n",yr1,mo1,dy1,hr1,mi1,sc1);
}


for(i=0;i<60;i++)
   {
   yr1=2003;
   jday1=360;
   hr1=14;
   mi1=32;
   sc1=11;
   sc2=i*3600;
   mi2=sc2/60;
   hr2=mi2/60;
   printf("%4.4d %3.3d %2.2d:%2.2d:%2.2d - %2.2d %2.2d %2.2d ",yr1,jday1,hr1,mi1,sc1,hr2,mi2,sc2);
   jday1+=addtime(&hr1,&mi1,&sc1,hr2,mi2,sc2);
   jul2cal(&yr1, &jday1, &mo1, &dy1);
   printf("%4.4d/%2.2d/%2.2d %3.3d %2.2d:%2.2d:%2.2d\n",yr1,mo1,dy1,jday1,hr1,mi1,sc1);
   }
}



int decode_time_string(char *time_char, int *yr, int *mo, int *dy, int *hr, int *mi, int *sc)
   {
   sscanf(time_char,"%d-%d-%2dT%2d:%2d:%2d",yr,mo,dy,hr,mi,sc);
   }
*/

int addtime(int *,int *,int *,int , int , int );
int jul2cal(int *, int *, int *, int *);
double dtime(int , int , int , int , float , int , int , int , int , float );
int ndays(int ,int ,int ,int );
int julmd(int ,int , int *, int *);
int julian(int ,int ,int );

int addtime(int *hr,int *mi,int *sc,int hr1, int mi1, int sc1)
   /* add/subtract time: changes times and returns change in days */
   {
   int dy;
   *sc += sc1;
   *mi += mi1 + (*sc)/60;
   *sc = *sc % 60;
   if( *sc < 0)
      {
      (*sc)+=60;
      (*mi)-=1;
      }
   *hr += hr1 + (*mi)/60;
   *mi= (*mi)%60;
   if( *mi < 0)
      {
      (*mi)+=60;
      (*hr)-=1;
      }
   dy = *hr / 24;
   *hr = (*hr) % 24;
   if( *hr < 0)
      {
      (*hr)+=24;
      dy-=1;
      }
   return dy;
   }

int jul2cal(int *yr, int *jday, int *mo, int *dy)
   /* converts julian date to calender date: adjusts year and julian date if julian date is outside range */
   {
   int newday;
   while((newday=julmd(*yr, *jday, mo, dy)) > 0)
      {
      *jday =newday-31;
      (*yr)++;
      } 
   if(*jday < 1)
      {
      while( *jday < 1)
         {
         (*yr)--;
         *jday += julian(*yr,12,31);
         }
      julmd(*yr,*jday,mo,dy);
      } 
   }

double dtime(int yr1, int jd1, int hr1, int mi1, float sc1,
             int yr2, int jd2, int hr2, int mi2, float sc2)
   /*computes difference (in seconds) between two dates */
   {
   double dif;
   dif=((ndays(yr1,jd1,yr2,jd2)*24.+hr2-hr1)*60.+mi2-mi1)*
         60.+sc2-sc1 ;
   return(dif);
   }

int ndays(int yr1,int jd1,int yr2,int jd2)
   /* computes number of days between julian dates */
   {
   int i,i1,i2,isign,nd;
   if(yr1 > yr2)
      {
      i1=yr2;
      i2=yr1;
      isign=-1;
      nd=jd1-jd2;
      }
   else
      {
      i1=yr1;
      i2=yr2;
      isign=1;
      nd=jd2-jd1;
      }
   for(i=i1;i<i2;i++) nd+=julian(i,12,31);
   nd=isign*nd;
   return(nd);
   }

int julmd(int yr,int dy, int *cmo, int *cdy)
   {
   /* update so that overflow of dy will return number of days left if jday is out of range*/
   static int days[2][12] = {
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
      {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
      };
   int i, j, k, leap;

   if(yr<100) yr+=1900;
   leap= yr%4 == 0 && yr%100 != 0 || yr%400 == 0;
   
   i=0;
   *cdy=dy;
   while((*cdy-=days[leap][i] ) > 0 && i < 11) i++;
   *cdy+=days[leap][i];
   *cmo=i+1;
   if(*cdy < 1)
      {
      return( (*cdy)-1);
      }
   else if( *cdy > 31 && *cmo == 12)
      {
      return((*cdy) - 31);
      }
   else
      {
      return(0);
      }
   }

int julian(int yr,int mo,int dy)
   /* comoute juian day from calendar date */
   {
   static int days[2][12] = {
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
      {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
      };
   int i, j, k, leap;

   if(yr<100) yr+=1900;
   leap= yr%4 == 0 && yr%100 != 0 || yr%400 == 0;

   for (i=1;i<mo;i++) dy += days[leap][i-1];
   return(dy);
   }
