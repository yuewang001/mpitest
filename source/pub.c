#define _GNU_SOURCE
#ifdef _PARALLEL
#include "mpi.h"
#endif
#include <string.h>
#include <stdio.h>
//#include "Pub_Func.h"
#include <stdio.h>
//#include "PMF_GlbVarsDef.h"
#ifdef _LINUX
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#endif
#include <sys/ipc.h>
#include <i_malloc.h>

#include "windriver.h"



void  cplusplus_rtclock_gettime (unsigned long long *curr_time)
{
	unsigned long long curr;

	rtclock_gettime(&curr);
	*curr_time = 0.001*curr;
	//*curr_time = curr;
}

void cplusplus_rtclock_sleep(int usec)
{
	unsigned long long begin, add;

	add = usec*1000;
	//rtclock_gettime(&begin);
	rtclock_gettime(&begin);	
	begin += add;
	clock_sleepto(begin);
}
