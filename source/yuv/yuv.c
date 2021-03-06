

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <clenvironment.h>
#include <clmath.h>
#ifndef CL_BUILD_RUNTIME
#include <kernel_yuv.h>
#endif

void notify(cl_program program, void *arg)
{
    printf("Compiled Program %p Arg %p\n",program, arg);
}

cl_int cl_convert_rgbf_to_yuvf_bt601(cl_environment_t *pEnv,
                                     float *r,
                                     float *g,
                                     float *b,
                                     float *y,
                                     float *u,
                                     float *v,
                                     cl_uint numPixels)
{
    cl_int err = CL_SUCCESS;
    const size_t numBytes = numPixels * sizeof(float);
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, numBytes, r, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, g, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, b, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, y, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, u, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, v, NULL, CL_MEM_WRITE_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_rgb2yuv_bt601",
        params, dimof(params),
        1,
        {0,0,0},
        {numPixels, 0,0},
        {1,1,1},
        CL_SUCCESS,0,0,0
    };
    err = clCallKernel(pEnv, &call,1);
    return err;
}

cl_int cl_convert_rgbf_to_yuvf(cl_environment_t *pEnv,
                               float *r,
                               float *g,
                               float *b,
                               float *y,
                               float *u,
                               float *v,
                               float *a, // constants
                               cl_uint numPixels)
{
    cl_int err = CL_SUCCESS;
    const size_t numBytes = numPixels * sizeof(float);
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, numBytes, r, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, g, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, b, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, sizeof(float) * 9, a, NULL, CL_MEM_READ_ONLY}, // constants only have 9 elements
        {CL_KPARAM_BUFFER_1D, numBytes, y, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, u, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, numBytes, v, NULL, CL_MEM_WRITE_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_rgb2yuv",
        params, dimof(params),
        1,
        {0,0,0},
        {numPixels, 0, 0},
        {1,1,1},
        CL_SUCCESS,0,0,0
    };
    err = clCallKernel(pEnv, &call, 1);
    return err;
}

cl_int convert_uyvy_to_2bgr(cl_environment_t *pEnv, cl_uchar *pUYVY, cl_uchar *pBGR, cl_uint width, cl_uint height, cl_int srcStride, cl_int dstStride)
{
    cl_int err = CL_SUCCESS;
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, srcStride*height, pUYVY, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, dstStride*height, pBGR, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &width, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &height, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &srcStride, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &dstStride, NULL, CL_MEM_READ_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_uyvy_to_2bgr",
        params, dimof(params),
        2,
        {0,0,0},
        {width/2, height, 0}, // width/2 since it is macropixel packed!
        {1,1,1},
        CL_SUCCESS, 0,0,0
    };
    err = clCallKernel(pEnv, &call, 1);
    return err;
}

cl_int convert_yuv420_to_luma(cl_environment_t *pEnv, cl_uchar *pYUV420, cl_uchar *pLuma, cl_uint width, cl_uint height, cl_int srcStride, cl_int dstStride)
{
    cl_int err = CL_SUCCESS;
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, srcStride*height, pYUV420, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, dstStride*height, pLuma, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &width, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &height, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &srcStride, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &dstStride, NULL, CL_MEM_READ_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_yuv420_to_luma",
        params, dimof(params),
        2,
        {0,0,0},
        {width, height, 0},
        {1,1,1},
        CL_SUCCESS, 0,0,0
    };
    err = clCallKernel(pEnv, &call, 1);
    return err;
}


int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        const cl_uint width = 1920;
        const cl_uint height = 1080;
        const cl_uint numPixels = height * width;
        cl_int err = CL_SUCCESS;

        float *r = cl_malloc_array(float, numPixels);
        float *g = cl_malloc_array(float, numPixels);
        float *b = cl_malloc_array(float, numPixels);
        float *y = cl_malloc_array(float, numPixels);
        float *u = cl_malloc_array(float, numPixels);
        float *v = cl_malloc_array(float, numPixels);
        cl_uchar *Yp = cl_malloc_array(cl_uchar, numPixels);
        cl_uchar *Up = cl_malloc_array(cl_uchar, numPixels);
        cl_uchar *Vp = cl_malloc_array(cl_uchar, numPixels);

        float bt601[9] = {0.257f, 0.504f, 0.098f, -0.148f, -0.291f, 0.439f, 0.439f, -0.368f, -0.071f};
        time_t start, diff;
        clock_t c_start, c_diff1, c_diff2;

#ifdef CL_BUILD_RUNTIME
        cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_yuv.cl",CL_DEVICE_TYPE_GPU,1,notify, CL_ARGS);
#else
        cl_environment_t *pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, CL_ARGS);
