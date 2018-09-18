
#ifndef _PUB_H__
#define _PUB_H__
//返回当前时间
#ifdef __cplusplus
extern "C"
{
#endif

extern void cplusplus_rtclock_sleep(int usec);

extern void  cplusplus_rtclock_gettime (double *curr_time);

#endif


#ifdef _LINUX
extern unsigned long long rdtsc();                             //通过CPU频率取时间
#endif


#ifdef __cplusplus
};
#endif
