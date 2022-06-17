/* include file for double-precision complex arithmetic subroutines */

typedef struct { double re; double im;} dcomplex ;
typedef struct { float re; float im;} complex ;

extern dcomplex  dcmult(),dconj();
extern dcomplex  dcdiv(),dcsum();
extern dcomplex  dcdiff();
extern dcomplex  dscamult();
extern dcomplex  dscadiv(),zsqrt();
extern double dmodu();

extern complex  cmult(),conj();
extern complex  cdiv(),csum();
extern complex  cdiff();
extern complex  scamult();
extern complex  scadiv();
extern float modu();
