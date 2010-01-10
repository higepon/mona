SRCTOP=../..
include $(SRCTOP)/Makefile.inc

TARGET     = NET
SOURCES    = main.cpp
CSOURCES   = \
sys_arch.c \
lwip-1.3.1/src/core/stats.c \
lwip-1.3.1/src/core/dhcp.c \
lwip-1.3.1/src/core/dns.c \
lwip-1.3.1/src/core/init.c \
lwip-1.3.1/src/core/mem.c \
lwip-1.3.1/src/core/memp.c \
lwip-1.3.1/src/core/netif.c \
lwip-1.3.1/src/core/pbuf.c \
lwip-1.3.1/src/core/raw.c \
lwip-1.3.1/src/core/stats.c \
lwip-1.3.1/src/core/sys.c \
lwip-1.3.1/src/core/tcp.c \
lwip-1.3.1/src/core/tcp_in.c \
lwip-1.3.1/src/core/tcp_out.c \
lwip-1.3.1/src/core/udp.c \
lwip-1.3.1/src/api/api_lib.c \
lwip-1.3.1/src/api/api_msg.c \
lwip-1.3.1/src/api/err.c \
lwip-1.3.1/src/api/netbuf.c \
lwip-1.3.1/src/api/netdb.c \
lwip-1.3.1/src/api/netifapi.c \
lwip-1.3.1/src/api/sockets.c \
lwip-1.3.1/src/api/tcpip.c \
lwip-1.3.1/src/netif/etharp.c \
lwip-1.3.1/src/core/ipv4/autoip.c \
lwip-1.3.1/src/core/ipv4/icmp.c \
lwip-1.3.1/src/core/ipv4/igmp.c \
lwip-1.3.1/src/core/ipv4/inet.c \
lwip-1.3.1/src/core/ipv4/inet_chksum.c \
lwip-1.3.1/src/core/ipv4/ip.c \
lwip-1.3.1/src/core/ipv4/ip_addr.c \
lwip-1.3.1/src/core/ipv4/ip_frag.c


INSTDIR    = $(BINDIR)/SERVERS
ADDLINK    = -lmonalibc-imp --enable-auto-import
ADDCFLAGS  = -fpack-struct -DLWIP_DEBUG
ADDINCLUDE = -I . -I lwip-1.3.1/src/include -I lwip-1.3.1/src/include/ipv4/

SHAREDIR   = $(MONADIR)/share
ifneq ($(BUILD_TARGET),ELF)
include $(SHAREDIR)/configs/monapi-ex5.inc
else
include $(SHAREDIR)/configs/monapi-el5.inc
endif

check-syntax:
	LANG=C $(CC) -o nul $(CFLAGS) $(INCLUDE) -S ${CHK_SOURCES}

-include dependencies
