/*
 * main.cpp - Net Server which offers BSD socket.
 *
 *   Copyright (c) 2009  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <monapi.h>

using namespace MonAPI;

#include "lwip/init.h"
#include "lwip/tcpip.h"


#include "lwip/debug.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"

#include "lwip/stats.h"

#include "lwip/ip.h"
#include "lwip/ip_frag.h"
#include "lwip/udp.h"
//#include "lwip/snmp_msg.h"
#include "lwip/tcp.h"
#include "etherif.h"
#include "netif/etharp.h"

#include "timer.h"
//#include <signal.h>
#include <monapi.h>
#include "ContigousPhysicalMemory.h"
#include "Ether.h"
#include "VirtioNet.h"
#include "Util.h"
#include "DHCPClient.h"

#include "echo.h"
extern "C" {
#include "shell.h"
};

#include "messageLoop.h"
//#include "private_mib.h"

/* (manual) host IP configuration */
static struct ip_addr ipaddr, netmask, gw;

/* SNMP trap destination cmd option */
static unsigned char trap_flag;
//static struct ip_addr trap_addr;

/* nonstatic debug cmd option, exported in lwipopts.h */
unsigned char debug_flags;

/* 'non-volatile' SNMP settings
  @todo: make these truly non-volatile */
u8_t syscontact_str[255];
u8_t syscontact_len = 0;
u8_t syslocation_str[255];
u8_t syslocation_len = 0;
/* enable == 1, disable == 2 */
u8_t snmpauthentraps_set = 2;

// static struct option longopts[] = {
//   /* turn on debugging output (if build with LWIP_DEBUG) */
//   {"debug", no_argument,        NULL, 'd'},
//   /* help */
//   {"help", no_argument, NULL, 'h'},
//   /* gateway address */
//   {"gateway", required_argument, NULL, 'g'},
//   /* ip address */
//   {"ipaddr", required_argument, NULL, 'i'},
//   /* netmask */
//   {"netmask", required_argument, NULL, 'm'},
//   /* ping destination */
//   {"trap_destination", required_argument, NULL, 't'},
//   /* new command line options go here! */
//   {NULL,   0,                 NULL,  0}
// };
// #define NUM_OPTS ((sizeof(longopts) / sizeof(struct option)) - 1)

// void usage(void)
// {
//   unsigned char i;

//   printf("options:\n");
//   for (i = 0; i < NUM_OPTS; i++) {
//     printf("-%c --%s\n",longopts[i].val, longopts[i].name);
//   }
// }

uintptr_t ContigousPhysicalMemory::startAddress = 0x9E000000;
#define USE_QEMU_USER_NETWORK 1

VirtioNet* g_virtioNet = NULL;
void init_virtio(uint32_t& hostAddress, uint32_t& gatewayAddress)
{
        _logprintf("net: %s:%d\n", __FILE__, __LINE__);
  syscall_get_io();
        _logprintf("net: %s:%d\n", __FILE__, __LINE__);

  g_virtioNet = new VirtioNet();
  const int numberOfReadBufferes = 5;
  enum VirtioNet::DeviceState state = g_virtioNet->probe(numberOfReadBufferes);
  if (state != VirtioNet::DEVICE_FOUND) {
      _printf("[virtio] virtio-net device not found\n");
      exit(-1);
  }
// qemu -net user mode:
//   we send DHCP request to QEMU and get an ip address.
#ifdef USE_QEMU_USER_NETWORK
    DHCPClient dhcp(g_virtioNet, g_virtioNet->macAddress());;
    if (!dhcp.request(hostAddress, gatewayAddress)) {
        _printf("[uIP] DHCP server not found. exit server\n");
        exit(-1);
    }
#endif
}

static void tcpip_init_done(void* arg)
{
    sys_sem_t *sem = (sys_sem_t*)arg;
    sys_sem_signal(*sem);
}


