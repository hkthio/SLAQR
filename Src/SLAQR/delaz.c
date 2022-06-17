#include <math.h>
delaz(ts,ps,tr,pr,dist,az,baz)
float ts,ps,tr,pr,*baz,*az,*dist;
/* back azimuth, azimuth and distance between source and receiver:
   source coordinate (ts,ps) : receiver coordinate (tr,pr)
   ts and tr are latitude while ps and pr are longitude,
   back azimuth (azimuth of source from receiver)
   azimuth (azimuth of receiver from source)
   They are all in degrees, measured clockwise from north*/
{
	int icase;
	float azm,bazm;
	double x,xd,dph,dis,sang,cang,rdph,rts,rtr;
	double conv=3.14159265358979/180.;

	rts=(90.-ts)*conv;
	rtr=(90.-tr)*conv;
	xd=pr-ps;
	if(xd < -360.) xd=xd+360.;
	if(xd > 360.) xd=xd-360.;
	if( xd > -360. && xd <= -180. )
	{
		icase = 1;
		dph=xd+360.;
	}
	if( xd > -180. && xd <= 0.0)
	{
		icase = 2;
		dph= -xd;
	}
	if( xd > 0.0 && xd <= 180. )
	{
		icase = 3;
		dph=xd;
	}
	if( xd > 180. && xd <= 360. )
	{
		icase = 4;
		dph=360.-xd;
	}

	rdph=dph*conv;
	x=cos(rts)*cos(rtr)+sin(rts)*sin(rtr)*cos(rdph);
	dis=acos(x);
	*dist=dis/conv;

	sang=sin(rts)*sin(rdph)/sin(dis);
	cang=(cos(rts)-x*cos(rtr))/(sin(dis)*sin(rtr));
	bazm=atan2(sang,cang)/conv;
	if(icase == 2 || icase == 4) bazm=  -bazm;

	sang=sin(rtr)*sin(rdph)/sin(dis);
	cang=(cos(rtr)-x*cos(rts))/(sin(rts)*sin(dis));
	azm=atan2(sang,cang)/conv;
	if(icase == 1 || icase == 3) azm= -azm;

	*baz = -1.* bazm;
	*az = -1. * azm;
}
