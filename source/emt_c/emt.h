
#define CalcNum			30
#define EMT_COMM_NUM		24

enum SIMUTYPE_ALL
{
	// 1~100为机电进程
	SIMUTYPE_STCTRL=1,		// 机电主控
	SIMUTYPE_STSUB,			// 机电子网
	SIMUTYPE_STUD,			// 机电UD模型
	SIMUTYPE_STMAT,			// 机电Matlab模型
	SIMUTYPE_STPHY,			// 机电物理接口

	// 101~200为电磁进程
	SIMUTYPE_EMTSUB=101,	// 电磁子网
	SIMUTYPE_EMTUD,			// 电磁UD模型
	SIMUTYPE_EMTMAT,		// 电磁Matlab模型
	SIMUTYPE_EMTPHY,		// 电磁物理接口
	
	// 201以上为IO进程
	SIMUTYPE_STIO=201,		// 机电IO进程
	SIMUTYPE_EMTIO			// 电磁IO进程
};

// MPI通信域信息
struct SPROCINFO {
	MPI_Comm CommCAL;
	MPI_Comm CommST;
	MPI_Comm CommEMT;
	MPI_Comm CommEMTCAL;	
	
	int NumProc;
	int NumProcCAL;
	int NumProcST;
	int NumProcEMT;
	int NumProcEMTCAL;
	int NumProcEMTPhy;
	int NumProcEMTIO;

	int Id;					// 本进程在所有进程中的ID
	int CALId;				// 本进程在CommCAL域中的ID
	int EMTId;				// 本进程在CommEMT域中的ID
	int EMTCALId;			// 本进程在CommEMTCAL域中的ID

	int* EMTPhyId;			// 本进程对应的EMTPhy进程ID
	int EMTIOId;			// 本进程对应的EMTIO进程ID
} ProcInfo;


#define SHM_EMTIO

#ifdef SHM_EMTIO
#define SHM_READ_COMPLETE	0x5A5A5A5A
#define SHM_WRITE_COMPLETE	0xA5A5A5A5

#define SHM_SIZE_PER_TASK   4096
#define SHM_BLOCK_SIZE		256
#define SHM_BLOCK_NUM		(SHM_SIZE_PER_TASK/SHM_BLOCK_SIZE)

struct shm_blk
{
	unsigned int sem;
	double data[EMT_COMM_NUM];
};

struct shm_per_task
{
	int shm_size;	/* shm size used by shmget */
	void *mem;		/* address of shm */
	int blk_num;	/* shm_size is divided into blk_num blocks */
	int blk_size;	/* size of each block */

	int prod_ind;	/* block index for producer task */
	int cons_ind;	/* block index for consumer task */
};
#endif

