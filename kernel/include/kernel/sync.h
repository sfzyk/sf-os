#ifndef __SYNC_H__
#define __SYNC_H__

#include <kernel/preempt.h>

/*smbody" why not add break_lock*/

typedef struct {
	volatile unsigned int slock;
	unsigned magic;
} spinlock_t;

typedef struct {
	volatile unsigned int lock;
	unsigned magic;
} rwlock_t;

#define SPINLOCK_MAGIC	0xdead4ead
#define SPINLOCK_MAGIC_INIT	 ,SPINLOCK_MAGIC
#define SPIN_LOCK_UNLOCKED (spinlock_t) { 1 SPINLOCK_MAGIC_INIT }

#define RWLOCK_MAGIC	0xdeaf1eed
#define RWLOCK_MAGIC_INIT	, RWLOCK_MAGIC
#define RW_LOCK_BIAS		 0x01000000

#define RW_LOCK_UNLOCKED (rwlock_t) { RW_LOCK_BIAS RWLOCK_MAGIC_INIT }

/*
 * Simple spin lock operations.  There are two variants, one clears IRQ's
 * on the local processor, one does not.
 *
 * We make no fairness assumptions. They have a cost.
 */
#define spin_lock_init(x)	do { *(x) = SPIN_LOCK_UNLOCKED; } while(0)
#define spin_is_locked(x)	(*(volatile signed char *)(&(x)->slock) <= 0)

#define rwlock_init(x)	do { *(x) = RW_LOCK_UNLOCKED; } while(0)
#define read_can_lock(x) ((int)(x)->lock > 0)
#define write_can_lock(x) ((x)->lock == RW_LOCK_BIAS)


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

static inline void _raw_spin_lock(spinlock_t *lock)
{
	__asm__ __volatile__(
		spin_lock_string
		:"=m" (lock->slock) : : "memory");
}

#define DEFINE_SPINLOCK(x) spinlock_t x = SPIN_LOCK_UNLOCKED

/* smbody : delete __acquire(lock); */
#define _spin_lock(lock)	\
do { \
	preempt_disable(); \
	_raw_spin_lock(lock); \
} while(0)

/* smbody : delete __release(lock); */
#define _spin_unlock(lock) \
do { \
	_raw_spin_unlock(lock); \
	preempt_enable(); \
} while (0)

/* smbody : delete __acquire(lock); */
#define _write_lock(lock) \
do { \
	preempt_disable(); \
	_raw_write_lock(lock); \
} while(0)

/* smbody : delete __acquire(lock); */
#define _read_lock(lock)	\
do { \
	preempt_disable(); \
	_raw_read_lock(lock); \
} while(0)

/* smbody : delete __release(lock); */
#define _write_unlock(lock) \
do { \
	_raw_write_unlock(lock); \
	preempt_enable(); \
} while(0)

/* smbody : delete __release(lock); */
#define _read_unlock(lock) \
do { \
	_raw_read_unlock(lock); \
	preempt_enable(); \
} while(0)

#define spin_lock(lock)		_spin_lock(lock)
#define spin_unlock(lock)	_spin_unlock(lock)

#define write_lock(lock)	_write_lock(lock)
#define write_unlock(lock)	_write_unlock(lock)

#define read_lock(lock)		_read_lock(lock)
#define read_unlock(lock)	_read_unlock(lock)

#endif