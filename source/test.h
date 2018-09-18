#ifndef __TEST_H__

#define RUN_LONG_TIME_TEST
#define CALCU_NUM 200000

#ifndef RUN_LONG_TIME_TEST
//#define STATISTICS_LOG
#endif

#ifdef RUN_LONG_TIME_TEST
extern void get_general_info(unsigned long long val);
extern void time_status_init(void);
extern void output_status_results(unsigned long long step);
#endif

#endif

