

#ifndef _CL_QUERY_H_
#define _CL_QUERY_H_

void clPrintAllPlatformInfo(cl_platform_id platform);
void clPrintAllDeviceInfo(cl_device_id device);
void clPrintAllContextInfo(cl_context context);
void clPrintAllQueueInfo(cl_command_queue queue);
void clPrintAllKernelInfo(cl_kernel kernel);
void clPrintAllEventInfo(cl_event event);
void clPrintAllKernelWorkInfo(cl_kernel kernel, cl_device_id device);

#endif
