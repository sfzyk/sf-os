#ifndef __BITOPS_H__
#define __BITOPS_H__
#define ADDR (*(volatile long *) addr)


static inline void  set_bit(int nr,volatile unsigned long * addr){
    __asm__ volatile (
        "lock;btsl %1,%0"
        :"=m"(ADDR)
        :"Ir"(nr)
    );
}


static inline void  non_atomic_set_bit(int nr,volatile unsigned long * addr){
    __asm__ volatile (
        "btsl %1,%0"
        :"=m"(ADDR)
        :"Ir"(nr)
    );
}


static inline void clear_bit(int nr , volatile unsigned long *addr){
    __asm__ volatile(
        "lock;btrl %1,%0"
        :"=m"(ADDR)
        :"Ir"(nr)
    );
}


static inline void non_atomic_clear_bit(int nr , volatile unsigned long *addr){
    __asm__ volatile(
        "btrl %1,%0"
        :"=m"(ADDR)
        :"Ir"(nr)
    );
}
/*find first bit set*/

static inline int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

/*
 * fls: find last bit set.
 */

static __inline__ int fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}


static inline int test_and_clear_bit(int nr, volatile unsigned long * addr)
{
	int oldbit;

	__asm__ __volatile__(
		"btrl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"Ir" (nr) : "memory");
	return oldbit;
}

/**
 * __test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.  
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline int __test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	int oldbit;

	__asm__(
		"btrl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"Ir" (nr));
	return oldbit;
}

#ifdef __compiler_offsetof
#define offsetof(TYPE, member) __compiler_offsetof(TYPE,member) 
#else 
#define offsetof(TYPE, member) (char *)(&((TYPE*)0)->member) 
#endif 

#endif 