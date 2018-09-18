/*
   ����ϵͳʵʱ����������������̬(ST)�������̬(EMT)�ȶ������ͳ��򣬾����emt.h��SIMUTYPE_ALL���塣

   ������ģ��ʵʱ�����е����̬����EMT����SIMUTYPE_STCTRL��SIMUTYPE_STSUB��SIMUTYPE_EMTSUB��SIMUTYPE_EMTPHY��SIMUTYPE_EMTIO�����͵Ľ��̴���ͨ�Ź�ϵ��
   ������ʵ�ֵ���emt��ͨ�ŷ�ʽ֮һ����������֮���Ե�ͨ�š�

   ������ɵ������У����÷�����
# ./mpirun -np 6 emt
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <i_malloc.h>
#include "test.h"
#include "emt.h"

#include "windriver.h"  

#define _GNU_SOURCE

#ifdef _LINUX
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sched.h>
#include <errno.h> 
//mlock
#include <sys/mman.h>

//setrlimit
#include <sys/resource.h>
#else //_LINUX
#include <sys/timeb.h>
#endif //_LINUX

#ifdef _LINUX
pthread_t thread;
#endif //_LINUX


int isCalculationOver;

unsigned long long ts,te,tn,tstep,tc;
long long  tmax_ctrl=0, tmin_ctrl=1000000;
long long  tmax_val=0, tmin_val=100000000,tmax_cal=0,tmin_cal=100000000;
unsigned long long step, nstep;
int cnt=0;
FILE *fp;
FILE *fp11;
double SendBuf[EMT_COMM_NUM], RecvBuf[EMT_COMM_NUM], *DataBuf;

#ifdef SHM_EMTIO
struct shm_per_task shm_ctrl_per_task;
#endif

#ifdef STATISTICS_LOG /* log the time spent for each step */

unsigned long long time_info_1[CALCU_NUM];
unsigned long long time_info_2[CALCU_NUM];
unsigned long long time_info_3[CALCU_NUM];
unsigned long long time_info_4[CALCU_NUM];
unsigned long long time_info_5[CALCU_NUM];
unsigned long long time_info_6[CALCU_NUM];
unsigned long long time_info_7[CALCU_NUM];
unsigned long long time_info_8[CALCU_NUM];

void print_and_log(void)
{
	char str[2048];
	int i;
	int tmp;

	/* ouput the original data to file */	
	if ((fp11 = fopen("time_info.txt", "w")) != NULL)
	{

		for(i = 0; i < CALCU_NUM; i++)
		{
			tmp = sprintf(str, "%d	%lld	%lld	%lld	%lld	%lld	%lld",i,
					time_info_1[i],time_info_2[i],time_info_3[i],time_info_4[i],time_info_5[i],time_info_6[i]);

			sprintf(&str[tmp], "	%lld	%lld	%lld	%lld	%lld	%lld\n",
					time_info_2[i]-time_info_1[i],time_info_3[i]-time_info_2[i],time_info_4[i]-time_info_3[i],time_info_5[i]-time_info_4[i],time_info_6[i]-time_info_5[i],time_info_6[i]-time_info_1[i]);		

			// sprintf(str, "%d	%lld	%lld	%lld	%lld	%lld	%lld	%lld	%lld	%lld	%lld	%lld	%lld\n",i,
			// time_info_1[i],time_info_2[i],time_info_3[i],time_info_4[i],time_info_5[i],time_info_6[i],
			// time_info_2[i]-time_info_1[i],time_info_3[i]-time_info_2[i],time_info_4[i]-time_info_3[i],time_info_5[i]-time_info_4[i],time_info_6[i]-time_info_5[i],time_info_6[i]-time_info_1[i]);		
			fprintf(fp11, str);
		}
		fclose(fp11);
	}
}
#endif /* STATISTICS_LOG */

void do_calc(int num)
{
	int i, j;

	for (i=0; i<num; i++)
	{
		for (j=0; j<EMT_COMM_NUM; j++)
		{
			SendBuf[j] = sin(RecvBuf[j]*3.1415926+0.5);
			SendBuf[j]= sin(SendBuf[j]*3.1415926+0.5);
		}
	}
}

