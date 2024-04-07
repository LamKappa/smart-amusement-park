/*
 * This header was generated from the Linux kernel headers by update_headers.py,
 * to provide necessary information from kernel to userspace, such as constants,
 * structures, and macros, and thus, contains no copyrightable information.
 */
#ifndef _IPT_TTL_H
#define _IPT_TTL_H
#include <linux/types.h>
enum {
	IPT_TTL_SET = 0,
	IPT_TTL_INC,
	IPT_TTL_DEC
};
#define IPT_TTL_MAXMODE	IPT_TTL_DEC
struct ipt_TTL_info {
	__u8	mode;
	__u8	ttl;
};
#endif
