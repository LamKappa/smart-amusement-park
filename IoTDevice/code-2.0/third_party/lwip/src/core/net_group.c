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
#include "lwip/net_group.h"
#include "lwip/netif.h"

static struct netif root_loop_netif = {0};
static struct net_group root_net_group = {
    .loop_netif = &root_loop_netif,
};

struct net_group *get_root_net_group(void)
{
    return &root_net_group;
}

struct net_group *get_curr_process_net_group(void)
{
    return get_default_net_group_ops()->get_curr_process_net_group();
}

static void do_set_netif_net_group(struct netif *netif, struct net_group *group)
{
    (void)netif;
    (void)group;
}

static struct net_group *do_get_net_group_from_netif(struct netif *netif)
{
    (void)netif;
    return get_root_net_group();
}

static void do_set_ippcb_net_group(struct ip_pcb *pcb, struct net_group *group)
{
    (void)pcb;
    (void)group;
}

static struct net_group *do_get_net_group_from_ippcb(struct ip_pcb *pcb)
{
    (void)pcb;
    return get_root_net_group();
}

static struct net_group_ops root_net_group_ops = {
  .get_curr_process_net_group = get_root_net_group,
  .set_netif_net_group = do_set_netif_net_group,
  .get_net_group_from_netif = do_get_net_group_from_netif,
  .set_ippcb_net_group = do_set_ippcb_net_group,
  .get_net_group_from_ippcb = do_get_net_group_from_ippcb,
};

struct net_group_ops *default_net_group_ops = &root_net_group_ops;

void set_default_net_group_ops(struct net_group_ops *ops) {
    default_net_group_ops = ops;
}

struct net_group_ops *get_default_net_group_ops(void) {
    return default_net_group_ops;
}
#endif