#endif
		printf("Processing %ux%u => %u pixels\n", width, height, numPixels);
        
        if (pEnv && r && g && b && y && u && v)
        {
            cl_uint i = 0;

            srand((unsigned int)time(NULL));
            // initialize the data
            for (i = 0; i < numPixels; i++)
            {
                r[i] = frrand(0.0,1.0); // [0-1]
                g[i] = frrand(0.0,1.0); // [0-1]
                b[i] = frrand(0.0,1.0); // [0-1]
                y[i] = 0.00;
                u[i] = 0.00;
                v[i] = 0.00;
            }

            start = time(NULL);
            c_start = clock();
            err = cl_convert_rgbf_to_yuvf_bt601(pEnv, r, g, b, y, u, v, numPixels);
            cl_assert(err == CL_SUCCESS,printf("Error = %d\n",err));
            c_diff1 = clock() - c_start;
            diff = time(NULL) - start;
            printf("With Constants Version Ran in %lu seconds (%lu ticks)\n", diff, c_diff1);

            // initialize the data
            for (i = 0; i < numPixels; i++)
            {
                r[i] = frrand(0.0,1.0); // [0-1]
                g[i] = frrand(0.0,1.0); // [0-1]
                b[i] = frrand(0.0,1.0); // [0-1]
                y[i] = 0.00;
                u[i] = 0.00;
                v[i] = 0.00;
            }

            start = time(NULL);
            c_start = clock();
            cl_convert_rgbf_to_yuvf(pEnv, r, g, b, y, u, v, bt601, numPixels);
            c_diff2 = clock() - c_start;
            diff = time(NULL) - start;
            printf("With No Constants Version Ran in %lu seconds (%lu ticks)\n", diff, c_diff2);

            normalize_float(y,   16, 235, Yp, numPixels);
            normalize_float(u, -128, 128, Up, numPixels);
            normalize_float(v, -128, 128, Vp, numPixels);

    #ifdef CL_DEBUG
            for (i = 0; i < numPixels; i++)
                printf("YUV = {0x%02x, 0x%02x, 0x%02x}\n", Yp[i], Up[i], Vp[i]);
    #endif
            clDeleteEnvironment(pEnv);
        }

        cl_free(r); cl_free(g); cl_free(b);
        cl_free(y); cl_free(u); cl_free(v);
    }
    else if (argc >= 6)
    {
        char srcfSrcle[CL_MAX_PATHSIZE];
        char dstfSrcle[CL_MAX_PATHSIZE];
        int width = atoi(argv[2]);
        int height = atoi(argv[3]);
        int f, numFrames = atoi(argv[4]);
        unsigned int srcStride = 0;
        unsigned int dstStride = 0;
        int numSrcBytes = 0;
        int numDstBytes = 0;
        int numSrcPlanes = 1;
        int numDstPlanes = 1;
        int type = (argc > 6?atoi(argv[6]):0);
        FILE *fSrc = NULL;
        FILE *fDst = NULL;
        cl_uchar *pSrc = NULL;
        cl_uchar *pDst = NULL;
		cl_environment_t *pEnv = NULL;
        if (type == 0) // UYVY to BGR
        {
            srcStride = width * 2;
            dstStride = width * 3;
            numSrcBytes = srcStride * height * numSrcPlanes;
            numDstBytes = dstStride * height * numDstPlanes;
        }
        else if (type == 1) // IYUV to Luma
        {
            srcStride = width;
            numSrcPlanes = 3;
            dstStride = width;
            numSrcBytes = (width * height * 3)/2;
            numDstBytes = dstStride * height * numDstPlanes;
        }

        pSrc = cl_malloc_array(cl_uchar, numSrcBytes);
        pDst = cl_malloc_array(cl_uchar, numDstBytes);

        strncpy(srcfSrcle, argv[1], CL_MAX_PATHSIZE);
        strncpy(dstfSrcle, argv[5], CL_MAX_PATHSIZE);

        fSrc = fopen(srcfSrcle, "rb");
        fDst = fopen(dstfSrcle, "wb+");

#ifdef CL_BUILD_RUNTIME
        pEnv = clCreateEnvironment(KDIR"kernel_yuv.cl",CL_DEVICE_TYPE_GPU, 1,notify, CL_ARGS);
#else
        pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, CL_ARGS);
#endif
        if (pEnv && fSrc && fDst && pSrc && pSrc)
        {
            for (f = 0; f < numFrames; f++)
            {
                fread(pSrc, 1, numSrcBytes, fSrc);
                if (type == 0)
                    convert_uyvy_to_2bgr(pEnv, pSrc, pDst, width, height, srcStride, dstStride);
                else if (type == 1)
                    convert_yuv420_to_luma(pEnv, pSrc, pDst, width, height, srcStride, dstStride);
                fwrite(pDst, 1, numDstBytes, fDst);
            }
            clDeleteEnvironment(pEnv);
        }
        cl_free(pDst);
        cl_free(pSrc);

        fclose(fSrc);
        fclose(fDst);
    }
    return 0;
}

