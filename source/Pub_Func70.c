//////////////////////////////////////////////////////////////////////
// 版权 (C), 1988-1999, XXXX公司
// 文 件 名: Pub_Func.c
// 作    者:        版本:       时间:
// 描    述:       // 模块描述      
// 其    他：用 #include <filename.h> 格式来引用标准库的头文件（编译器将从标准库目录开始搜索）
//           用 #include "filename.h" 格式来引用非标准库的头文件（编译器将从用户的工作目录开始搜索）
// 修改记录:       // 历史修改记录
// 1. 时间:2015.09.08
//    作者:彭红英
//    修改内容:增加错误临时处理：有错误时，打印错误号，程序退出
// 2. ...
//////////////////////////////////////////////////////////////////////
#define _GNU_SOURCE
#ifdef _PARALLEL
#include "mpi.h"
#endif
#include <string.h>
#include <stdio.h>
#include "Pub_Func.h"
#include <stdio.h>
#include "PMF_GlbVarsDef.h"
#ifdef _LINUX
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#endif
#ifdef _REALTIME
#include <sys/ipc.h>
#include <i_malloc.h>

#include "windriver.h"

/*
 * cplusplus passes arguments by reference whereas C passes arguments by value. When you put a 
 * variable name into a function call from cplusplus, the corresponding C function receives a 
 * pointer to that variable. Variable's value can be accessed by dereferencing it.
 */

pthread_t thread;
static int rt_core_id = 0;

extern void rtclock_gettime(unsigned long long *tt);
extern void clock_sleepto(unsigned long long tt);

#endif
#ifdef _LINUX
static int processcore = 0;

static 	cpu_set_t set;
static  cpu_set_t get;
#endif


//************************************
// MethodName:    find_C
// FullName:  find_C
// Access:    public 
// Returns:   int
// Qualifier: /*搜索数组中的元素，返回元素位置 */ /* v is the data range */ /* x is the value to find */ /* m is the search range */ // return the index of x in v
// Parameter: int * v
// Parameter: int x
// Parameter: int m
// Creator:   Mu Qing 2018/02/03
// Other:
//************************************
int find_C(int* v, int x, int m)//C中自己写的搜索数组中的元素，返回元素位置
	// v is the data range
	// x is the value to find
	// m is the search range
	// return the index of x in v
{
	int i;
	for (i = 0;i<m;i++)
	{
		if (x == v[i])
		{ return i+1;}
	}
	return -1;
}


///////////////////////////
//  return current time of computer (s)
//////////////////////////
double EMT_gettime()
{
	double sttime = 0;
	unsigned long long curr;
#ifdef _PARALLEL
#ifdef _REALTIME
	if (g_CtrlPara.iRTvalid==1)
	{
		rtclock_gettime(&curr);
		sttime  = 0.000000001*curr;
	}
	else
	{
		sttime = MPI_Wtime();
	}
#else
	sttime = MPI_Wtime();
#endif
#endif
	return sttime;
}

#ifdef _LINUX
/*
unsigned long long rdtsc()                             //通过CPU频率取时间
{
	unsigned long hi = 0;
	unsigned long lo = 0;
	__asm__ __volatile__ ("lfence;rdtsc" : "=a"(lo), "=d"(hi));

	return (((unsigned long long)lo))|(((unsigned long long)hi)<<32);
}
*/
#endif



#ifdef _REALTIME
static void sig_usr(int signo)
{
	if (signo == SIGINT)
	{
		if (rt_core_id > 0)
		{
			release_rtcore(rt_core_id);
		}
		exit(0);
	}
}

void init_rt_env()
{
	/*i_malloc =  rt_malloc;
	i_calloc =  rt_calloc;
	i_realloc = rt_realloc;
	i_free = rt_free;*/

	if (signal(SIGINT, sig_usr) == SIG_ERR)
	{
		printf("Cannot catch SIGINT\n");
	}
}

