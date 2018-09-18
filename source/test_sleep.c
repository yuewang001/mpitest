/*
debug only sleep ,get time,
no MPI, no multiple process
# ./mpirun -np 6 emt
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <i_malloc.h>
#include "pub.h"

#include "windriver.h"  

#define _GNU_SOURCE

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
#include <sys/timeb.h>
#include <sys/time.h> //for gettimeofday
#include <unistd.h>   //for sleep


int main(int argc,char *argv[])
{


	int myrank=-1,count;

	// 閸掓繂顫愰崠鏈歅I閻滎垰顣�
	//娴犲氦绻栭柌灞藉讲娴犮儳婀呴崚甯礉娑擄拷閼哥悎PI閹稿洣鎶ら弰鐤I_鐠虹喍绔存稉顏勩亣閸愭瑥鐡уВ锟�
	//瀵拷婵娈戦崡鏇＄槤閿涘苯鎮楅棃銏ゅ厴閺勵垰鐨崘娆忕摟濮ｅ秲锟斤拷




	// 閼惧嘲褰囪ぐ鎾冲鏉╂稓鈻奸崷銊╋拷姘繆閸ｂ垥PI_COMM_WORLD娑擃厾娈戞潻娑氣柤閸欙拷
#ifdef MPI_TEST
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	printf("myrank = %d: Hello, world!\n", myrank);
#endif

	int processID=myrank;

	int ms;  //
	int nm;//sleep 50 us

#define RT_TIME 1
	printf("argc=%d\n",argc);
	if(argc==4)
	{
		printf("argv[0]:%s\n",argv[0]);
		count=atoi(argv[1]);
		nm=atoi(argv[2]);	
		ms=atoi(argv[3]);
		printf("count=%d\n",count);
		printf("nm=%d\n",nm);
		printf("ms=%d\n",ms);


	}
	else
	{
		printf("usage: test_sleep  $count_repeat of 10000  $nm(us num of sleep) $ms(max sleep ux) \n",count);

		return(1);

	}

	printf("test count =%d \n",count);
	unsigned long long us_rt,us_linux,us_rt2,us_linux2,tmp,df1,df2;
	

//#define RT_TIME 1

#ifdef MPI_TEST
	MPI_Barrier(MPI_COMM_WORLD);
#endif

	int j=0;
	int error_count=0;
	printf("test count =%d \n",count);
	//count=1;
	for(; j<count; j++)
	{
		printf("j =%d \n",j);
		printf("count =%d \n",count);
		int substep=10000;
		//sleep nm  micro secconds
		unsigned long long  Timetmp[10000] = {0};

		int x=0;

#ifdef RT_TIME
		printf("call real time sleep and get time function--\n");
		cplusplus_rtclock_sleep(nm);
		cplusplus_rtclock_gettime(&Timetmp[x]);
		/*
		struct timeval timestat;
		gettimeofday(&timestat,NULL);
		Timetmp[x]=timestat.tv_sec*1000000+timestat.tv_usec;
		*/


#else
		printf("call linux sleep and get time function--\n");
		usleep(nm);
		struct timeval timestat;
		gettimeofday(&timestat,NULL);
		Timetmp[x]=timestat.tv_sec*1000000+timestat.tv_usec;
		substep=60;
#endif

		x++;
		while(x<substep)
		{
#ifdef RT_TIME


			cplusplus_rtclock_sleep(nm);
			cplusplus_rtclock_gettime(&Timetmp[x]);

			/*
			struct timeval timestat;
			gettimeofday(&timestat,NULL);
			Timetmp[x]=timestat.tv_sec*1000000+timestat.tv_usec;
			*/
#else


			struct timeval timestat;
			usleep(nm);
			gettimeofday(&timestat,NULL);
			Timetmp[x]=timestat.tv_sec*1000000+timestat.tv_usec;
#endif

			//Timetmp[x]=round(Timetmp[x]);

			//printf("Timetmp[%d]=%lld\n",x,Timetmp[x] );
			x++;

		}

		int i;

		for(i=0; i< x-1; i++)
		{
			//printf("ms=%d\n",ms);

			if (Timetmp[i+1] - Timetmp[i] > ms)
			{
				printf("%d:   %lld  %lld %lld\n",i,Timetmp[i],Timetmp[i+1],Timetmp[i+1]-Timetmp[i] );
				error_count++;
			}

			//printf("%d:   %lld  %lld %lld  \n",i,Timetmp[i],Timetmp[i+1],Timetmp[i+1]-Timetmp[i] );


		}

		printf("10000*%d process done\n",j );

	}

	double error_percent=error_count/count;


	printf("error_count: %d   total count:%d\n",error_count,10000*count);
	printf("error count(millionth): %f \n",error_percent);
	printf("myrank: %d process done\n",processID);
#ifdef MPI_TEST
	MPI_Barrier(MPI_COMM_WORLD);


	MPI_Finalize();
#endif


	return 0;
}