void emt_comm()
{
	int i, j;
	MPI_Status MpiStt;
	double buf[EMT_COMM_NUM];

	memset(buf, 0, EMT_COMM_NUM*sizeof(double));

	// ������������ż�����̷�MPI��Ϣ
	for (i=0; i<ProcInfo.NumProcEMTCAL; i++)
	{
		if (i != ProcInfo.EMTCALId)
		{
			MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, i, i+1, ProcInfo.CommEMTCAL);
		}
	}

	// ������������ż�����̽���MPI��Ϣ
	for (i=0; i<ProcInfo.NumProcEMTCAL; i++)
	{
		if (i != ProcInfo.EMTCALId)
		{
			MPI_Recv(buf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, i, ProcInfo.EMTCALId+1, ProcInfo.CommEMTCAL, &MpiStt);
		}

		// �ۼ����е�ż�����̷��ؽ��
		for (j=0; j<EMT_COMM_NUM; j++) RecvBuf[j] += buf[j];
	}
}

void emtphy_comm()
{
	int i, j;
	MPI_Status MpiStt;
	double buf[EMT_COMM_NUM];

	memset(buf, 0, EMT_COMM_NUM*sizeof(double));

	// ��������������ӿڽ��̷�MPI��Ϣ
	for (i=0; i<ProcInfo.NumProcEMTPhy; i++)
	{
		MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTPhyId[i], ProcInfo.EMTPhyId[i]+1, MPI_COMM_WORLD);
	}

	// ��������������ӿڽ��̽���MPI��Ϣ
	for (i=0; i<ProcInfo.NumProcEMTPhy; i++)
	{
		MPI_Recv(buf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTPhyId[i], ProcInfo.Id+1, MPI_COMM_WORLD, &MpiStt);
	}

	// �ۼ���������ӿڽ��̷��ؽ��
	for (j=0; j<EMT_COMM_NUM; j++) RecvBuf[j] += buf[j];
}

#ifndef SHM_EMTIO
void emtio_comm()
{
	MPI_Request MpiRqst;

	if (ProcInfo.NumProcEMTIO == 0) return;					// û��EMTIO���̣��˳�

	// ��IO���̷���MPI��Ϣ
	MPI_Isend(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTIOId, ProcInfo.EMTIOId+1, MPI_COMM_WORLD, &MpiRqst);
}
#else
void emtio_comm()
{
	int ind;
	struct shm_blk *pblk;

	if (ProcInfo.NumProcEMTIO == 0) return;					// û��EMTIO���̣��˳�

	// ��IO���̷���MPI��Ϣ
	if (shm_ctrl_per_task.mem == NULL)
	{
		return;
	}

	ind  = shm_ctrl_per_task.prod_ind;
	pblk = (struct shm_blk *)((unsigned char*)shm_ctrl_per_task.mem + shm_ctrl_per_task.blk_size * ind);
	if (pblk->sem != SHM_READ_COMPLETE)
	{
		printf("write to emtio, overflow\n");
	}

	//printf("emt, data=%lf, sem=0x%08x", pblk->data[0], pblk->sem);
	memcpy(pblk->data, SendBuf, EMT_COMM_NUM * sizeof(double));
	//memcpy(pblk->data, "feedback for the shm...", EMT_COMM_NUM * sizeof(double));
	pblk->sem = SHM_WRITE_COMPLETE;

	//printf("---- data=%lf, sem=0x%08x\n", pblk->data[0], pblk->sem);

	shm_ctrl_per_task.prod_ind++;
	if (shm_ctrl_per_task.prod_ind == shm_ctrl_per_task.blk_num)
	{
		shm_ctrl_per_task.prod_ind = 0;
	}
}

