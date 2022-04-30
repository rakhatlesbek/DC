

#ifndef _CL_MATH_H_
#define _CL_MATH_H_

/** Integer Square Root Function */
cl_uint isqrt(cl_int x);

/** Integer Power Function */
int ipow(int x, int n);

/** Float Random Number Generator */
float frand(void);

/** Float Random Number Generator with a low and hi bounds */
float frrand(float low, float hi);

/** Float Summation of an array of float values */
float fsum(float *a, cl_uint num);

/** Integer Random Number Generator with a low and hi bounds */
int rrand(int low, int hi);

/** Float[4] Random Number Generator for a single float[4] array with a low and hi bound */
void frand4(cl_float4 f, cl_int l, cl_int h);

/** Normalizes an array of floats with a low and hi bounds into an output array of bytes */
void normalize_float(float *a, cl_int low, cl_int hi, cl_uchar *b, cl_uint numPixels);

#endif
