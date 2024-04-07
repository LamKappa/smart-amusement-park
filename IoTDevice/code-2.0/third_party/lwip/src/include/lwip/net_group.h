/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef LOSCFG_NET_CONTAINER
#ifndef _NET_GROUP_H
#define _NET_GROUP_H
#include "lwip/arch.h"

struct netif;
struct ip_pcb;

struct net_group {
    u8_t netif_num;
    /** The default network interface. */
    struct netif *netif_default;
    /** The list of network interfaces. */
    struct netif *netif_list;
    struct netif *loop_netif;
};

struct net_group_ops {
    struct net_group *(*get_curr_process_net_group)(void);
    void (*set_netif_net_group)(struct netif *, struct net_group *);
    struct net_group *(*get_net_group_from_netif)(struct netif *);
    void (*set_ippcb_net_group)(struct ip_pcb *, struct net_group *);
    struct net_group *(*get_net_group_from_ippcb)(struct ip_pcb *);
};

struct net_group *get_root_net_group(void);
struct net_group *get_curr_process_net_group(void);
struct net_group_ops *get_default_net_group_ops(void);
void set_default_net_group_ops(struct net_group_ops *ops);
#endif
#endif /* _NET_GROUP_H */