#endif
void emt_comm_both_sides()
{
	
	int i, j;
	MPI_Status MpiStt;
	double buf[EMT_COMM_NUM];
	memset(buf,0,EMT_COMM_NUM*sizeof(double));
	if(ProcInfo.EMTCALId%2==0)
	{		
		MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId + 1, ProcInfo.EMTCALId + 1, ProcInfo.CommEMTCAL);
		MPI_Recv(buf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId+1, ProcInfo.EMTCALId, ProcInfo.CommEMTCAL, &MpiStt);
			// �ۼ����е�ż�����̷��ؽ��
		for (j = 0; j<EMT_COMM_NUM; j++) RecvBuf[j] += buf[j];
	}else
	{		
		MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId - 1, ProcInfo.EMTCALId - 1, ProcInfo.CommEMTCAL);
		MPI_Recv(buf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId - 1, ProcInfo.EMTCALId, ProcInfo.CommEMTCAL, &MpiStt);
			// �ۼ����е�ż�����̷��ؽ��
		for (j = 0; j<EMT_COMM_NUM; j++) RecvBuf[j] += buf[j];
	}

}
void emt_comm_both_sides2()
{
	int i, j;
	MPI_Status MpiStt;
	double buf[EMT_COMM_NUM];
	memset(buf,0,EMT_COMM_NUM*sizeof(double));
	if(ProcInfo.EMTCALId!=0&&ProcInfo.EMTCALId!=(ProcInfo.NumProcEMTCAL-1))
	{
		
			MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId + 1, ProcInfo.EMTCALId + 1, ProcInfo.CommEMTCAL);
			MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId - 1, ProcInfo.EMTCALId - 1, ProcInfo.CommEMTCAL);
	}
	if (ProcInfo.EMTCALId == 0)
	{
		MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId + 1, ProcInfo.EMTCALId + 1, ProcInfo.CommEMTCAL);
	//	printf("this is 0 \n");
	}
	if (ProcInfo.EMTCALId == (ProcInfo.NumProcEMTCAL - 1))
	{
		MPI_Send(SendBuf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId - 1, ProcInfo.EMTCALId - 1, ProcInfo.CommEMTCAL);
	}
	if(ProcInfo.EMTCALId!=0)
	{
		
			MPI_Recv(buf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId-1, ProcInfo.EMTCALId, ProcInfo.CommEMTCAL, &MpiStt);
			// �ۼ����е�ż�����̷��ؽ��
			for (j = 0; j<EMT_COMM_NUM; j++) RecvBuf[j] += buf[j];
	}
	if(ProcInfo.EMTCALId!=(ProcInfo.NumProcEMTCAL-1))
	{
		
			MPI_Recv(buf, EMT_COMM_NUM, MPI_DOUBLE_PRECISION, ProcInfo.EMTCALId+1, ProcInfo.EMTCALId, ProcInfo.CommEMTCAL, &MpiStt);
			// �ۼ����е�ż�����̷��ؽ��
			for (j = 0; j<EMT_COMM_NUM; j++) RecvBuf[j] += buf[j];
	}
}
int init_buf()
{
	int i;

	for (i=0; i<EMT_COMM_NUM; i++)
	{
		SendBuf[i] = (1.0 + ProcInfo.EMTCALId) * i;
		RecvBuf[i] = 0.0;
	}

	//DataBuf = new double[EMT_COMM_NUM * nstep];
//	DataBuf = malloc(EMT_COMM_NUM * nstep * sizeof(double));

	return 0;
}

int update_buf()
{
	int i;

	for (i=0; i<EMT_COMM_NUM; i++)
	{
		RecvBuf[i] = 0.0;

		//DataBuf[(step-1)*EMT_COMM_NUM+i] = SendBuf[i];
	}

	return 0;
}

