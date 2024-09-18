
#if __OPENCL_VERSION__ <= CL_VERSION_1_1
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif
#if __OPENCL_C_VERSION__ <= CL_VERSION_1_1
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#endif
typedef double REAL;
typedef double4 REAL4;
typedef double8 REAL8;

typedef int INT;
typedef unsigned int UINT;
typedef int2 INT2;
typedef uint4 UINT4;
typedef int4 INT4;
typedef ulong ULONG;
typedef uchar UCHAR;
// arrays
typedef __global REAL *REAL_ARRAY;
typedef __global REAL4 *REAL4_ARRAY;
typedef __global REAL8 *REAL8_ARRAY;
typedef __global UINT4 *UINT4_ARRAY;
typedef __global INT4 *INT4_ARRAY;
typedef __global INT *INT_ARRAY;
typedef __global INT2 *INT2_ARRAY;
typedef __global ULONG *ULONG_ARRAY;
typedef __global UCHAR *UCHAR_ARRAY;

#define MAKE_REAL4(x, y, z, w) (REAL4)(x, y, z, w)
#define MAKE_REAL8_ZERO() (REAL8)(0, 0, 0, 0, 0, 0, 0, 0)
#define MAKE_REAL4_ZERO() (REAL4)(0, 0, 0, 0)
#define MAKE_UINT4(x, y, z, w) (UINT4)(x, y, z, w)
__constant REAL PI = 3.14159265359;
