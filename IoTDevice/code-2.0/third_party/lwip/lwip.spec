%global debug_package %{nil}
%global __os_install_post %{nil}

Summary: lwip is a small independent implementation of the TCP/IP protocol suite
Name:    lwip
Version: 2.1.3
Release: 102
License: BSD
URL:     http://savannah.nongnu.org/projects/lwip/
Source0: http://download.savannah.nongnu.org/releases/lwip/%{name}-%{version}.zip

Patch6001:  backport-tcp-fix-sequence-number-comparison.patch
Patch6002:  backport-tcp-tighten-up-checks-for-received-SYN.patch

Patch9000:  0001-add-makefile.patch
Patch9001:  0002-adapt-lstack.patch
Patch9002:  0003-fix-the-occasional-coredump-when-the-lwip-exits.patch
Patch9003:  0004-fix-error-of-deleting-conn-table-in-connect.patch
Patch9004:  0005-syn-rcvd-state-reg-conn-into-conntable.patch
Patch9005:  0006-fix-coredump-in-etharp.patch
Patch9006:  0007-gazelle-fix-epoll_ctl-EPOLLET-mode-error.patch
Patch9007:  0008-gazelle-fix-lwip_accept-memcpy-sockaddr-large.patch
Patch9008:  0009-fix-stack-buffer-overflow-when-memcpy-addr.patch
Patch9009:  0010-fix-the-incomplete-release-of-the-conntable.patch
Patch9010:  0011-remove-gazelle-tcp-conn-func.patch
Patch9011:  0012-fix-incomplete-resource-release-in-lwip-close.patch
Patch9012:  0013-remove-gazelle-syscall-thread.patch
Patch9013:  0014-fix-some-compile-errors.patch
Patch9014:  0015-fix-tcp-port-alloc-issue.patch
Patch9015:  0016-lstack-support-mysql-mode.patch
Patch9016:  0017-support-REUSEPOR-option.patch
Patch9017:  0018-exec-gazelle_init_sock-before-read-event.patch
Patch9018:  0019-gazelle-reduce-copy-in-send.patch
Patch9019:  0020-remove-chose_dlsym_handle-function-set-handle-to-RTL.patch
Patch9020:  0021-refactor-event-if-ring-is-full-the-node-is-added-to-.patch
Patch9021:  0022-notify-app-that-sock-state-changes-to-CLOSE_WAIT.patch
Patch9022:  0023-refactor-event-and-checksum-offload-support.patch
Patch9023:  0024-refactor-pkt-read-send-performance.patch
Patch9024:  0025-Replace-gettid-with-syscall-SYS_gettid.patch
Patch9025:  0026-del-redundant-wait_close-and-move-epoll_events-pos.patch
Patch9026:  0027-modify-EISCONN-condition.patch
Patch9027:  0028-per-thread-reassdata-variables.patch
Patch9028:  0029-fix-EISCONN-err-and-remove-same-customized-modificat.patch
Patch9029:  0030-refactor-tcp-new-port.patch
Patch9030:  0031-refactor-add-event-limit-send-pkts-num.patch
Patch9031:  0032-fix-free-pbuf-miss-data.patch
Patch9032:  0033-alloc-socket-fail-clean-sock.patch
Patch9033:  0034-add-accept4-and-epoll_create1.patch
Patch9034:  0035-add-writev-and-readv.patch
Patch9035:  0036-add-fs-secure-compilation-option.patch
Patch9036:  0037-enable-ARP-QUEUE-to-avoid-sync-packet-dropped.patch
Patch9037:  0038-add-tso.patch
Patch9038:  0039-optimize-app-thread-write-buff-block.patch
Patch9039:  0040-add-huge-snd_buf.patch
Patch9040:  0041-optimite-pcb-list-limit-send-size-and-ack-now.patch
Patch9041:  0042-expand-recv-win.patch
Patch9042:  0043-add-prefetch.patch
Patch9043:  0044-skip-unnecessary-tcp_route.patch
Patch9044:  0045-add-variable-in-struct-sock.patch
Patch9045:  0046-add-dataack-when-recv-too-many-acks-with-data.patch
Patch9046:  0047-reduce-struct-pbuf-size.patch
Patch9047:  0048-listen-pcb-also-use-pcb_if.patch
Patch9048:  0049-expand-recv-mbox-size.patch
Patch9049:  0050-lwip-reuse-ip-port.patch
Patch9050:  0051-lwip-add-need_tso_send.patch
Patch9051:  0052-lwip_fnctl-only-support-F_SETFL-F_GETFL.patch
Patch9052:  0053-cleancode-improve-lwipopts.h-readability.patch
Patch9053:  0054-reduce-cpu-usage-when-send.patch
Patch9054:  0055-add-pbuf-lock-when-aggregate-pbuf.patch
Patch9055:  0056-fix-tso-small-packet-drop-in-kernel-server.patch
Patch9056:  0057-same-node-gazellectl-a.patch
Patch9057:  0058-lwip-send-recv-thread-bind-numa.patch
Patch9058:  0059-fix-last_unsent-last_unacked.patch
Patch9059:  0060-lwip-add-udp-multicast.patch
Patch9060:  0061-fix-pbuf-leak-in-udp-connection.patch
Patch9061:  0062-drop-netbuf-in-recv_udp-to-fix-mem-overflow.patch
Patch9062:  0063-optimize-avoid-too-many-empty-acks-in-tcp_input.patch
Patch9063:  0064-fix-udp-send-recv-in-multiple-queue.patch
Patch9064:  0065-fix-udp-recvmbox-size-not-set.patch
Patch9065:  0066-adapt-to-dpdk-19.11-and-dpdk-21.11.patch
Patch9066:  0067-fix-null-pointer-when-all-zero-address-listen.patch
Patch9067:  0068-enable-UDP-CKSUM-in-lwip.patch
Patch9068:  0069-add-error-check-in-hugepage_init-and-sys_mbox_free.patch
Patch9069:  0070-add-CHECKSUM_UDP-when-not-support-OFFLOAD_UDP_CHECKS.patch
Patch9070:  0071-fix-pbuf-tot_len-incorrect-after-pbuf_split_64k-is-c.patch
Patch9071:  0072-add-O_NONBLOCK-and-FIONBIO-when-not-defined.patch
Patch9072:  0073-lstack_lwip-external-api-start-with-do_lwip_-prefix.patch
Patch9073:  0074-gazelle-offloads-are-registered-to-lwip.patch
Patch9074:  0075-adapt-read-write-for-rtc-mode.patch
Patch9075:  0076-fix-recvmsg-return-EINVAL.patch
Patch9076:  0077-adpat-event-for-rtc-mode.patch
Patch9077:  0078-posix_api-support-select.patch
Patch6003:  backport-Add-outgoing-VLAN-PCP-support.patch
Patch6004:  backport-fix-compiling-ETHARP_SUPPORT_VLAN.patch
Patch9078:  0079-enable-vlan-define.patch
Patch9079:  0080-enable-ipv6.patch
Patch9080:  0081-ip6-hdr.patch
Patch9081:  0082-add-vlanid-in-netif.patch
Patch9082:  0083-lwipopts-add-lwip-debug-log-macro.patch
Patch9083:  0084-add-tcpslowtmr-log-and-tcpfasttmr-cnt.patch
Patch9084:  0085-add-lwip-log-tcp_rst-tcp_abandon-tcp_abort.patch
Patch9085:  0086-log-add-errevent-log-and-tcp-exception-statistics.patch
Patch9086:  0087-support-vlan-offload.patch
Patch9087:  0088-modify-log-info-err.patch
Patch9088:  0089-add-struct-gz-addr.patch
Patch9089:  0090-frag-fix-coredump-when-get-netif.patch
Patch9090:  0091-add-fd-log-info-and-fix-wrong-port-log-info.patch
Patch9091:  0092-fix-the-coredump-issue-when-UDP-traffic-is-sent.patch
Patch9092:  0093-modfiy-accept-null-pointer-when-new-conn-receive-RST-packet-in-listening.patch
Patch9093:  0094-lwip-log-fix-reversed-port-in-tcp_input.patch
Patch9094:  0095-event_callback-del-errevent-log-if-err-is-ERR_OK.patch
Patch9095:  0096-tcp_send_fin-add-the-fin-to-the-last-unsent-segment.patch
Patch9096:  0097-Mod-the-issue-that-2w-connection-unable-to-establish.patch
Patch9097:  0098-remove-duplicate-lwip-log.patch
Patch9098:  0099-fix-rte_ring_create-time-consuming.patch

