#
# Copyright (c) 2011-2012, fortiss GmbH.
# Licensed under the Apache License, Version 2.0.
# 
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# http://chromosome.fortiss.org/.
#
# This file is part of CHROMOSOME.
#
# $Id$
#
# Author:
#         Simon Barner <barner@fortiss.org>
#

# Platform (OS) specific extensions for LWIP
xme_get (_XME_PLATFORM PROPERTY_GLOBAL XME_PLATFORM)
xme_defined (_XME_LWIP_PLATFORM_DEFINED PROPERTY_GLOBAL XME_LWIP_PLATFORM)
if (NOT ${_XME_LWIP_PLATFORM_DEFINED})
	if ((${_XME_PLATFORM} STREQUAL "freertos") OR (${_XME_PLATFORM} STREQUAL "baremetal"))
		xme_set (PROPERTY_GLOBAL XME_LWIP_PLATFORM "${_XME_PLATFORM}")
		message (STATUS "Deriving XME_LWIP_PLATFORM from XME_PLATFORM: '${_XME_PLATFORM}'")
	else ((${_XME_PLATFORM} STREQUAL "freertos") OR (${_XME_PLATFORM} STREQUAL "baremetal"))
		xme_set (PROPERTY_GLOBAL XME_LWIP_PLATFORM "baremetal")
		message ("Selected XME_PLATFORM '${_XME_PLATFORM}' not supported by LWIP. Assuming 'baremetal'.")
	endif ((${_XME_PLATFORM} STREQUAL "freertos") OR (${_XME_PLATFORM} STREQUAL "baremetal"))
endif (NOT ${_XME_LWIP_PLATFORM_DEFINED})

xme_get (_XME_LWIP_PLATFORM PROPERTY_GLOBAL XME_LWIP_PLATFORM)

if (${_XME_LWIP_PLATFORM} STREQUAL "freertos")

# FreeRTOS options

# The following FreeRTOS options are required when lwIP is
# build with FreeRTOS support.
#
# xme_build_option_set: Value cannot be overridden by user
#
	xme_build_option_set(EXTERNAL_FREERTOS_configUSE_MUTEXES 1 "Required for lwIP FreeRTOS support.")
	xme_build_option_set(EXTERNAL_FREERTOS_configUSE_PREEMPTION 1 "Required for lwIP FreeRTOS support.")



# TODO:
#
# Here, we could add checks that the configured heap and stack size
# is sufficient for lwIP.
#
endif (${_XME_LWIP_PLATFORM} STREQUAL "freertos")

#### lwIP options ####

