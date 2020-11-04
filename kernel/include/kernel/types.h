#ifndef __TYPES_H__
#define __TYPES_H__

#define BITS_PER_LONG 32

#define BITS_TO_LONGS(bits) \
    (((bits) + BITS_PER_LONG - 1)/BITS_PER_LONG)
#define DECLARE_BITMAP(name,bits) \
    unsigned long name[BITS_TO_LONGS(bits)]
#endif