BuildRequires: gcc-c++ dos2unix dpdk-devel

#Requires: 

ExclusiveArch: x86_64 aarch64 loongarch64 sw_64

%description
lwip is a small independent implementation of the TCP/IP protocol suite.

%prep
%setup -n %{name}-%{version} -q
find %{_builddir}/%{name}-%{version} -type f -exec dos2unix -q {} \;
%autopatch -p1

%build
#export DPDK_VERSION_1911=1
cd %{_builddir}/%{name}-%{version}/src
%make_build

%install
cd %{_builddir}/%{name}-%{version}/src
%make_install INSTALL_LIB=%{buildroot}%{_libdir}

%files
%defattr(0644,root,root)
%{_includedir}/lwip
%{_libdir}/liblwip.a

%changelog
* Tue Dec 26 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-102
- fix rte_ring_create/free time-consuming

* Tue Dec 26 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-101
- remove duplicate lwip log

* Mon Dec 25 2023 hankangkang <hankangkang5@huawei.com> - 2.1.3-100
- Mod the issue that 2w connection unable to establish

* Sat Dec 23 2023 yangchen <yangchen145@huawei.com> - 2.1.3-99
- tcp_send_fin: add the fin to the last unsent segment

* Wed Dec 20 2023 yangchen <yangchen145@huawei.com> - 2.1.3-98
- event_callback: del errevent log if err is ERR_OK

