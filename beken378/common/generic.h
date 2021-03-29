#ifndef _GENERIC_H_
#define _GENERIC_H_
#include <stdbool.h>
#include "include.h"

typedef void (*FUNCPTR)(void);
typedef void (*FUNC_1PARAM_PTR)(void *ctxt);
typedef void (*FUNC_2PARAM_PTR)(void *arg, uint8_t vif_idx);

#ifndef MAX
#define MAX(x, y)                  (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y)                  (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x, y)                  (((x) > (y)) ? (x) : (y))
#endif

#ifndef min
#define min(x, y)                  (((x) < (y)) ? (x) : (y))
#endif

#define min3(x, y, z) ({			\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	typeof(z) _min3 = (z);			\
	(void) (&_min1 == &_min2);		\
	(void) (&_min1 == &_min3);		\
	_min1 < _min2 ? (_min1 < _min3 ? _min1 : _min3) : \
		(_min2 < _min3 ? _min2 : _min3); })

#define max3(x, y, z) ({			\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	typeof(z) _max3 = (z);			\
	(void) (&_max1 == &_max2);		\
	(void) (&_max1 == &_max3);		\
	_max1 > _max2 ? (_max1 > _max3 ? _max1 : _max3) : \
		(_max2 > _max3 ? _max2 : _max3); })

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max/clamp at all, of course.
 */
#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2; })


/*
 * swap - swap value of @a and @b
 */
#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)


extern void bk_printf(const char *fmt, ...);
#define as_printf (bk_printf("%s:%d\r\n",__FUNCTION__,__LINE__))

#define IS_ALIGNED(x, a)		(((x) & ((typeof(x))(a) - 1)) == 0)

//##define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

/* The `const' in roundup() prevents gcc-3.3 from calling __divdi3 */
#define roundup(x, y) (					\
{							\
	const typeof(y) __y = y;			\
	(((x) + (__y - 1)) / __y) * __y;		\
}							\
)
#define rounddown(x, y) (				\
{							\
	typeof(x) __x = (x);				\
	__x - (__x % (y));				\
}							\
)
#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(divisor) __divisor = divisor;		\
	(((x) + ((__divisor) / 2)) / (__divisor));	\
}							\
)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#ifdef container_of
#undef container_of
#endif
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))


#if (0 == CFG_RELEASE_FIRMWARE)
#define ASSERT_EQ(a, b)                             \
{                                                   \
    if ((a) != (b))                                 \
    {                                               \
        bk_printf("%s:%d %d!=%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_NE(a, b)                             \
{                                                   \
    if ((a) == (b))                                 \
    {                                               \
        bk_printf("%s:%d %d==%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_GT(a, b)                             \
{                                                   \
    if ((a) <= (b))                                 \
    {                                               \
        bk_printf("%s:%d %d<=%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_GE(a, b)                             \
{                                                   \
    if ((a) < (b))                                 \
    {                                               \
        bk_printf("%s:%d %d<%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_LT(a, b)                             \
{                                                   \
    if ((a) >= (b))                                 \
    {                                               \
        bk_printf("%s:%d %d>=%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT_LE(a, b)                             \
{                                                   \
    if ((a) > (b))                                 \
    {                                               \
        bk_printf("%s:%d %d>%d\r\n",__FUNCTION__,__LINE__, (a), (b)); \
        while(1);                                   \
    }                                               \
}
#define ASSERT(exp)                                 \
{                                                   \
    if ( !(exp) )                                   \
    {                                               \
    	as_printf;							     	\
        while(1);                                   \
    }                                               \
}
#else
#define ASSERT_EQ(exp)
#define ASSERT_NE(exp)
#define ASSERT_GT(exp)
#define ASSERT_GE(exp)
#define ASSERT_LT(exp)
#define ASSERT_LE(exp)
#define ASSERT(exp)
#endif

#define BUG_ON(exp)               ASSERT(!(exp))

#ifndef NULL
#define NULL                     (0L)
#endif

#ifndef NULLPTR
#define NULLPTR                  ((void *)0)
#endif

#define BIT(i)                   (1UL << (i))

static inline __uint16_t __bswap16(__uint16_t _x)
{

	return ((__uint16_t)((_x >> 8) | ((_x << 8) & 0xff00)));
}

static inline __uint32_t __bswap32(__uint32_t _x)
{

	return ((__uint32_t)((_x >> 24) | ((_x >> 8) & 0xff00) |
	    ((_x << 8) & 0xff0000) | ((_x << 24) & 0xff000000)));
}

static inline __uint64_t __bswap64(__uint64_t _x)
{

	return ((__uint64_t)((_x >> 56) | ((_x >> 40) & 0xff00) |
	    ((_x >> 24) & 0xff0000) | ((_x >> 8) & 0xff000000) |
	    ((_x << 8) & ((__uint64_t)0xff << 32)) |
	    ((_x << 24) & ((__uint64_t)0xff << 40)) |
	    ((_x << 40) & ((__uint64_t)0xff << 48)) | ((_x << 56))));
}

#define __swab16(x) __bswap16((__u8 *)&(x))
#define __swab32(x) __bswap32((__u8 *)&(x))

#define cpu_to_le16(x)   (x)
#define cpu_to_le32(x)   (x)

#define __cpu_to_be32(x) __swab32((x))
#define __be32_to_cpu(x) __swab32((x))
#define __cpu_to_be16(x) __swab16((x))
#define __be16_to_cpu(x) __swab16((x))


#define	__htonl(_x)	__bswap32(_x)
#define	__htons(_x)	__bswap16(_x)
#define	__ntohl(_x)	__bswap32(_x)
#define	__ntohs(_x)	__bswap16(_x)

#define ___htonl(x) __cpu_to_be32(x)
#define ___htons(x) __cpu_to_be16(x)
#define ___ntohl(x) __be32_to_cpu(x)
#define ___ntohs(x) __be16_to_cpu(x)

#if (!CFG_SUPPORT_RTT)
#define htons(x) __htons(x)
#define ntohs(x) __ntohs(x)
#define htonl(x) __htonl(x)
#define ntohl(x) __ntohl(x)
#endif

#endif // _GENERIC_H_

