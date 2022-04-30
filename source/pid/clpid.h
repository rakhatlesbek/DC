

#ifndef _CL_PID_H_
#define _CL_PID_H_

#define PCT_INT_WINDOW  (10)

/** e = sv - pv; mv = kp*e + ki*Integral(e,t) + kd*Derivative(e,t) */
typedef struct _cl_pid_t {
    float mv; /**< manipulated variable (final output) */
    float sv; /**< subjective variable (desired output) */
    float pv; /**< perceived variable (actual value) */
    float e;  /**< Last/Current Error */

    float kp; /**< Proportional Constant Tuning Parameter */
    float p_out; /**< Proportional Output */

    float ki; /**< Integral Constant Tuning Paramter */
    float iv; /**< Integral Summing Value over a window */
    float em[PCT_INT_WINDOW]; /**< Past error values which we remember to use to calculate the integrated error */
    int  idx; /**< Current index into the error memory values (wraps by % of PCT_INT_WINDOW) */
    float i_out; /**< Integral Output */

    float kd; /**< Derivative Constant Tuning Parameter */
    float dp; /**< Derivative Product */
    float d_out; /**< Derivative Output */
} cl_pid_t;

#endif

