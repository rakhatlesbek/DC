

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <clenvironment.h>
#include <clpid.h>
#ifndef CL_BUILD_RUNTIME
#include <kernel_pid.h>
#endif
#include <clmath.h>

void notify(cl_program program, void *arg)
{
    //printf("Program %p Arg %p\n",program, arg);
}

cl_int pid_loop(cl_environment_t *pEnv,
                cl_pid_t *pids,
                size_t    numPids)
{
    cl_int err;
    cl_kernel_param_t params[] = {
        {CL_KPARAM_BUFFER_1D, sizeof(cl_pid_t) * numPids, pids, NULL, CL_MEM_READ_WRITE}
    };
    cl_kernel_call_t call = {
        "kernel_pid",
        params, dimof(params),
		1, 
		{0,0,0},
        {numPids, 0, 0},
        {1,1,1},
        CL_SUCCESS
    };

    err = clCallKernel(pEnv, &call,1);
    if (err != CL_SUCCESS)
        return err;
    else
        return call.err;
}

int main(int argc, char *argv[])
{
    const size_t numPct = 2;
    cl_pid_t pids[2];
	cl_environment_t *pEnv = NULL;
    time_t start, diff;
    clock_t c_start, c_diff;

    memset(pids, 0, sizeof(pids));

#ifdef CL_BUILD_RUNTIME
	pEnv = clCreateEnvironment(KDIR "kernel_pid.cl", CL_DEVICE_TYPE_GPU, 1, notify, CL_ARGS);
#else	
    pEnv = clCreateEnvironmentFromBins(&gKernelBins, notify, CL_ARGS);
#endif
	if (pEnv)
    {
        cl_uint i = 0;
        for (i = 0; i < numPct; i++)
        {
            pids[i].sv = frand();
            pids[i].pv = frand();
            pids[i].kp = 1.0;
            pids[i].ki = 0.0;
            pids[i].kd = 0.0;
        }

        start = time(NULL);
        c_start = clock();
        pid_loop(pEnv, pids, numPct);
        c_diff = clock() - c_start;
        diff = time(NULL) - start;
        printf("PID Ran in %lu seconds (%lu ticks)\n", diff, c_diff);

        for (i = 0; i < numPct; i++)
        {
            printf("e = sv - pv; mv = po + io + do;\n%lf = %lf - %lf; %lf = %lf + %lf + %lf\n",
                    pids[i].e,
                    pids[i].sv,
                    pids[i].pv,
                    pids[i].mv,
                    pids[i].p_out,
                    pids[i].i_out,
                    pids[i].d_out);
        }
        clDeleteEnvironment(pEnv);
    }
    return 0;
}

