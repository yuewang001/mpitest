//////////////////////////////////////////////////////////////////////
// 版权 (C), 1988-1999, XXXX公司
// 文 件 名: Pub_Func.h
// 作    者:        版本:       时间:
// 描    述: 程序公用接口函数声明 
// 其    他：用 #include <filename.h> 格式来引用标准库的头文件（编译器将从标准库目录开始搜索）
//           用 #include "filename.h" 格式来引用非标准库的头文件（编译器将从用户的工作目录开始搜索）
// 修改记录:     // 修改历史记录列表，每条修改记录应包括修改日期、修改
                 // 者及修改内容简述  
// 1. 时间:
//    作者:
//    修改内容:
// 2. ...
//////////////////////////////////////////////////////////////////////
#ifndef _PUB_FUNC_H__
#define _PUB_FUNC_H__
//返回当前时间
#ifdef __cplusplus
extern "C"
{
#endif
extern double EMT_gettime();

extern int find_C(int* v, int x, int m);  //搜索数组中的元素，返回元素位置
//void cplusplus_print_version_(char * s)
extern void cplusplus_print_version_(void); 
#ifdef _REALTIME
// FIFO close
extern void cplusplus_fifo_close(char *devname,int *fno,int *nret);
// FIFO read
extern void cplusplus_fifo_read(int *fno,char *sline,int *maxlen, int *nread);
// FIFO operation
extern void cplusplus_fifo_write(int *fno,char *sline, int *nwrite, int *nret);

extern void cplusplus_fifo_truncate(int *fno,int *length);

extern void cplusplus_fifo_open(char *devname, int *fno);

extern void cplusplus_usleep(int * us);

extern void cplusplus_prints(char * s);

extern void cplusplus_rtprints(char * s);

extern void cplusplus_rt_finilize(void);

extern void cplusplus_rtclock_sleepto(double *sleepto);

extern void cplusplus_rtclock_sleep(int usec);

extern void  cplusplus_rtclock_gettime (double *curr_time);

extern void unlock_pages(void);
extern void lock_pages(void);
extern int get_coreid();
extern int create_rt_thread(void * func);
extern void *rt_thread(void *func);
extern void init_rt_env();

#endif


#ifdef _LINUX
extern unsigned long long rdtsc();                             //通过CPU频率取时间
#endif


#ifdef __cplusplus
};
#endif
#endif