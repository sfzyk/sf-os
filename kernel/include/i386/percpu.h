#ifndef __PERCPU_H__
#define __PERCPU_H__

#ifdef __CONFIG_SMP__
    /* to do support smp */

#else

    #define DEFINE_PER_CPU(type, name)\
        __typeof__(type) per_cpu__##name

    #define __get_cpu_var(var)  per_cpu__##var 

#endif 

#endif 