### FreeRTOS integration ###
# Locking options
if (${_XME_LWIP_PLATFORM} STREQUAL "freertos")
	xme_build_option_set (EXTERNAL_LWIP_SYS_LIGHTWEIGHT_PROT       1 "Required for lwIP FreeRTOS support.")
	xme_build_option_set (EXTERNAL_LWIP_NO_SYS                     0 "Required for lwIP FreeRTOS support.")
	
	# Thread options / RTOS integration
	# TODO: Are these default priorities and mbox sizes ok?
	xme_build_option_assert("EXTERNAL_FREERTOS_configMINIMAL_STACK_SIZE" "Probably in bsp/<...>/Options.cmake")
	xme_build_option(EXTERNAL_LWIP_TCPIP_THREAD_NAME               "\"tcpip_thread\"" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_TCPIP_THREAD_STACKSIZE          "(configMINIMAL_STACK_SIZE)" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_TCPIP_THREAD_PRIO               1 "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_TCPIP_MBOX_SIZE                 0 "lwipopts.h" "EXTERNAL_LWIP_")

	xme_build_option(EXTERNAL_LWIP_SLIPIF_THREAD_NAME              "\"slipif_loop\"" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_SLIPIF_THREAD_STACKSIZE         "(configMINIMAL_STACK_SIZE)" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_SLIPIF_THREAD_PRIO              1 "lwipopts.h" "EXTERNAL_LWIP_")

	xme_build_option(EXTERNAL_LWIP_PPP_THREAD_NAME                 "\"pppMain\"" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_PPP_THREAD_STACKSIZE             "(configMINIMAL_STACK_SIZE)" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_PPP_THREAD_PRIO                 1 "lwipopts.h" "EXTERNAL_LWIP_")

	xme_build_option(EXTERNAL_LWIP_DEFAULT_THREAD_NAME             "\"lwIP\"" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_DEFAULT_THREAD_STACKSIZE         "(configMINIMAL_STACK_SIZE)" "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_DEFAULT_THREAD_PRIO             1 "lwipopts.h" "EXTERNAL_LWIP_")

	xme_build_option(EXTERNAL_LWIP_DEFAULT_RAW_RECVMBOX_SIZE       0 "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_DEFAULT_UDP_RECVMBOX_SIZE       0 "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_DEFAULT_TCP_RECVMBOX_SIZE       0 "lwipopts.h" "EXTERNAL_LWIP_")
	xme_build_option(EXTERNAL_LWIP_DEFAULT_ACCEPTMBOX_SIZE         0 "lwipopts.h" "EXTERNAL_LWIP_")

	# Turn on the sequential API by default (can be overriden by user)
	xme_build_option_weak_set(EXTERNAL_LWIP_LWIP_NETCONN           1)
	xme_build_option_weak_set(EXTERNAL_LWIP_LWIP_SOCKET            1)
elseif (${_XME_LWIP_PLATFORM} STREQUAL "baremetal")
	xme_build_option_set (EXTERNAL_LWIP_NO_SYS 1 "Required for lwIP baremetal support.")
	
	xme_build_option_set(EXTERNAL_LWIP_LWIP_NETCONN                0 "The sequential API requires an operating system.")
	xme_build_option_set(EXTERNAL_LWIP_LWIP_SOCKET                 0 "The sequential API requires an operating system.")
else (${_XME_LWIP_PLATFORM} STREQUAL "baremetal")
	message (FATAL_ERROR "XME_LWIP_PLATFORM '${_XME_LWIP_PLATFORM}' is not supported.")
endif (${_XME_LWIP_PLATFORM} STREQUAL "freertos")

xme_build_option_assert("EXTERNAL_LWIP_SYS_LIGHTWEIGHT_PROT" "Set depending on XME_LWIP_PLATFORM.")
xme_build_option_assert("EXTERNAL_LWIP_NO_SYS" "Set depending on XME_LWIP_PLATFORM.")
xme_build_option_assert("EXTERNAL_LWIP_LWIP_NETCONN" "Set depending on XME_LWIP_PLATFORM.")
xme_build_option_assert("EXTERNAL_LWIP_LWIP_SOCKET" "Set depending on XME_LWIP_PLATFORM.")

# No default value given here since we xme_build_option_assert() that the user supplies
# matching value for his platform
xme_build_option(EXTERNAL_LWIP_SYS_LIGHTWEIGHT_PROT           "" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_NO_SYS                         "" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_NETCONN                   "" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_SOCKET                    "" "lwipopts.h" "EXTERNAL_LWIP_")

### Target specific options ###
# Memory options
# No default value given here since we xme_build_option_assert() that the user supplies
# matching value for his architecture
xme_build_option_assert("EXTERNAL_LWIP_MEM_ALIGNMENT" "Probably in bsp/<...>/Options.cmake")
xme_build_option(EXTERNAL_LWIP_MEM_ALIGNMENT                       "" "lwipopts.h" "EXTERNAL_LWIP_")

# No default value given here since we xme_build_option_assert() that the user supplies
# matching value for his architecture
xme_build_option_assert("EXTERNAL_LWIP_MEM_SIZE" "Probably in bsp/<...>/Options.cmake")
xme_build_option(EXTERNAL_LWIP_MEM_SIZE                            "" "lwipopts.h" "EXTERNAL_LWIP_")

# Link-layer header length: Ethernet: 14
xme_build_option(EXTERNAL_LWIP_PBUF_LINK_HLEN                                    "14" "lwipopts.h" "EXTERNAL_LWIP_")

xme_build_option(EXTERNAL_LWIP_ETH_PAD_SIZE                                      "0" "lwipopts.h" "EXTERNAL_LWIP_")
xme_get(_EXTERNAL_LWIP_PBUF_LINK_HLEN PROPERTY_GLOBAL EXTERNAL_LWIP_PBUF_LINK_HLEN)
xme_get(_EXTERNAL_LWIP_ETH_PAD_SIZE PROPERTY_GLOBAL EXTERNAL_LWIP_ETH_PAD_SIZE)

math(EXPR _EXTERNAL_LWIP_PBUF_LINK_HLEN_NET "${_EXTERNAL_LWIP_PBUF_LINK_HLEN} - ${_EXTERNAL_LWIP_ETH_PAD_SIZE}")

if (NOT ${_EXTERNAL_LWIP_PBUF_LINK_HLEN_NETTO} EQUAL 14)
	message ("The net length of the HW address (MAC) is not 14. This seems unusual.")
endif (NOT ${_EXTERNAL_LWIP_PBUF_LINK_HLEN_NETTO} EQUAL 14)

# Support for DMA-enabled MACs with scatter-gather support
xme_build_option(LWIP_NETIF_TX_SINGLE_PBUF                         "0" "lwipopts.h" "EXTERNAL_LWIP_")

xme_build_option(EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE                0 "lwipopts.h" "EXTERNAL_LWIP_")

# If no hardware-checksumming is available, enable software checksumming by default
xme_get (_EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE PROPERTY_GLOBAL EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE)
if (${_EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE})
	set (_SW_CHECKSUM_GEN 0)
else (${_EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE})
	set (_SW_CHECKSUM_GEN 1)
endif (${_EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE})
xme_build_option(EXTERNAL_LWIP_CHECKSUM_GEN_IP ${_SW_CHECKSUM_GEN} "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_CHECKSUM_GEN_UDP ${_SW_CHECKSUM_GEN} "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_CHECKSUM_GEN_TCP ${_SW_CHECKSUM_GEN} "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_CHECKSUM_CHECK_IP ${_SW_CHECKSUM_GEN} "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_CHECKSUM_CHECK_UDP ${_SW_CHECKSUM_GEN} "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_CHECKSUM_CHECK_TCP ${_SW_CHECKSUM_GEN} "lwipopts.h" "EXTERNAL_LWIP_")

### General options ###
# Memory options

xme_build_option(EXTERNAL_LWIP_MEMP_OVERFLOW_CHECK             0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_SANITY_CHECK               0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEM_USE_POOLS                   0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_USE_CUSTOM_POOLS           0 "lwipopts.h" "EXTERNAL_LWIP_")

# Internal pool sizes
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_PBUF                   16 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_RAW_PCB                4 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_UDP_PCB                4 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_TCP_PCB                5 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_TCP_PCB_LISTEN         10 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_TCP_SEG                16 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_REASSDATA              5 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_ARP_QUEUE              30 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_IGMP_GROUP             8 "lwipopts.h" "EXTERNAL_LWIP_")

xme_build_option(EXTERNAL_LWIP_MEMP_NUM_NETBUF                 2 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_NETCONN                4 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_TCPIP_MSG_API          8 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MEMP_NUM_TCPIP_MSG_INPKT        8 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_PBUF_POOL_SIZE                  16 "lwipopts.h" "EXTERNAL_LWIP_")

### Protocols ###
# ARP
xme_build_option(EXTERNAL_LWIP_LWIP_ARP                        1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_ARP_TABLE_SIZE                  10 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_ARP_QUEUEING                    1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_ETHARP_TRUST_IP_MAC             1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_ETHARP_SUPPORT_VLAN             0 "lwipopts.h" "EXTERNAL_LWIP_")

# IP options
xme_build_option(EXTERNAL_LWIP_IP_FORWARD                      0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_OPTIONS_ALLOWED              1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_REASSEMBLY                   1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_FRAG                         1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_REASS_MAXAGE                 3 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_REASS_MAX_PBUFS              10 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_FRAG_USES_STATIC_BUF         1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_FRAG_MAX_MTU                 1500 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_DEFAULT_TTL                  255 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_SOF_BROADCAST                0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_IP_SOF_BROADCAST_RECV           0 "lwipopts.h" "EXTERNAL_LWIP_")

# ICMP options
xme_build_option(EXTERNAL_LWIP_LWIP_ICMP                       1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_ICMP_TTL                        "(IP_DEFAULT_TTL)" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_BROADCAST_PING             0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_MULTICAST_PING             0 "lwipopts.h" "EXTERNAL_LWIP_")

# DHCP options
xme_build_option(EXTERNAL_LWIP_LWIP_DHCP                       1 "lwipopts.h" "EXTERNAL_LWIP_")

# AutoIP options
xme_build_option(EXTERNAL_LWIP_LWIP_AUTOIP                     0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_DHCP_AUTOIP_COOP           "((LWIP_DHCP) && (LWIP_AUTOIP))" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_DHCP_AUTOIP_COOP_TRIES     9 "lwipopts.h" "EXTERNAL_LWIP_")

# SNMP options
xme_build_option(EXTERNAL_LWIP_LWIP_SNMP                       0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_SNMP_CONCURRENT_REQUESTS        1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_SNMP_TRAP_DESTINATIONS          1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_SNMP_PRIVATE_MIB                0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_SNMP_SAFE_REQUESTS              1 "lwipopts.h" "EXTERNAL_LWIP_")

# IGMP options
xme_build_option(EXTERNAL_LWIP_LWIP_IGMP                       1 "lwipopts.h" "EXTERNAL_LWIP_")

# DNS options
xme_build_option(EXTERNAL_LWIP_LWIP_DNS 1                      "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_DNS_TABLE_SIZE                  4 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_DNS_MAX_NAME_LENGTH             256 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_DNS_MAX_SERVERS                 2 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_DNS_DOES_NAME_CHECK             1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_DNS_USES_STATIC_BUF             1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_DNS_MSG_SIZE                    512 "lwipopts.h" "EXTERNAL_LWIP_")

# UDP options
xme_build_option(EXTERNAL_LWIP_LWIP_UDP                        1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_UDPLITE                    0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_UDP_TTL                         "(IP_DEFAULT_TTL)" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_NETBUF_RECVINFO            1 "lwipopts.h" "EXTERNAL_LWIP_")

# TCP options
xme_build_option(EXTERNAL_LWIP_LWIP_TCP                        1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_TTL                         "(IP_DEFAULT_TTL)" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_WND                         "(4 * TCP_MSS)" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_MAXRTX                      12 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_SYNMAXRTX                   6 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_QUEUE_OOSEQ                 "(LWIP_TCP)" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_MSS                         1500 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_CALCULATE_EFF_SEND_MSS      1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_SND_BUF                     768 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_SND_QUEUELEN                "(4 * (TCP_SND_BUF)/(TCP_MSS))" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_SNDLOWAT                    "((TCP_SND_BUF)/2)" "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_LISTEN_BACKLOG              0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_DEFAULT_LISTEN_BACKLOG      0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_TCP_TIMESTAMPS             0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_TCP_WND_UPDATE_THRESHOLD        "(TCP_WND / 4)" "lwipopts.h" "EXTERNAL_LWIP_")

### APIs ###
# Hard default settings in lwip/opt.h. Do not set here in order to avoid warning about
# redefinition
#xme_build_option(EXTERNAL_LWIP_LWIP_EVENT_API                  0 "lwipopts.h" "EXTERNAL_LWIP_")
#xme_build_option(EXTERNAL_LWIP_LWIP_CALLBACK_API               1 "lwipopts.h" "EXTERNAL_LWIP_")

# Raw options
xme_build_option(EXTERNAL_LWIP_LWIP_RAW                        1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_RAW_TTL                         "(IP_DEFAULT_TTL)" "lwipopts.h" "EXTERNAL_LWIP_")

# Sequential API
xme_build_option(EXTERNAL_LWIP_LWIP_COMPAT_SOCKETS             1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_POSIX_SOCKETS_IO_NAMES     1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_TCP_KEEPALIVE              0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_SO_RCVTIMEO                0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_SO_RCVBUF                  1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_RECV_BUFSIZE_DEFAULT            "INT_MAX" "lwipopts.h" "EXTERNAL_LWIP_")


### Network interface options ###
xme_build_option(EXTERNAL_LWIP_LWIP_NETIF_HOSTNAME             0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_NETIF_API                  0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_NETIF_STATUS_CALLBACK      1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_NETIF_LINK_CALLBACK        1 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_NETIF_HWADDRHINT           0 "lwipopts.h" "EXTERNAL_LWIP_")

# Loopback options
xme_build_option(EXTERNAL_LWIP_LWIP_NETIF_LOOPBACK             0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_LOOPBACK_MAX_PBUFS         0 "lwipopts.h" "EXTERNAL_LWIP_")

# Loopif
xme_build_option(EXTERNAL_LWIP_LWIP_HAVE_LOOPIF                0 "lwipopts.h" "EXTERNAL_LWIP_")

# Slipif
xme_build_option(EXTERNAL_LWIP_LWIP_HAVE_SLIPIF                0 "lwipopts.h" "EXTERNAL_LWIP_")

# PPP[O{E,S]} support
xme_build_option(EXTERNAL_LWIP_PPP_SUPPORT                     0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_PPPOE_SUPPORT                   0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_PPPOS_SUPPORT                   0 "lwipopts.h" "EXTERNAL_LWIP_")

# CHAP, ...
xme_build_option(EXTERNAL_LWIP_PAP_SUPPORT                     0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_CHAP_SUPPORT                    0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_VJ_SUPPORT                      0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_MD5_SUPPORT                     0 "lwipopts.h" "EXTERNAL_LWIP_")

# Statistics and debugging options
xme_build_option(EXTERNAL_LWIP_LWIP_STATS                      0 "lwipopts.h" "EXTERNAL_LWIP_")
xme_build_option(EXTERNAL_LWIP_LWIP_PROVIDE_ERRNO              1 "lwipopts.h" "EXTERNAL_LWIP_")


### Network settings ###
xme_build_option_file_open ("network_settings.h")

# Option that influences EnetInit() function. If set to 1 then the function requires alerady set MAC Address. 
# In the Belle II project the MAC Address is obtained from the boothloader.
xme_build_option(GET_MAC_ADDR_FROM_BOOTLOADER "0" "network_settings.h")

xme_build_option(MAC_ADDR_FIRMWARE "1" "network_settings.h")
xme_get (_MAC_ADDR_FIRMWARE PROPERTY_GLOBAL MAC_ADDR_FIRMWARE)

if (_MAC_ADDR_FIRMWARE)

	# Dummy MAC address to fix build when user does not supply a MAC address.
	xme_defined (_MAC_ADDR0_DEFINED PROPERTY_GLOBAL MAC_ADDR0)
	if (NOT ${_MAC_ADDR0_DEFINED})
		message ("Using dummy MAC address 02:00:00:00:00:00 in order to make code compile.")
		message ("In order to use ethernet with a software-defined MAC address, you must set the following global properties: MAC_ADDR0-MAC_ADDR5.")
	else (NOT ${_MAC_ADDR0_DEFINED})
		xme_build_option_assert(MAC_ADDR0 "You must specify MAC_ADDR0-MAC_ADDR5 when MAC_ADDR_FIRMWARE is set.")
		xme_build_option_assert(MAC_ADDR1 "You must specify MAC_ADDR0-MAC_ADDR5 when MAC_ADDR_FIRMWARE is set.")
		xme_build_option_assert(MAC_ADDR2 "You must specify MAC_ADDR0-MAC_ADDR5 when MAC_ADDR_FIRMWARE is set.")
		xme_build_option_assert(MAC_ADDR3 "You must specify MAC_ADDR0-MAC_ADDR5 when MAC_ADDR_FIRMWARE is set.")
		xme_build_option_assert(MAC_ADDR4 "You must specify MAC_ADDR0-MAC_ADDR5 when MAC_ADDR_FIRMWARE is set.")
		xme_build_option_assert(MAC_ADDR5 "You must specify MAC_ADDR0-MAC_ADDR5 when MAC_ADDR_FIRMWARE is set.")
	endif (NOT ${_MAC_ADDR0_DEFINED})

	xme_build_option(MAC_ADDR0 "0x02" "network_settings.h")
	xme_build_option(MAC_ADDR1 "0x00" "network_settings.h")
	xme_build_option(MAC_ADDR2 "0x00" "network_settings.h")
	xme_build_option(MAC_ADDR3 "0x00" "network_settings.h")
	xme_build_option(MAC_ADDR4 "0x00" "network_settings.h")
	xme_build_option(MAC_ADDR5 "0x00" "network_settings.h")
endif (_MAC_ADDR_FIRMWARE)

# Select the way to obtain an IP address. (See ip_addr_config_mode_t in enet.h).
# The lwIP options EXTERNAL_LWIP_LWIP_DHCP and EXTERNAL_LWIP_LWIP_AUTOIP
# whether to include DHCP and AutoIP support, or not.
xme_build_option(IPMODE "IPADDR_DHCP" "network_settings.h")

xme_get (_IPMODE PROPERTY_GLOBAL IPMODE)
xme_get (_EXTERNAL_LWIP_LWIP_DHCP PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_DHCP)
xme_get (_EXTERNAL_LWIP_LWIP_AUTOIP PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_AUTOIP)

if (${_IPMODE} STREQUAL "IPADDR_DHCP")
	if (NOT ${_EXTERNAL_LWIP_LWIP_DHCP})
		message (FATAL_ERROR "You must include DHCP support in order to select EXTERNAL_LWIP_LWIP_IPMODE=IPADDR_DHCP.")
	endif (NOT ${_EXTERNAL_LWIP_LWIP_DHCP})
	set (_DHCP_AUTOIP 1)
elseif (${_IPMODE} STREQUAL "IPADDR_AUTOIP")
	if (NOT ${_EXTERNAL_LWIP_LWIP_AUTOIP})
		message (FATAL_ERROR "You must include AUTOIP support in order to select EXTERNAL_LWIP_LWIP_IPMODE=IPADDR_AUTOIP.")
	endif (NOT ${_EXTERNAL_LWIP_LWIP_AUTOIP})
	set (_DHCP_AUTOIP 1)
else (${_IPMODE} STREQUAL "IPADDR_AUTOIP")
	xme_build_option_assert(IP_ADDR0 "You must specify an IP address in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(IP_ADDR1 "You must specify an IP address in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(IP_ADDR2 "You must specify an IP address in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(IP_ADDR3 "You must specify an IP address in static (= !(dhcp || autoip)) mode.")
	
	xme_build_option_assert(NETMASK_ADDR0 "You must specify netmask in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(NETMASK_ADDR1 "You must specify netmask in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(NETMASK_ADDR2 "You must specify netmask in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(NETMASK_ADDR3 "You must specify netmask in static (= !(dhcp || autoip)) mode.")
	
	xme_build_option_assert(GW_ADDR0 "You must specify a gateway address in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(GW_ADDR1 "You must specify a gateway address in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(GW_ADDR2 "You must specify a gateway address in static (= !(dhcp || autoip)) mode.")
	xme_build_option_assert(GW_ADDR3 "You must specify a gateway address in static (= !(dhcp || autoip)) mode.")
	
	set (_DHCP_AUTOIP 0)
endif (${_IPMODE} STREQUAL "IPADDR_DHCP")

if (_DHCP_AUTOIP)
	# For DHCP or AutoIP, set all addresses to 0, unless the user provided
	# some custom default values
	xme_build_option_weak_set (IP_ADDR0 0)
	xme_build_option_weak_set (IP_ADDR1 0)
	xme_build_option_weak_set (IP_ADDR2 0)
	xme_build_option_weak_set (IP_ADDR3 0)
	
	xme_build_option_weak_set (NETMASK_ADDR0 0)
	xme_build_option_weak_set (NETMASK_ADDR1 0)
	xme_build_option_weak_set (NETMASK_ADDR2 0)
	xme_build_option_weak_set (NETMASK_ADDR3 0)
	
	xme_build_option_weak_set (GW_ADDR0 0)
	xme_build_option_weak_set (GW_ADDR1 0)
	xme_build_option_weak_set (GW_ADDR2 0)
	xme_build_option_weak_set (GW_ADDR3 0)
endif (_DHCP_AUTOIP)

# Empty default values since we assert a user setting above (in static IP mode),
# and set it to all zeros in DHCP and AutoIP mode.

xme_build_option(IP_ADDR0 "" "network_settings.h")
xme_build_option(IP_ADDR1 "" "network_settings.h")
xme_build_option(IP_ADDR2 "" "network_settings.h")
xme_build_option(IP_ADDR3 "" "network_settings.h")

xme_build_option(NETMASK_ADDR0 "" "network_settings.h")
xme_build_option(NETMASK_ADDR1 "" "network_settings.h")
xme_build_option(NETMASK_ADDR2 "" "network_settings.h")
xme_build_option(NETMASK_ADDR3 "" "network_settings.h")

xme_build_option(GW_ADDR0 "" "network_settings.h")
xme_build_option(GW_ADDR1 "" "network_settings.h")
xme_build_option(GW_ADDR2 "" "network_settings.h")
xme_build_option(GW_ADDR3 "" "network_settings.h")

### Derived options ##
# MEMP_NUM_SYS_TIMEOUT must be >= (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT)))
set (_NUM_SYS_TIMEOUT 0)
xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_TCP)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 1")
endif (${_DEFINED})

xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_IP_REASSEMBLY)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 1")
endif (${_DEFINED})

xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_ARP)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 1")
endif (${_DEFINED})

xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_DHCP)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 2")
endif (${_DEFINED})

xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_AUTOIP)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 1")
endif (${_DEFINED})

xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_IGMP)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 1")
endif (${_DEFINED})

xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_LWIP_DNS)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 1")
endif (${_DEFINED})

xme_get (_DEFINED PROPERTY_GLOBAL EXTERNAL_LWIP_PPP_SUPPORT)
if (${_DEFINED})
	math(EXPR _NUM_SYS_TIMEOUT "${_NUM_SYS_TIMEOUT} + 1")
endif (${_DEFINED})

xme_build_option(EXTERNAL_LWIP_MEMP_NUM_SYS_TIMEOUT            ${_NUM_SYS_TIMEOUT} "lwipopts.h" "EXTERNAL_LWIP_")

xme_build_option_file_close ("network_settings.h")