int finalize_buf()
{
	int i, j;
	FILE* fp;
	char fname[128];

#ifndef RUN_LONG_TIME_TEST
	sprintf(fname, "emt_%02d.txt", ProcInfo.EMTCALId);
	if ((fp = fopen(fname, "w")) != NULL)
	{
		for (i=0; i<nstep; i++)
		{
			for (j=0; j<EMT_COMM_NUM; j++) fprintf(fp, "%14.6e, ", DataBuf[i*EMT_COMM_NUM+j]);
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
#endif

	//delete[] DataBuf;
	free(DataBuf);

	return 0;
}

#ifdef _LINUX
extern void rtclock_gettime(unsigned long long *tt);
extern void clock_sleepto(unsigned long long tt);
extern void start_launch_on_core(int (*f)(void *), void *arg, unsigned char core);
#endif //_LINUX

#ifdef _LINUX

int warm=0;
void *thread_emt(__attribute__((unused)) void *arg)
{
	unsigned long long t1, t2,t3,t4,t5,t6,max_val=0,val,cal,sum_cal=0,avg_cal=0,sum_val=0,avg_val=0;
	unsigned long long step_all_max=0,step_all_min=0,step_calc_max=0,step_calc_min=0;
	char filename[40];
	int aa=0;
	unsigned long long *total_time=malloc((nstep-warm)*sizeof(unsigned long long));
	
	memset(total_time,0,((nstep-warm)*sizeof(unsigned long long)));
	//unsigned int core_id = *(unsigned int*)arg;
	//set_thread_affinity(core_id);
	FILE *fp;
	unsigned long pid=getpid();
	sprintf(filename,"total_comm_%d.txt",ProcInfo.Id);
	fp=fopen(filename,"wb");
#ifdef RUN_LONG_TIME_TEST
	time_status_init();
#endif
	printf("%d: Warm system %d times\n",gettid(),warm);
	MPI_Barrier(ProcInfo.CommCAL);			// ͬ�����MPI���̵�thread
	
	// �������ڿ���
	rtclock_gettime(&ts);
	for(step=1; step<=nstep; step++)
	{
		rtclock_gettime(&t1);
//		tn = t1 + tstep;
		emt_comm();							// emt֮��MPIͨ�źͼ���
	// 	emt_comm_both_sides();
	// 	emt_comm_both_sides2();
//		rtclock_gettime(&t2);
//		emtphy_comm();						// emt��emtphy֮��MPIͨ��
		rtclock_gettime(&t3);
		do_calc(CalcNum);
		rtclock_gettime(&t4);
//		emtio_comm();						// emt��emtio֮��MPIͨ��
//		rtclock_gettime(&t5);
		update_buf();
		rtclock_gettime(&t6);

#ifdef STATISTICS_LOG
		time_info_1[step-1]=t1;
		time_info_2[step-1]=t2;
		time_info_3[step-1]=t3;
		time_info_4[step-1]=t4;
		time_info_5[step-1]=t5;
		time_info_6[step-1]=t6;
		time_info_7[step-1]=step;
		time_info_7[step-1]=pid;
#endif

	  if ( step >  warm)
	  {	
		val = t3-t1;
		if (val > tmax_val)
		{
			tmax_val = val;
			step_all_max=step;
		}
		if (val < tmin_val)
		{
			tmin_val = val;
			step_all_min=step;
		}
		cal=t4-t3;
		if(cal>tmax_cal)
		{
			tmax_cal=cal;
			step_calc_max=step;
		}
		if(cal<tmin_cal)
		{
			tmin_cal=cal;
			step_calc_min=step;
		}
		sum_cal+=cal;
		
		sum_val+=val;
		total_time[aa]=val;
		aa++;
	   }
		MPI_Barrier(ProcInfo.CommCAL);			// ͬ�����MPI���̵�thread
		//printf("core=%d,step:%d,t3-t1:%lld,t4-t3:%lld,t6-t4:%lld,t6-61:%lld\n", \
			pid,step,t3-t1,t4-t3,t6-t4,t6-t1);
	


	}

	rtclock_gettime(&tc);
	
	for(aa=0;aa<nstep-warm;aa++)
	{
		fprintf(fp,"%lld\n",total_time[aa]);
	}

	avg_val=sum_val/(nstep-warm);
	avg_cal=sum_cal/(nstep-warm);
	printf("core=%d,Id:%d CALC: Mean %lld Max %lld (%lld) Min %lld (%lld) | TOT: Mean %lld Max %lld (%lld) Min %lld (%lld)\n", \
			pid,ProcInfo.Id,avg_cal,tmax_cal,step_calc_max,tmin_cal,step_calc_min,\
			avg_val,tmax_val,step_all_max,tmin_val,step_all_min);

	fclose(fp);
	return NULL;
}

#else //_LINUX

void thread_emt()
{
	MPI_Barrier(ProcInfo.CommCAL);			// ͬ�����MPI���̵�thread

	// �������ڿ���
	rtclock_gettime(&ts);
	for(step=1; step<=nstep; step++)
	{
		emt_comm();							// emt֮��MPIͨ�źͼ���
		emtphy_comm();						// emt��emtphy֮��MPIͨ��
		do_calc(CalcNum);
		emtio_comm();						// emt��emtio֮��MPIͨ��
		update_buf();
	}
	rtclock_gettime(&tc);

	if (0==ProcInfo.Id)
	{
		printf( "Time step: %f\n",tstep);
		printf( "Total steps: %d\n",nstep);
		printf( "Elapsed time: %f\n",tc-ts);
	}
	isCalculationOver = 1;
}

#endif //_LINUX

#ifdef SHM_EMTIO
void init_shm(int no)
{
	int i;
	int shm_id;
	key_t key;
	char pathname[30];
	struct shm_blk *pblk;
	FILE *fp_shm;

	shm_ctrl_per_task.shm_size = SHM_SIZE_PER_TASK;
	shm_ctrl_per_task.mem	= NULL;
	shm_ctrl_per_task.blk_num  = SHM_BLOCK_NUM;
	shm_ctrl_per_task.blk_size = SHM_BLOCK_SIZE;
	shm_ctrl_per_task.prod_ind = 0;
	shm_ctrl_per_task.cons_ind = 0;

	sprintf(pathname, "emtio_shm_%d", no);

	fp_shm = fopen(pathname, "w");
	if (fp_shm == NULL)
	{
		printf("Can not open %s\n", pathname);
		return;
	}
	else
	{
		fclose(fp_shm);
	}

	key = ftok(pathname, 0x03);	 
	if(key == -1)	 
	{	 
		printf("ftok failed in emt, no=%d, errno=%d\n", no, errno);
		return;	 
	}	 

	shm_id = shmget(key, shm_ctrl_per_task.shm_size, 0666|IPC_CREAT);
	if (shm_id == -1)
	{
		printf("shmget failed in emt, no=%d, errno=%d\n", no, errno);
	}
	else
	{	
		shm_ctrl_per_task.mem = shmat(shm_id, NULL, 0);
		if (shm_ctrl_per_task.mem == NULL)
		{
			printf("shmat failed in emt, no=%d, errno=%d\n", no, errno);
		}
		else
		{

			for (i = 0; i < shm_ctrl_per_task.blk_num; i++)
			{
				pblk = (struct shm_blk *)((unsigned char*)shm_ctrl_per_task.mem + shm_ctrl_per_task.blk_size * i);
				pblk->sem = SHM_READ_COMPLETE;
				memset(pblk->data, 0, sizeof(double) * EMT_COMM_NUM);
			}
		}
	}

	return;
}

void deinit_shm(int no)
{
	int shm_id;
	key_t key;
	char pathname[30];
	int ret;

	sprintf(pathname, "emtio_shm_%d", no) ;	 
	key = ftok(pathname, 0x03);	 
	if(key == -1)	 
	{	 
		printf("shmdt failed in emt1, no=%d, errno=%d\n", no, errno);
		return;	 
	}	 

	shm_id = shmget(key, 0, 0);
	if (shm_id == -1)
	{
		printf("shmget failed in emt1, no=%d, errno=%d\n", no, errno);
		return;
	}

	ret = shmdt(shm_ctrl_per_task.mem);
	if (ret)
	{
		printf("shmdt failed in emt1, no=%d, errno=%d\n", no, errno);
		return;
	}

	ret=shmctl(shm_id, IPC_RMID, 0);	
	if (ret == -1)
	{
		printf("shmctl failed in emt1, no=%d, errno=%d\n", no, errno);
	}

	return;
}

#endif

// �������н��̵����ͣ����ɲ�ͬ��ͨ����
int init_comm()
{
	int *IDataRecv;
	int *ProcIdx;
	int i, j, Ntype;

	//IDataRecv = new int[ProcInfo.NumProc];
	IDataRecv = malloc(ProcInfo.NumProc*sizeof(int));
	//ProcIdx = new int[ProcInfo.NumProc];
	ProcIdx = malloc(ProcInfo.NumProc*sizeof(int));

	MPI_Group GroupAll,GroupST,GroupEMT,GroupEMTCAL,GroupCAL;		//ȫ�������顢������̬�����顢�����̬�����顢�����̬���������顢���������

	MPI_Comm_group(MPI_COMM_WORLD, &GroupAll);

	// ÿ�����̶������Լ������ͺţ����ռ����н��̵����ͺ�
	Ntype=SIMUTYPE_EMTSUB;											//���������ͺ�
	MPI_Allgather( &Ntype, 1, MPI_INTEGER, IDataRecv, 1, MPI_INTEGER, MPI_COMM_WORLD);

	// ���м������ͨ����
	ProcInfo.NumProcCAL = 0;
	ProcInfo.NumProcEMTIO = 0;
	for (i=0; i<ProcInfo.NumProc; i++)
	{
		if (IDataRecv[i] <= 200)									// ���м������
		{
			ProcIdx[ProcInfo.NumProcCAL] = i;
			ProcInfo.NumProcCAL++;
		}

		if (IDataRecv[i] == SIMUTYPE_EMTIO)							// һ������ֻ��1��emtio����
		{
			ProcInfo.EMTIOId = i;
			ProcInfo.NumProcEMTIO++;
		}
	}
	if (ProcInfo.NumProcCAL>0)
	{
		MPI_Group_incl(GroupAll, ProcInfo.NumProcCAL, ProcIdx, &GroupCAL);		//�γɼ��������
		MPI_Comm_create(MPI_COMM_WORLD, GroupCAL, &ProcInfo.CommCAL);			//�γɼ���ͨ����
	}
	else
	{
		ProcInfo.CommCAL = MPI_COMM_NULL;
	}
	MPI_Comm_rank( ProcInfo.CommCAL, &ProcInfo.CALId);

	// ������̬ͨ����
	ProcInfo.NumProcST = 0;
	for (i=0; i<ProcInfo.NumProc; i++)
	{
		if (IDataRecv[i]<=100)													// ���л�����̬����
		{
			ProcIdx[ProcInfo.NumProcST] = i;
			ProcInfo.NumProcST++;
		}
	}
	if (ProcInfo.NumProcST>0)
	{
		//�����Ѵ��ڵĽ�����GroupCAL�����µĽ�����GroupST
		MPI_Group_incl(GroupCAL, ProcInfo.NumProcST, ProcIdx, &GroupST);		//�γɻ�����̬������
		MPI_Comm_create(ProcInfo.CommCAL, GroupST, &ProcInfo.CommST);			//�γɻ�����̬ͨ����
	}
	else
	{
		ProcInfo.CommST = MPI_COMM_NULL;
	}

	// �����̬ͨ����
	ProcInfo.NumProcEMT = 0;
	for (i=0; i<ProcInfo.NumProc; i++)
	{
		if ((IDataRecv[i]>100) && (IDataRecv[i]<=200))
		{
			ProcIdx[ProcInfo.NumProcEMT] = i;
			ProcInfo.NumProcEMT++;
		}
	}	
	if (ProcInfo.NumProcEMT>0)
	{
		//�����Ѵ��ڵĽ�����GroupCAL�����µĽ�����GroupEMT
		MPI_Group_incl(GroupCAL, ProcInfo.NumProcEMT, ProcIdx, &GroupEMT);		//�γɵ����̬������
		MPI_Comm_create(ProcInfo.CommCAL, GroupEMT, &ProcInfo.CommEMT);			//�γɵ����̬ͨ����
	}
	else
	{
		ProcInfo.CommEMT=MPI_COMM_NULL;
	}
	MPI_Comm_rank( ProcInfo.CommEMT, &ProcInfo.EMTId);

	// �����̬����ͨ���壨�������̬������
	ProcInfo.NumProcEMTCAL = 0;											//�����̬������
	ProcInfo.NumProcEMTPhy = 0;											//�����̬����ӿ���Ŀ
	for (i=0; i<ProcInfo.NumProc; i++)
	{
		if (SIMUTYPE_EMTSUB == IDataRecv[i])
		{
			ProcIdx[ProcInfo.NumProcEMTCAL] = i-ProcInfo.NumProcST;		//�����������ڵ����̬�������еı�ʶ�ţ�Ĭ�ϲ�ͬ���ͽ��̵�����˳����emt.h�ж����˳��һ��
			ProcInfo.NumProcEMTCAL++;
		}
		if (SIMUTYPE_EMTPHY == IDataRecv[i])
		{
			ProcInfo.NumProcEMTPhy++;
		}
	}
	if (ProcInfo.NumProcEMTCAL>0)
	{
		//�����Ѵ��ڵĽ�����GroupEMT�����µĽ�����GroupEMTCAL
		MPI_Group_incl(GroupEMT, ProcInfo.NumProcEMTCAL, ProcIdx, &GroupEMTCAL);	//�γɵ����̬����������
		MPI_Comm_create(ProcInfo.CommEMT, GroupEMTCAL, &ProcInfo.CommEMTCAL);		//�γɵ����̬����ͨ����
	}
	else
	{
		ProcInfo.CommEMTCAL=MPI_COMM_NULL;
	}
	MPI_Comm_rank( ProcInfo.CommEMTCAL, &ProcInfo.EMTCALId);

#ifdef SHM_EMTIO
	/* init shm used by emt to send data to emtio */
	if (ProcInfo.NumProcEMTIO > 0)
	{
		init_shm(ProcInfo.EMTCALId);
	}
#endif

	// ͳ����������ӿڽ��̺�
	if (ProcInfo.NumProcEMTPhy>0)
	{
		//ProcInfo.EMTPhyId = new int[ProcInfo.NumProcEMTPhy];
		ProcInfo.EMTPhyId = malloc(ProcInfo.NumProcEMTPhy * sizeof(int));
		j = 0;
		for (i=0; i<ProcInfo.NumProc; i++)
		{
			if (SIMUTYPE_EMTPHY == IDataRecv[i])
			{
				ProcInfo.EMTPhyId[j] = i;
				j++;
			}
		}
	}

	//�ͷŽ�����
	MPI_Group_free(&GroupAll);
	if(ProcInfo.NumProcCAL>0) MPI_Group_free(&GroupCAL);
	if(ProcInfo.NumProcST>0) MPI_Group_free(&GroupST);
	if(ProcInfo.NumProcEMT>0) MPI_Group_free(&GroupEMT);
	if(ProcInfo.NumProcEMTCAL>0) MPI_Group_free(&GroupEMTCAL);

	//delete[] IDataRecv;
	free(IDataRecv);
	//delete[] ProcIdx;
	free(ProcIdx);

	printf("emt, NumProc: %d, NumProcCAL: %d, NumProcST: %d, NumProcEMT: %d, NumProcEMTCAL: %d, NumProcEMTPhy: %d, NumProcEMTIO: %d\n", ProcInfo.NumProc, ProcInfo.NumProcCAL, ProcInfo.NumProcST, ProcInfo.NumProcEMT, ProcInfo.NumProcEMTCAL, ProcInfo.NumProcEMTPhy, ProcInfo.NumProcEMTIO);
	printf("emt, Id: %d, CALId; %d, EMTId: %d, EMTCALId: %d\n", ProcInfo.Id, ProcInfo.CALId, ProcInfo.EMTId, ProcInfo.EMTCALId);

	return 0;
}

int finalize_comm()
{
	//if (ProcInfo.NumProcEMTPhy>0) delete[] ProcInfo.EMTPhyId;
	if (ProcInfo.NumProcEMTPhy>0) free(ProcInfo.EMTPhyId);

#ifdef SHM_EMTIO
	if (ProcInfo.NumProcEMTIO > 0)
	{
		deinit_shm(ProcInfo.EMTCALId);
	}
#endif

	return 0;
}



#include <signal.h>
static int rt_core_id = 0; 
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

int main(int argc,char *argv[])
{
	
	
	int core = 9 ;	
	int count=0;
	struct sched_param parm;
	struct rlimit rlimit_new;
	
	//rt_init(5);
	if (signal(SIGINT, sig_usr) == SIG_ERR)
	{
		printf("Cannot catch SIGINT\n");
	}
	if(argc > 2)
	{
		count=atoi(argv[2]);
		warm=atoi(argv[3]);
		
		printf("count=%dwarm =%d\n",count,warm);


	}else if(argc > 1)
	{
		count=atoi(argv[1]);
	}
	if(count > 0)
		nstep = count;
	else
		nstep = CALCU_NUM;

	printf("test count =%u \n",nstep);
	tstep = 50000;

	isCalculationOver = 0;

	memset(&ProcInfo, 0, sizeof(struct SPROCINFO));

	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &ProcInfo.Id);
	MPI_Comm_size( MPI_COMM_WORLD, &ProcInfo.NumProc);

	init_comm();

	init_buf();

	MPI_Barrier(MPI_COMM_WORLD);

	
#ifdef _LINUX

	thread_emt((void * )0);

#else //_LINUX

	thread_emt();

#endif //_LINUX

	MPI_Barrier(MPI_COMM_WORLD);

#ifdef _LINUX
	if (0==ProcInfo.Id)
	{
		if ((fp = fopen("log", "a")) != NULL)
		{
			struct timeval nCurr;
			struct tm* pTm;
			char str[256], sTime[128];

			gettimeofday(&nCurr, NULL);
			pTm = localtime(&nCurr.tv_sec);
			strftime(sTime, sizeof(sTime), "%Y-%m-%d %H:%M:%S    ", pTm);

			fprintf(fp, sTime);
			sprintf(str, "%lld  %lld  %lld  %d  %lf  %lf  %lf  %lf\n", tstep, nstep, tc-ts, cnt, tmax_ctrl, tmin_ctrl, tmax_cal, tmin_cal);
			fprintf(fp, str);
			fclose(fp);
		}
	}
#endif //_LINUX

	finalize_buf();
	finalize_comm();

	MPI_Finalize();

	munlockall();
	//release_rtcore(core);
	//rt_core_id = 0;

	return 0;
}