int main(int argc, char **argv)
{
  const char* MAP_FILE_PATH = "/APPS/NET.MAP";
  uint32_t pid = syscall_get_pid();
  intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
  if (ret != M_OK) {
      fprintf(stderr, "syscall_stack_trace_enable error %d\n", ret);
      exit(-1);
  }

  struct netif netif;
  struct in_addr inaddr;
//  int ch;
  char ip_str[16] = {0}, nm_str[16] = {0}, gw_str[16] = {0};
//  DebuggerService::breakpoint();

  uint32_t hostAddress = 0;
  uint32_t gatewayAddress = 0;
  init_virtio(hostAddress, gatewayAddress);

  ipaddr.addr = hostAddress;
  gw.addr = gatewayAddress;
  IP4_ADDR(&netmask, 255,255,255,0);

  trap_flag = 0;
  /* use debug flags defined by debug.h */
  debug_flags = LWIP_DBG_OFF;
//  debug_flags |= (LWIP_DBG_ON|SOCKETS_DEBUG|API_MSG_DEBUG);
//  debug_flags |= (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT);

  inaddr.s_addr = ipaddr.addr;
  strncpy(ip_str,inet_ntoa(inaddr),sizeof(ip_str));
  inaddr.s_addr = netmask.addr;
  strncpy(nm_str,inet_ntoa(inaddr),sizeof(nm_str));
  inaddr.s_addr = gw.addr;
  strncpy(gw_str,inet_ntoa(inaddr),sizeof(gw_str));
  printf("Host at %s mask %s gateway %s\n", ip_str, nm_str, gw_str);


#ifdef PERF
  perf_init("/tmp/minimal.perf");
#endif /* PERF */


  // for multi thread
  sys_sem_t sem = sys_sem_new(0);
  tcpip_init(tcpip_init_done, &sem);
  sys_sem_wait(sem);
  sys_sem_free(sem);

  // for single thread
  //lwip_init();

  printf("TCP/IP initialized.\n");

//  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, etherif_init, ip_input);
  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, etherif_init, tcpip_input);
  netif_set_default(&netif);
  netif_set_up(&netif);

  // N.B.
  //  This thread should be the first.
  //  thread id of the thread should be "main_thread_id + 1"
  message_loop_init();

#if SNMP_PRIVATE_MIB != 0
  /* initialize our private example MIB */
  lwip_privmib_init();
#endif
//   snmp_trap_dst_ip_set(0,&trap_addr);
//   snmp_trap_dst_enable(0,trap_flag);
//   snmp_set_syscontact(syscontact_str,&syscontact_len);
//   snmp_set_syslocation(syslocation_str,&syslocation_len);
//   snmp_set_snmpenableauthentraps(&snmpauthentraps_set);
//   snmp_init();


  echo_init();
  socket_examples_init();

  timer_init();
  timer_set_interval(TIMER_EVT_ETHARPTMR,2000);
  timer_set_interval(TIMER_EVT_TCPFASTTMR, TCP_FAST_INTERVAL / 10);
  timer_set_interval(TIMER_EVT_TCPSLOWTMR, TCP_SLOW_INTERVAL / 10);
#if IP_REASSEMBLY
  timer_set_interval(TIMER_EVT_IPREASSTMR,100);
#endif

  printf("Applications started.\n");


  while (1) {

      /* poll for input packet and ensure
         select() or read() arn't interrupted */
//       sigemptyset(&mask);
//       sigaddset(&mask, SIGALRM);
//       sigprocmask(SIG_BLOCK, &mask, &oldmask);

      /* start of critical section,
         poll netif, pass packet to lwIP */
      if (etherif_select(&netif) > 0)
      {
        /* work, immediatly end critical section
           hoping lwIP ended quickly ... */
//        sigprocmask(SIG_SETMASK, &oldmask, NULL);
      }
      else
      {
        /* no work, wait a little (10 msec) for SIGALRM */
//          sigemptyset(&empty);
//          sigsuspend(&empty);
        /* ... end critical section */
//          sigprocmask(SIG_SETMASK, &oldmask, NULL);
      }
      if(timer_testclr_evt(TIMER_EVT_TCPFASTTMR))
      {
        tcp_fasttmr();
      }
      if(timer_testclr_evt(TIMER_EVT_TCPSLOWTMR))
      {
        tcp_slowtmr();
      }
#if IP_REASSEMBLY
      if(timer_testclr_evt(TIMER_EVT_IPREASSTMR))
      {
        ip_reass_tmr();
      }
#endif
      if(timer_testclr_evt(TIMER_EVT_ETHARPTMR))
      {
        etharp_tmr();
      }

  }

  return 0;
}
