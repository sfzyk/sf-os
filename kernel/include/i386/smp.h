#ifndef __SMP_H__
#define __SMP_H__

#ifdef CONFIG_SMP
#define NR_CPUS		CONFIG_NR_CPUS
#else
#define NR_CPUS		1
#endif

#define getcpu() (0)

#define putcpu() do{}while(0)

#endif 