* Fri Dec 15 2023 yangchen <yangchen145@huawei.com> - 2.1.3-97
- lwip log: fix reversed port in tcp_input

* Thu Dec 14 2023 hankangkang <hankangkang5@huawei.com> - 2.1.3-96
- modfiy-accept-null-pointer-when-new-conn-receive-RST-packet-in-listening

* Sat Dec 9 2023 wuchangye <wuchangye@huawei.com> - 2.1.3-95
- fix the coredump issue when UDP traffic is sent

* Fri Dec 8 2023 yangchen <yangchen145@huawei.com> - 2.1.3-94
- add fd log info and fix wrong port log info

* Fri Dec 8 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-93
- ip4 frag: fix coredump when get netif

* Wed Dec 6 2023 zhengjiebing <zhengjiebing@cmss.chinamobile.com> - 2.1.3-92
- add struct gz_addr_t

* Tue Dec 5  2023 hankangkang <hankangkang5@huawei.com> - 2.1.3-91
- modify-log-info-err

* Tue Nov 28 2023 zhengjiebing <zhengjiebing@cmss.chinamobile.com> - 2.1.3-90
- support vlan offload

* Tue Nov 28 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-89
- log: add errevent log and tcp exception statistics

* Tue Nov 28 2023 yangchen <yangchen145@huawei.com> - 2.1.3-88
- add lwip log: tcp_rst & tcp_abandon & tcp_abort

* Tue Nov 28 2023 hankangkang <hankangkang5@huawei.com> - 2.1.3-87
- lwipopts: add tcpslowtmr log and tcpfasttmr cnt

* Mon Nov 27 2023 yangchen <yangchen145@huawei.com> - 2.1.3-86
- lwipopts: add lwip debug log macro

* Mon Nov 27 2023 zhengjiebing <zhengjiebing@cmss.chinamobile.com> - 2.1.3-85
- add vlan_id in netif

* Fri Nov 24 2023 zhangxingrong <zhangxingrong@uniontech.com> - 2.1.3-84
- modify error date

* Fri Nov 17 2023 zhengjiebing <zhengjiebing@cmss.chinamobile.com> - 2.1.3-83
- enable ipv6 in lwip

* Fri Nov 03 2023 zhujunhao <zhujunhao11@huawei.com> - 2.1.3-82
- add support vlan

* Fri Nov 03 2023 yangchen <yangchen145@huawei.com> - 2.1.3-81 
- posix_api support select

* Fri Oct 27 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-80
- adapt read/write for rtc mode
- fix recvmsg return EINVAL
- adapt event for rtc mode

* Tue Oct 24 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-79
- gazelle offloads are registerd to lwip

* Sun Oct 08 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-78
- lstack_lwip: external api start with do_lwip_ prefix

* Sun Oct 08 2023 panchenbo <panchenbo@kylinsec.com.cn> - 2.1.3-77
- add O_NONBLOCK and FIONBIO when not defined

* Fri Sep 15 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-76
- fix pbuf->tot_len incorrect after pbuf_split_64k is called

* Tue Jun 27 2023 kircher <majun65@huawei.com> - 2.1.3-75
- add CHECKSUM_UDP when not support OFFLOAD_UDP_CHECKS

