/*
 * This header was generated from the Linux kernel headers by update_headers.py,
 * to provide necessary information from kernel to userspace, such as constants,
 * structures, and macros, and thus, contains no copyrightable information.
 */
#ifndef _XT_TCPMSS_H
#define _XT_TCPMSS_H
#include <linux/types.h>
struct xt_tcpmss_info {
	__u16 mss;
};
#define XT_TCPMSS_CLAMP_PMTU 0xffff
#endif
