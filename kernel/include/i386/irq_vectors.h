#ifndef __IRQ_VECTORS_H__
#define __IRQ_VECTORS_H__

#define NR_VECTORS 256

#define FIRST_EXTERNAL_VECTOR 0x20
#define FIRST_SYSTEM_VECTOR 0xef


#define SYSCALL_VECTOR 0x80

#ifdef CONFIG_PCI_MSI
#define NR_IRQS FIRST_SYSTEM_VECTOR
#define NR_IRQ_VECTORS NR_IRQS
#else
#ifdef CONFIG_X86_IO_APIC
#define NR_IRQS 224
# if (224 >= 32 * NR_CPUS)
# define NR_IRQ_VECTORS NR_IRQS
# else
# define NR_IRQ_VECTORS (32 * NR_CPUS)
# endif
#else
#define NR_IRQS 16
#define NR_IRQ_VECTORS NR_IRQS
#endif
#endif

#endif