* Sun Jun 25 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-74
- add error check in hugepage_init and sys_mbox_free

* Wed Jun 21 2023 kircher <majun65@huawei.com> - 2.1.3-73
- enable udp cksum in lwip

* Thu Jun 15 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-72
- fix null pointer when zero port listen

* Thu Jun 15 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-71
- patch -p1 automaition in lwip.spec

* Thu Jun 15 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-70
- adapt to dpdk-19.11 and dpdk-21.11

* Wed Jun 14 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-69
- fix udp recvmbox size not set

* Wed Jun 14 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-68
- fix udp send/recv in mutiple queue

* Wed Jun 07 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-67
- optimize: avoid too many empty acks in tcp_input

* Tue Jun 06 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-66
- revert cleancode series patches

* Mon May 29 2023 kircher <majun65@huawei.com> - 2.1.3-65
- drop netbuf in recv_udp to fix mem overflow

* Mon May 29 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-64
- cleancode: refactor memp

* Mon May 29 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-63
- cleancode: refactor OFFLOAD_CHECKSUM GAZELLE_TCP_DATAACKS_REXMIT GAZELLE_TCP_NEW_PORT

* Mon May 29 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-62
- fix spec patch9069 

* Mon May 29 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-61
- cleancode: refactor sys_now and lwip_ioctl

* Mon May 29 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-60
- cleancode: refactor GAZELLE_TCP_PCB_HASH

* Mon May 29 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-59
- cleancode: refactor options define

* Thu May 25 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-58
- cleancode: refactor gazelle_hlist.h

* Thu May 25 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-57
- cleancode: refactor gazelle_list.h

* Wed May 24 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-56
- cleancode: refactor gazelle_posix_api.h

* Tue May 23 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-55
- cleancode: refactor lwipsock.h

* Tue May 23 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-54
- cleancode: remove perf
- cleancode: rename gazelle files in lwip

* Tue May 23 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-53
- cleancode: improving makefile readability

* Tue May 16 2023 kircher <majun65@huawei.com> - 2.1.3-52
- fix pbuf leak in udp connection

* Fri May 12 2023 kircher <majun65@huawei.com> - 2.1.3-51
- add udp multicast support in lwip

* Sat Apr 01 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-50
- fix last_unsent/last_unacked error
- fix send failed due to pcb->nrtx > TCP_MAXRTX

* Wed Mar 22 2023 kircher <majun65@huawei.com> - 2.1.3-49
- lwip send recv thread bind numa

* Mon Mar 13 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-48
- add same node ring & gazellectl -a

* Mon Mar 13 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-47
- fix tso small packet drop in kernel server

* Mon Mar 13 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-46
- use pbuf lock when aggregate pbuf

* Fri Mar 10 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-45
- reduce cpu usage when send

* Thu Mar 9 2023 Lemmy Huang <huangliming5@huawei.com> - 2.1.3-44
- cleancode: improve lwipopts.h readability

* Wed Feb 22 2023 jiangheng <jiangheng14@huawei.com> - 2.1.3-43
- lwip_fnctl only suport F_SETFL,F_GETFL, other opt return 0 for compitable

* Tue Feb 21 2023 majun<majun65@huawei.com> - 2.1.3-42
- add lwip need_tso_send

* Tue Feb 14 2023 majun<majun65@huawei.com> - 2.1.3-41
- add lwip reuse ip port

* Sat Feb 11 2023 majun<majun65@huawei.com> - 2.1.3-40
- fix TSO snd_nxt incorrectly update

* Fri Dec 30 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-39
- expand recv mbox size

* Wed Dec 21 2022 jiangheng <jiangheng14@huawei.com> - 2.1.3-38
- move pcb_if to ip_pcb to let listen pcb can use it

* Wed Dec 21 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-37
- reduce struct pbuf size

* Wed Dec 21 2022 kircher<majun65@huawei.com> - 2.1.3-36
- do not update cwnd when send dataack

* Tue Dec 20 2022 kircher<majun65@huawei.com> - 2.1.3-35
- fix the dataack is always lower than 256

* Tue Dec 20 2022 kircher<majun65@huawei.com> - 2.1.3-34
- add dataack when recv too many acks with data

* Tue Dec 20 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-33
- add variable in struct sock

* Mon Dec 19 2022 kircher<majun65@huawei.com> - 2.1.3-32
- skip unnecessary tcp_route

