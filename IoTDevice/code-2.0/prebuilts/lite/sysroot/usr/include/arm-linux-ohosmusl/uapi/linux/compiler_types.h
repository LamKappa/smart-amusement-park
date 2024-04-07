/*
 * This header was generated from the Linux kernel headers by update_headers.py,
 * to provide necessary information from kernel to userspace, such as constants,
 * structures, and macros, and thus, contains no copyrightable information.
 */
#ifndef __LINUX_COMPILER_TYPES_H
#define __LINUX_COMPILER_TYPES_H
#ifndef __ASSEMBLY__
#if 0
# define __user		__attribute__((noderef, address_space(1)))
# define __kernel	__attribute__((address_space(0)))
# define __safe		__attribute__((safe))
# define __force	__attribute__((force))
# define __nocast	__attribute__((nocast))
# define __iomem	__attribute__((noderef, address_space(2)))
# define __must_hold(x)	__attribute__((context(x,1,1)))
# define __acquires(x)	__attribute__((context(x,0,1)))
# define __releases(x)	__attribute__((context(x,1,0)))
# define __acquire(x)	__context__(x,1)
# define __release(x)	__context__(x,-1)
# define __cond_lock(x,c)	((c) ? ({ __acquire(x); 1; }) : 0)
# define __percpu	__attribute__((noderef, address_space(3)))
# define __rcu		__attribute__((noderef, address_space(4)))
# define __private	__attribute__((noderef))
extern void __chk_user_ptr(const volatile void __user *);
extern void __chk_io_ptr(const volatile void __iomem *);
# define ACCESS_PRIVATE(p, member) (*((typeof((p)->member) __force *) &(p)->member))
#else
# ifdef STRUCTLEAK_PLUGIN
#  define __user __attribute__((user))
# else
#  define __user
# endif
# define __kernel
# define __safe
# define __force
# define __nocast
# define __iomem
# define __chk_user_ptr(x) (void)0
# define __chk_io_ptr(x) (void)0
# define __builtin_warning(x, y...) (1)
# define __must_hold(x)
# define __acquires(x)
# define __releases(x)
# define __acquire(x) (void)0
# define __release(x) (void)0
# define __cond_lock(x,c) (c)
# define __percpu
# define __rcu
# define __private
# define ACCESS_PRIVATE(p, member) ((p)->member)
#endif
#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)
#ifdef __KERNEL__
#ifdef __clang__
#include <linux/compiler-clang.h>
#elif defined(__INTEL_COMPILER)
#include <linux/compiler-intel.h>
#elif defined(__GNUC__)
#include <linux/compiler-gcc.h>
#else
#error "Unknown compiler"
#endif
#ifdef CONFIG_HAVE_ARCH_COMPILER_H
#include <asm/compiler.h>
#endif
struct ftrace_branch_data {
	const char *func;
	const char *file;
	unsigned line;
	union {
		struct {
			unsigned long correct;
			unsigned long incorrect;
		};
		struct {
			unsigned long miss;
			unsigned long hit;
		};
		unsigned long miss_hit[2];
	};
};
struct ftrace_likely_data {
	struct ftrace_branch_data	data;
	unsigned long			constant;
};
#define __deprecated
#define __deprecated_for_modules
#endif
#endif
#ifndef __designated_init
# define __designated_init
#endif
#ifndef __latent_entropy
# define __latent_entropy
#endif
#ifndef __randomize_layout
# define __randomize_layout __designated_init
#endif
#ifndef __no_randomize_layout
# define __no_randomize_layout
#endif
#ifndef randomized_struct_fields_start
# define randomized_struct_fields_start
# define randomized_struct_fields_end
#endif
#ifndef __visible
#define __visible
#endif
#ifndef __assume_aligned
#define __assume_aligned(a, ...)
#endif
#ifndef asm_volatile_goto
#define asm_volatile_goto(x...) asm goto(x)
#endif
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define __native_word(t) \
	(sizeof(t) == sizeof(char) || sizeof(t) == sizeof(short) || \
	 sizeof(t) == sizeof(int) || sizeof(t) == sizeof(long))
#ifndef __attribute__((__const__))
#define __attribute__((__const__))	__attribute__((__const__))
#endif
#ifndef __noclone
#define __noclone
#endif
#ifndef __diag
#define __diag(string)
#endif
#ifndef __diag_GCC
#define __diag_GCC(version, severity, string)
#endif
#ifndef __copy
# define __copy(symbol)
#endif
#define __diag_push()	__diag(push)
#define __diag_pop()	__diag(pop)
#define __diag_ignore(compiler, version, option, comment) \
	__diag_ ## compiler(version, ignore, option)
#define __diag_warn(compiler, version, option, comment) \
	__diag_ ## compiler(version, warn, option)
#define __diag_error(compiler, version, option, comment) \
	__diag_ ## compiler(version, error, option)
#define __pure			__attribute__((pure))
#define __aligned(x)		__attribute__((aligned(x)))
#define __aligned_largest	__attribute__((aligned))
#define __printf(a, b)		__attribute__((format(printf, a, b)))
#define __scanf(a, b)		__attribute__((format(scanf, a, b)))
#define __maybe_unused		__attribute__((unused))
#define __always_unused		__attribute__((unused))
#define __mode(x)		__attribute__((mode(x)))
#define __malloc		__attribute__((__malloc__))
#define __used			__attribute__((__used__))
#define __noreturn		__attribute__((noreturn))
#define __packed		__attribute__((packed))
#define __weak			__attribute__((weak))
#define __alias(symbol)		__attribute__((alias(#symbol)))
#define __cold			__attribute__((cold))
#define __section(S)		__attribute__((__section__(#S)))
#ifdef CONFIG_ENABLE_MUST_CHECK
#define __must_check		__attribute__((warn_unused_result))
#else
#define __must_check
#endif
#if defined(CC_USING_HOTPATCH) && !defined(__CHECKER__)
#define notrace			__attribute__((hotpatch(0, 0)))
#else
#define notrace			__attribute__((no_instrument_function))
#endif
#define __naked			__attribute__((naked)) notrace
#define __compiler_offsetof(a, b)	__builtin_offsetof(a, b)
#ifdef __GNUC_STDC_INLINE__
# define __gnu_inline	__attribute__((gnu_inline))
#else
# define __gnu_inline
#endif
#ifndef __norecordmcount
#define __norecordmcount
#endif
#ifndef __nocfi
#define __nocfi
#endif
#if !defined(CONFIG_ARCH_SUPPORTS_OPTIMIZED_INLINING) || \
	!defined(CONFIG_OPTIMIZE_INLINING)
#define inline \
	inline __attribute__((always_inline, unused)) notrace __gnu_inline
#else
#define inline inline	__attribute__((unused)) notrace __gnu_inline
#endif
#define __inline__ inline
#define __inline inline
#define noinline	__attribute__((noinline))
#ifndef __always_inline
#define __always_inline inline __attribute__((always_inline))
#endif
#define noinline_for_stack noinline
#endif
