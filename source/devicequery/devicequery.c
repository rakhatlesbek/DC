

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clenvironment.h>
#include <clquery.h>

#define CL_PLAT_MAX     (2)
#define CL_DEV_MAX      (4)

void clQueryNotify(const char *errinfo,
                   const void *private_info,
                   size_t cb,
                   void * user_data)
{
    printf("ERROR! %s\n",errinfo);
}

int main(int argc, char *argv[])
{
    cl_uint        p,d;
    cl_platform_id platform_id[CL_PLAT_MAX];
    cl_device_id   device_id[CL_PLAT_MAX][CL_DEV_MAX];
    cl_int         err = 0;
    cl_uint        numPlatforms = 0;
    cl_uint        numDevices[CL_PLAT_MAX] = {0, 0};

    err = clGetPlatformIDs(CL_PLAT_MAX, &platform_id[0], &numPlatforms);
    if (err != CL_SUCCESS)
    {
        printf("Error in OpenCL Platform ID API (%d)\n", err);
    }
    else
    {
        for (p = 0; p < numPlatforms; p++)
        {
            clPrintAllPlatformInfo(platform_id[p]);
            err = clGetDeviceIDs(platform_id[p], CL_DEVICE_TYPE_ALL, CL_DEV_MAX, device_id[p], &numDevices[p]);
            if (err != CL_SUCCESS)
            {
                printf("Failed to acquire ID of OpenCL Devices\n");
                return -1;
            }
            else
            {
                printf("There are %u devices present.\n",numDevices[p]);
                for (d = 0; d < numDevices[p]; d++)
                {
                    clPrintAllDeviceInfo(device_id[p][d]);
                }
            }
        }
    }
    return 0;
}