* Sun Dec 18 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-31
- expand rcv wnd size and add prefetch

* Tue Dec 13 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-30
- optimite pcb unsent and unacked list
  fast rexmit all pkts

* Tue Dec 6 2022 zhujunhao<zhujunhao11@huawei.com> - 2.1.3-29
- add huge snd_buf

* Sat Dec 3 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-28
- add tso define

* Thu Dec 01 2022 jiangheng<jiangheng14@huawei.com> - 2.1.3-27
- remove lwip-2.1.3.tar.gz

* Sat Nov 26 2022 jiangheng<jiangheng14@huawei.com> - 2.1.3-26
- replace lwip-2.1.3.tar.gz to lwip-2.1.3.zip

* Wed Nov 23 2022 jiangheng<jiangheng14@huawei.com> - 2.1.3-25
- enable ARP QUEUE to avoid packet dropped

* Sat Oct 22 2022 jiangheng<jiangheng14@huawei.com> - 2.1.3-24
- add fs secure compilation option

* Wed Oct 19 2022 zhujunhao<zhujunhao11@huawei.com> - 2.1.3-23
- add writev and readv

* Sat Oct 15 2022 zhujunhao<zhujunhao11@huawei.com> - 2.1.3-22
- add epoll_create1 and accetp4

* Tue Oct 11 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-21
- alloc socket fail clean sock

* Thu Oct 6 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-20
- fix miss data due to free pbuf
  close debug

* Thu Oct 6 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-19
- refactor add event
  limit send pkts num max 10

* Thu Oct 6 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-18
- fix multithread duplicate port num
  support select appropriate port num to rss same as nic

* Thu Oct 6 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-17
- fix EISCONN conditon err
  remove same customized modification

* Thu Oct 6 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-16
- per thread reassdata variables

* Thu Oct 6 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-15
- modify EISCONN path condition
  add in_send and send_flag value in sock

* Tue Jul 26 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-14
- del redundant wait_close in lwip_sock
  move epoll_events into cache aligned area

* Tue Jul 12 2022 Honggang Li <honggangli@163.com> - 2.1.3-13
- Replace gettid() with syscall()

* Fri Jul 8 2022 xiusailong<xiusailong@huawei.com> - 2.1.3-12
- sync two patches from 20.03-LTS-SP1

* Thu Jul 7 2022 wuchangsheng<wuchangsheng2@huawei.com> - 2.1.3-11
- refactor refactor pkt read send performance

* Tue Mar 29 2022 jiangheng<jiangheng12@huawei.com> - 2.1.3-10
- refactor event
- add HW checksum offload support

* Tue Mar 15 2022 jiangheng<jiangheng12@huawei.com> - 2.1.3-9
- notify app that sock state changes to CLOSE_WAIT

* Tue Mar 15 2022 jiangheng<jiangheng12@huawei.com> - 2.1.3-8
- refactor event,if ring is full, node is added to list

* Mon Mar 07 2022 jiangheng<jiangheng12@huawei.com> - 2.1.3-7
- remove chose_dlsym_handle function as it is redundant

* Mon Mar 07 2022 wu-changsheng<wuchangsheng2@huawei.com> - 2.1.3-6
- gazelle reduce copy in send

* Mon Mar 07 2022 jiangheng<jiangheng12@huawei.com> - 2.1.3-5
- exec gazelle_sock_init before read event

* Thu Mar 03 2022 jiangheng<jiangheng12@huawei.com> - 2.1.3-4
- support REUSEPOR option
- fix rpc msg too much
- fix recrruing events

* Thu Feb 24 2022 jiangheng<jiangheng12@huawei.com> - 2.1.3-3
- remove kernel socket interface
- support the mode that listen and accept thread be separaten

* Fri Dec 31 2021 jiangheng<jiangheng12@huawei.com> - 2.1.3-2
- adapt to lstack

* Fri Nov 26 2021 jiangheng<jiangheng12@huawei.com> - 2.1.3-1
- update to 2.1.3

* Mon Sep 06 2021 jiangheng<jiangheng12@huawei.com> - 2.1.2-2
- backport some patches from community

* Mon Nov 30 2020 peanut_huang<huangliming5@huawei.com> - 2.1.2-1
- remove README

* Mon Nov 30 2020 peanut_huang<huangliming5@huawei.com> - 2.1.2-0
- Init package
