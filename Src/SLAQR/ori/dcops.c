#include <math.h>
#include "dcmath.h"

/*typedef struct { double re; double im;}  dcomplex;*/

dcomplex dcmult(a,b)		 /* complex multiplication routine. */
dcomplex a,b;
{
dcomplex c;

	c.re = (a.re * b.re) - (a.im * b.im);
	c.im = (a.re * b.im) + (a.im * b.re);
	return(c);

} /*end of cmult */

dcomplex dconj(a)			 /* complex conjugation routine */
dcomplex a;
{
dcomplex abar;

	abar.re = a.re;
	abar.im = -a.im;
	return(abar);

} /*  end conj   */

dcomplex dcdiv(a,b)		 /* complex division routine */
dcomplex a,b;

{
dcomplex c,temp;
/*dcomplex dconj(),dcmult(),dscadiv();*/
   
	temp = dconj(b);
	c = dcmult(a,temp);
	temp.re = b.re*b.re + b.im*b.im;
	temp.im = 0.e0;
	c = dscadiv(c,temp.re);
	return(c);

}  /* end of cdiv  */

dcomplex dcsum(a,b)		 /* complex addition  */
dcomplex a,b;
{
dcomplex c;

	c.re = a.re + b.re;
	c.im = a.im + b.im;
	return(c);

}   /* end of csum  */
 
dcomplex dcdiff(a,b)				 /* complex subtraction routine. */
dcomplex a,b;
{
dcomplex c;

	c.re = a.re - b.re;
	c.im = a.im - b.im;
	return(c);

}   /* end of cdiff  */

dcomplex dscamult(z,c)	 /* scalar multiplication of a complex number  */
dcomplex z;
double c;
{
dcomplex ans;

	ans.re = c * z.re;
	ans.im = c * z.im;
	return(ans);

}  /*  end of scamult   */

dcomplex dscadiv(z,c)   /* scalar division of a complex number */
dcomplex z;
double c;
{
dcomplex ans;

	ans.re =  z.re / c ;
	ans.im =  z.im / c ;
	return(ans);

}  /*  end of dscadiv   */


double dmodu(z)			/* function to compute modulus of a complex number */
dcomplex z;
{
double ans;

	ans = (z.re * z.re) + (z.im * z.im);
	ans = sqrt((double) ans);
	return(ans);

}   /*  end modu   */
