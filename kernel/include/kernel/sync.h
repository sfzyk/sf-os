#ifndef __SYNC_H__
#define __SYNC_H__

typedef struct {
	volatile unsigned int slock;
	unsigned magic;
} spinlock_t;

#define SPINLOCK_MAGIC	0xdead4ead
 
#define SPINLOCK_MAGIC_INIT	 ,SPINLOCK_MAGIC
 
#define SPIN_LOCK_UNLOCKED (spinlock_t) { 1 SPINLOCK_MAGIC_INIT }



/*
 * Simple spin lock operations.  There are two variants, one clears IRQ's
 * on the local processor, one does not.
 *
 * We make no fairness assumptions. They have a cost.
 */
#define spin_lock_init(x)	do { *(x) = SPIN_LOCK_UNLOCKED; } while(0)
#define spin_is_locked(x)	(*(volatile signed char *)(&(x)->slock) <= 0)

#define spin_lock_string \
	"\n1:\t" \
	"lock ; decb %0\n\t" \
	"jns 3f\n" \
	"2:\t" \
	"rep;nop\n\t" \
	"cmpb $0,%0\n\t" \
	"jle 2b\n\t" \
	"jmp 1b\n" \
	"3:\n\t"

#define spin_unlock_string \
	"movb $1,%0" \
		:"=m" (lock->slock) : : "memory"

#define spin_unlock_wait(x)	do { barrier(); } while(spin_is_locked(x))

static inline void _raw_spin_unlock(spinlock_t *lock)
{
	__asm__ __volatile__(
		spin_unlock_string
	);
}



#endif