void *rt_thread(void *func)
{
	void (*pfun)();
	pfun = func;

	set_thread_affinity(rt_core_id);

	pfun();
}

/* 
 * Creates a RT thread with parameter "func" as address of function to be 
 * executed as RT thread
 */

int create_rt_thread(void * func)
{
	int core, ret;

	core = allocate_rtcore();
	if (core < 0)
	{
		printf("allocate_rtcore failed, core %d\n", core);
		return -1;
	}
	rt_core_id = core;
	printf("EMT is using core %d\n", core);

	ret = pthread_create(&thread, NULL, (void *)rt_thread, func);
	if (0==ret)
	{
		printf("EMT: pthread_create returns %d, and its cpu no. is %d\n", ret, core);
	}
	else
	{
		printf("EMT: pthread_create returns %d\n", ret);
		return -2;
	}

	return 0;
}

int get_coreid()
{
	return rt_core_id;
}

/*
 * Locks process' current and future pages in memory.
 */
void lock_pages(void)
{
}

void unlock_pages(void)
{
}


void  cplusplus_rtclock_gettime (double *curr_time)
{
	unsigned long long curr;

	rtclock_gettime(&curr);
	*curr_time = 0.000000001*curr;
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


void cplusplus_rtclock_sleepto(double *sleepto)
{
	unsigned long long end;

	end = (*sleepto)*1000000000;
	clock_sleepto(end);
}

void cplusplus_rt_finilize(void)
{
	release_rtcore(rt_core_id);
	rt_core_id = 0;
	printf("rtcore_release is %d\n",processcore);
	release_rtcore(processcore);  //释放主进程核
	CPU_CLR(processcore,&set);	
	processcore = 0;	
}

void cplusplus_rtprints(char * s)
{
/*
	rt_printf("%s",s);
*/
}

void cplusplus_prints(char * s)
{
	printf("%s",s);
}

void cplusplus_usleep(int * us)
{
	usleep(*us);
}

void cplusplus_fifo_open(char *devname, int *fno)
{
#ifdef _fifoold
	unlink(devname);
	mkfifo(devname, 0);
	*fno=open(devname, O_RDWR | O_NONBLOCK);
	ftruncate(*fno, 1024*10);
	
#else
	if (pipe2(fno,O_NONBLOCK)<0)
	{
		printf("pipe error;\n");
		fflush(stdout);
	}
	else
	{
		printf("pipe create success, the read termial is %d and the write terminal is %d\n",fno[0],fno[1]);
	}
#endif	
}

void cplusplus_fifo_truncate(int *fno,int *length)
{
	ftruncate(*fno, *length);
}

// FIFO operation
void cplusplus_fifo_write(int *fno,char *sline, int *nwrite, int *nret)
{
	*nret=write(*fno,sline,*nwrite);
}

// FIFO read
void cplusplus_fifo_read(int *fno,char *sline,int *maxlen, int *nread)
{
	*nread = read(*fno,sline,*maxlen);
}

// FIFO close
void cplusplus_fifo_close(char *devname,int *fno,int *nret)
{
#ifdef _fifoold	
	*nret = close(*fno);
	*nret = unlink(devname);
#endif
}

#endif //_REALTIME

//void cplusplus_print_version_(char * s)
void cplusplus_print_version_(void)
{
	printf("ETSDAC version 2.3.0 (%s)", __DATE__);
}

void cpu_setcore()
{
	
	
	int core = 9 ;	
	int count=0;

	//int processcore;	
		processcore=allocate_rtcore();
	
	CPU_ZERO(&set);
	CPU_SET(processcore,&set);
	if(sched_setaffinity(getpid(),sizeof(set),&set)==-1)
	{
		printf("process bind cpu err\n");
		return;
	}
	return;
}

void cpu_processcore_release()
{
		release_rtcore(processcore);
	processcore = 0;
}
