

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <clenvironment.h>
#include <clquery.h>
#include <clmath.h>
#ifndef CL_BUILD_RUNTIME
#include <kernel_imgfilter.h>
#endif

static void notify(cl_program program, void *arg)
{
    //printf("Program %p Arg %p\n",program, arg);
}

cl_int range_of_operator(cl_char *op, cl_uint n, cl_uint limit)
{
    cl_int range;
    cl_uint p,q,r,s;
    cl_int max = 0, min = 0x7FFFFFFF;
    for (p = 0; p < 2; p++)
    {
        for (q = 0; q < n; q++)
        {
            for (r = 0; r < n; r++)
            {
                s = (p * n * n) + (q * n) + r;
                if (op[s] < min)
                    min = op[s];
                else if (op[s] > max)
                    max = op[s];
            }
        }
    }
    range = isqrt(ipow(abs(min)*limit,2) + ipow(max*limit,2));
    return range;
}

cl_int imgfilter1d(cl_environment_t *pEnv,
                   cl_uint width,
                   cl_uint height,
                   cl_uchar *pSrc,
                   cl_int srcStride,
                   cl_uchar *pDst,
                   cl_int dstStride,
                   cl_char *op,
                   cl_uint opDim,
                   cl_uint range,
                   cl_uint limit)
{
    cl_int err = CL_SUCCESS;
    cl_uint numSrcBytes = srcStride * height;
    cl_uint numDstBytes = dstStride * height;
    cl_uint numOpBytes = 2 * opDim * opDim;
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &width, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &height, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numSrcBytes, pSrc, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &srcStride, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numDstBytes, pDst, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &dstStride, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numOpBytes, op, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &opDim, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &range, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &limit, NULL, CL_MEM_READ_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_edge_filter",
        params, dimof(params),
        2,
        {0,0,0},
        {width, height, 0},
        {1,1,1},
        CL_SUCCESS, 0,0,0
    };
    err = clCallKernel(pEnv, &call,1);
    return err;
}

cl_int imgfilter1d_opt(cl_environment_t *pEnv,
                   cl_uint width,
                   cl_uint height,
                   cl_uchar *pSrc,
                   cl_int srcStride,
                   cl_uchar *pDst,
                   cl_int dstStride,
                   cl_char *op,
                   cl_uint opDim,
                   cl_uint range,
                   cl_uint limit)
{
    cl_int err = CL_SUCCESS;
    cl_uint numSrcBytes = srcStride * height;
    cl_uint numDstBytes = dstStride * height;
    cl_uint numOpBytes = 2 * opDim * opDim;
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &width, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &height, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numSrcBytes, pSrc, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &srcStride, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numDstBytes, pDst, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_int), &dstStride, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_1D, numOpBytes, op, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &opDim, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &range, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_BUFFER_0D, sizeof(cl_uint), &limit, NULL, CL_MEM_READ_ONLY},
        {CL_KPARAM_LOCAL,     sizeof(cl_uchar)*18, NULL, NULL, CL_MEM_READ_WRITE},
        {CL_KPARAM_LOCAL,     sizeof(cl_char)*18, NULL, NULL, CL_MEM_READ_WRITE},
    };
    cl_kernel_call_t call = {
        "kernel_edge_filter_opt",
        params, dimof(params),
        2,
        {0,0,0},
        {width, height, 0},
        {1,1,1},
        CL_SUCCESS, 0,0,0
    };
    err = clCallKernel(pEnv, &call,1);
    return err;
}

int main(int argc, char *argv[])
{
    if (argc >= 5)
    {
        FILE *fi = NULL;
        FILE *fo = NULL;
        cl_uint width = atoi(argv[2]);
        cl_uint height = atoi(argv[3]);
        cl_int err = CL_SUCCESS;

#ifdef CL_BUILD_RUNTIME
		cl_environment_t *pEnv = clCreateEnvironment(KDIR"kernel_imgfilter.cl", CL_DEVICE_TYPE_GPU, 2, notify, CL_ARGS);
#else
        cl_environment_t *pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, CL_ARGS);
#endif
        if (!pEnv)
        {
            clDeleteEnvironment(pEnv);
            return -1;
        }

        clPrintAllKernelInfo(pEnv->kernels[0]);

        fi = fopen(argv[1], "rb");
        fo = fopen(argv[4], "wb+");
        if (fi && fo)
        {
            cl_char sobel[2][3][3] = { { {1,2,1}, {0,0,0}, {-1,-2,-1} }, { {-1,0,1}, {-2,0,2}, {-1,0,1} } };
            size_t numBytes = width * height * sizeof(unsigned char);
            cl_uchar *input_image = (cl_uchar *)malloc(numBytes);
            cl_uchar *output_image = (cl_uchar *)malloc(numBytes);
            clock_t c_start, c_diff;
            cl_uint limit = 255;
            cl_uint range = range_of_operator((cl_char *)sobel, 3, limit);
            printf("Range of Sobel = %u, Limit = %u\n",range,limit);
            do {
                numBytes = fread(input_image, 1, numBytes, fi);
                if (numBytes == 0)
                    break;
                c_start = clock();
                err = imgfilter1d_opt(pEnv, width, height,
                                  input_image, width,
                                  output_image, width,
                                  (cl_char *)sobel, 3,
                                  range, limit);
                c_diff = clock() - c_start;
                printf("Sobel took %lu ticks\n", c_diff);
                numBytes = fwrite(output_image, 1, numBytes, fo);
            } while (err == CL_SUCCESS);

            free(input_image);
            free(output_image);

            fclose(fi);
            fclose(fo);
        }
        clDeleteEnvironment(pEnv);
    }
    else
    {
        printf("%s <filename> <width> <height> <outfile>\n",argv[0]);
    }
    return 0;
}



