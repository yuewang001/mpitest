//////////////////////////////////////////////////////////////////////
// ��Ȩ (C), 1988-1999, XXXX��˾
// �� �� ��: Pub_Func.h
// ��    ��:        �汾:       ʱ��:
// ��    ��: �����ýӿں������� 
// ��    ������ #include <filename.h> ��ʽ�����ñ�׼���ͷ�ļ������������ӱ�׼��Ŀ¼��ʼ������
//           �� #include "filename.h" ��ʽ�����÷Ǳ�׼���ͷ�ļ��������������û��Ĺ���Ŀ¼��ʼ������
// �޸ļ�¼:     // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                 // �߼��޸����ݼ���  
// 1. ʱ��:
//    ����:
//    �޸�����:
// 2. ...
//////////////////////////////////////////////////////////////////////
#ifndef _PUB_FUNC_H__
#define _PUB_FUNC_H__
//���ص�ǰʱ��
#ifdef __cplusplus
extern "C"
{
#endif
extern double EMT_gettime();

extern int find_C(int* v, int x, int m);  //���������е�Ԫ�أ�����Ԫ��λ��
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
extern unsigned long long rdtsc();                             //ͨ��CPUƵ��ȡʱ��
#endif


#ifdef __cplusplus
};
#endif
#endif