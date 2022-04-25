#
# Copyright (c) 2011-2012, fortiss GmbH.
# Licensed under the Apache License, Version 2.0.
#
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# http://chromosome.fortiss.org.
#
# This file is part of CHROMOSOME.
#
# $Id$
#
# Author:
#         Simon Barner <barner@fortiss.org>
#

# Define generic settings

# Definition to announce the usage of lwIP
xme_definition(
	"-DXME_USE_LWIP"
)

# Set lwIP src dir: CACHE Variable XME_EXTERNAL_LWIP_SCR_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_SCR_DIR)
	find_file (XME_EXTERNAL_LWIP_SCR_DIR "src" PATHS "${XME_EXTERNAL_LWIP_DIR}" DOC "CHROMOSOME external directory: lwIP src" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_SCR_DIR)

# Set lwIP API dir: CACHE Variable XME_EXTERNAL_LWIP_API_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_API_DIR)
	find_file (XME_EXTERNAL_LWIP_API_DIR "api" PATHS "${XME_EXTERNAL_LWIP_SCR_DIR}" DOC "CHROMOSOME external directory: lwIP API" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_API_DIR)

# Set lwIP core dir: CACHE Variable XME_EXTERNAL_LWIP_CORE_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_CORE_DIR)
	find_file (XME_EXTERNAL_LWIP_CORE_DIR "core" PATHS "${XME_EXTERNAL_LWIP_SCR_DIR}" DOC "CHROMOSOME external directory: lwIP core" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_CORE_DIR)

# Set lwIP netif dir: CACHE Variable XME_EXTERNAL_LWIP_NETIF_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_NETIF_DIR)
	find_file (XME_EXTERNAL_LWIP_NETIF_DIR "netif" PATHS "${XME_EXTERNAL_LWIP_SCR_DIR}" DOC "CHROMOSOME external directory: lwIP netif" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_NETIF_DIR)

# Set lwIP include dir: CACHE Variable XME_EXTERNAL_LWIP_INCLUDE_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_INCLUDE_DIR)
	find_file (XME_EXTERNAL_LWIP_INCLUDE_DIR "include" PATHS "${XME_EXTERNAL_LWIP_SCR_DIR}" DOC "CHROMOSOME external directory: lwIP include" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_INCLUDE_DIR)

# Set lwIP IPv4 include dir: CACHE Variable XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR)
	find_file (XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR "ipv4" PATHS "${XME_EXTERNAL_LWIP_INCLUDE_DIR}" DOC "CHROMOSOME external directory: lwIP IPv4 include" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR)

# Set lwIP IPv6 include dir: CACHE Variable XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR)
	find_file (XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR "ipv6" PATHS "${XME_EXTERNAL_LWIP_INCLUDE_DIR}" DOC "CHROMOSOME external directory: lwIP IPv6 include" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR)

# Include directories
xme_include_directory(
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}
	${XME_EXTERNAL_LWIP_DIR}	
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}
	"${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip"
)

# Components
xme_add_component(
	"xme_lwip_core"
	${XME_EXTERNAL_LWIP_CORE_DIR}/def.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/dhcp.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/dns.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/init.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/mem.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/memp.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/netif.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/pbuf.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/raw.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/stats.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/sys.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/tcp.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/tcp_in.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/tcp_out.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/timers.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/udp.c
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/arch.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/debug.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/def.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/dhcp.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/dns.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/init.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/mem.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/memp.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/memp_std.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/netif.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/opt.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/pbuf.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/raw.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/sio.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/stats.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/sys.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/tcp.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/timers.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/udp.h

	# TODO: XXX
	${XME_EXTERNAL_LWIP_API_DIR}/api_lib.c
	${XME_EXTERNAL_LWIP_API_DIR}/api_msg.c
	${XME_EXTERNAL_LWIP_API_DIR}/err.c
	${XME_EXTERNAL_LWIP_API_DIR}/netbuf.c
	${XME_EXTERNAL_LWIP_API_DIR}/netdb.c
	${XME_EXTERNAL_LWIP_API_DIR}/netifapi.c
	${XME_EXTERNAL_LWIP_API_DIR}/sockets.c
	${XME_EXTERNAL_LWIP_API_DIR}/tcpip.c

	xme_hal_dio
)

xme_add_component(
	"xme_lwip_core_ipv4"
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/autoip.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/icmp.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/igmp.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/inet.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/inet_chksum.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/ip.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/ip_addr.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv4/ip_frag.c
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/autoip.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/icmp.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/igmp.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/inet.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/inet_chksum.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/ip.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/ip_addr.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV4_DIR}/lwip/ip_frag.h
	xme_lwip_core
)

xme_add_component(
	"xme_lwip_core_ipv6"
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv6/icmp6.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv6/inet6.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv6/ip6.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/ipv6/ip6_addr.c
	${XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR}/lwip/icmp.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR}/lwip/inet.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR}/lwip/ip.h
	${XME_EXTERNAL_LWIP_INCLUDE_IPV6_DIR}/lwip/ip_addr.h
	xme_lwip_core
)

xme_add_component(
	"xme_lwip_core_snmp"
	${XME_EXTERNAL_LWIP_CORE_DIR}/snmp/asn1_dec.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/snmp/asn1_enc.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/snmp/mib2.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/snmp/mib_structs.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/snmp/msg_in.c
	${XME_EXTERNAL_LWIP_CORE_DIR}/snmp/msg_out.c
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/snmp.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/snmp_asn1.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/snmp_msg.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/snmp_structs.h
	xme_lwip_core
)

xme_add_component(
	"xme_lwip_api"
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/api.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/api_msg.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/err.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/netbuf.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/netdb.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/netifapi.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/sockets.h
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/lwip/tcpip.h
	
	xme_lwip_core
)

xme_add_component(
	"xme_lwip_netif_arp"
	${XME_EXTERNAL_LWIP_NETIF_DIR}/etharp.c
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/netif/etharp.h
	xme_lwip_core
)

xme_add_component(
	"xme_lwip_netif_slip"
	${XME_EXTERNAL_LWIP_NETIF_DIR}/slipif.c
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/netif/slipif.h
	xme_lwip_core
)

xme_add_component(
	"xme_lwip_netif_ppp"
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/auth.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/auth.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/chap.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/chap.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/chpms.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/chpms.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/fsm.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/fsm.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/ipcp.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/ipcp.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/lcp.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/lcp.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/magic.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/magic.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/md5.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/md5.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/pap.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/pap.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/ppp.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/ppp.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/pppdebug.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/randm.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/randm.h
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/vj.c
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/vj.h
	xme_lwip_core
)

xme_add_component(
	"xme_lwip_netif_pppoe"
	${XME_EXTERNAL_LWIP_NETIF_DIR}/ppp/ppp_oe.c
	${XME_EXTERNAL_LWIP_INCLUDE_DIR}/netif/ppp_oe.h
	xme_lwip_core
)

# Define architecture specific settings
xme_defined (_XME_TARGET_DEFINED PROPERTY_GLOBAL XME_TARGET)

if (NOT _XME_TARGET_DEFINED)
	set (_XME_TARGET "_")
else (NOT _XME_TARGET_DEFINED)
	xme_get (_XME_TARGET PROPERTY_GLOBAL XME_TARGET)
endif (NOT _XME_TARGET_DEFINED)

# Set lwIP port dir: CACHE Variable XME_EXTERNAL_LWIP_PORT_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_LWIP_PORT_DIR)
	find_file (XME_EXTERNAL_LWIP_PORT_DIR "port" PATHS "${XME_EXTERNAL_LWIP_DIR}" DOC "CHROMOSOME external directory: lwIP port" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_LWIP_PORT_DIR)

# Ethernet driver
xme_include_directory(
	${XME_EXTERNAL_LWIP_PORT_DIR}
)

xme_add_component (
	"xme_lwip_netif_ethernet"
	xme_lwip_netif_arp
	xme_target_bsp
	${XME_EXTERNAL_LWIP_PORT_DIR}/enet.c
	${XME_EXTERNAL_LWIP_PORT_DIR}/enet.h
)

	
# Platform and target specific LWIP code
xme_get (_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)
xme_get (_XME_LWIP_PLATFORM PROPERTY_GLOBAL XME_LWIP_PLATFORM)
xme_get (_XME_TARGET PROPERTY_GLOBAL XME_TARGET)

if (${_XME_ARCHITECTURE} STREQUAL "ARMv7-M")
	#  Cortex M3
	
	xme_include_directory(
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M
	)

	xme_add_component (
		"xme_lwip_netif_ethernet"
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/bpstruct.h
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/cc.h
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/cpu.h
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/epstruct.h
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/init.h
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/lib.h
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/perf.c
		${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/arch/perf.h
	)
	
	if (${_XME_TARGET} STREQUAL "STM32F107VC")
		xme_include_directory(
			${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32
		)

		xme_add_component (
			"xme_lwip_netif_ethernet"
			${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/lwiplib.c
			${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/lwiplib.h
			${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/stm32_eth.c
			${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/stm32_eth.h
			xme_stm32f0xspl_misc
			xme_stm32f0xspl_gpio
		)
		
		if (${_XME_LWIP_PLATFORM} STREQUAL "freertos")
			xme_include_directory(
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/FreeRTOS
			)
		
			xme_definition(
				-DLWIP_PLATFORM_FREERTOS
			)
			
			# Architecture dependent FreeRTOS support
			xme_add_component (
				"xme_lwip_netif_ethernet"
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/FreeRTOS/ethernetif.c
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/FreeRTOS/ethernetif.h
			)
		elseif (${_XME_LWIP_PLATFORM} STREQUAL "baremetal")
			message ("LWIP is running on bare metal. You will not be able to use the high-level APIs")
			
			xme_include_directory(
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/baremetal
			)
			
			xme_definition(
				-DNO_SYS
			)
			
			xme_add_component (
				"xme_lwip_netif_ethernet"
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/baremetal/ethernetif.c
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/baremetal/ethernetif.h
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/baremetal/timer.c
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/stm32/baremetal/timer.h
			)
		else (${_XME_LWIP_PLATFORM} STREQUAL "baremetal")
			message (FATAL_ERROR "XME_LWIP_PLATFORM '${_XME_LWIP_PLATFORM}' is not supported.")
		endif (${_XME_LWIP_PLATFORM} STREQUAL "freertos")
	
	elseif (${_XME_TARGET} STREQUAL "LM3S8962")
			xme_include_directory(
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/lm3s8962
			)

			xme_add_component (
				"xme_lwip_netif_ethernet"
				${XME_EXTERNAL_STELLARISWARE_UTIL_DIR}/lwiplib.c
				${XME_EXTERNAL_STELLARISWARE_UTIL_DIR}/lwiplib.h				
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/lm3s8962/stellarisif.c
				${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/lm3s8962/stellarisif.h
			)
			if (${_XME_LWIP_PLATFORM} STREQUAL "freertos")
				xme_include_directory(
					${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/lm3s8962/FreeRTOS
				)
				
				xme_definition(
					-DLWIP_PLATFORM_FREERTOS
				)
			elseif (${_XME_LWIP_PLATFORM} STREQUAL "baremetal")
				message ("LWIP is running on bare metal. You will not be able to use the high-level APIs")

				xme_include_directory(
					${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/lm3s8962/baremetal
				)
				
				xme_definition(
					-DNO_SYS
				)
				
				xme_add_component (
					"xme_lwip_netif_ethernet"
					${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/lm3s8962/baremetal/sys_arch.c
					${XME_EXTERNAL_LWIP_PORT_DIR}/ARMv7-M/lm3s8962/baremetal/sys_arch.h
				)
			else (${_XME_LWIP_PLATFORM} STREQUAL "baremetal")
				message (FATAL_ERROR "XME_LWIP_PLATFORM '${_XME_LWIP_PLATFORM}' is not supported.")
			endif (${_XME_LWIP_PLATFORM} STREQUAL "freertos")
	else (${_XME_TARGET} STREQUAL "LM3S8962")
		message (FATAL_ERROR "XME_TARGET '${_XME_TARGET}' is not supported.")
	endif (${_XME_TARGET} STREQUAL "STM32F107VC")


elseif (${_XME_ARCHITECTURE} STREQUAL  "NiosII")
		# Cyclone III 
		xme_include_directory(
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS
		)
	
		xme_include_directory(
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/arch
		)
			
		# Architecture independent FreeRTOS support
		xme_add_component (
			"xme_lwip_netif_ethernet"
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/lwiplib.c
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/lwiplib.h
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/sys_arch.c
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/sys_arch.h
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/arch/cc.h
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/arch/perf.h
			${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/arch/perf.c
			xme_freertos_kernel
			xme_freertos_timers
		)
else (${_XME_ARCHITECTURE} STREQUAL "NiosII")
	message (FATAL_ERROR "XME_TARGET '${_XME_TARGET}' is not supported.")
	
endif (${_XME_ARCHITECTURE} STREQUAL "ARMv7-M")

if (${_XME_LWIP_PLATFORM} STREQUAL "freertos")
	xme_include_directory(
		${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS
	)
			
	# Architecture independent FreeRTOS support
	xme_add_component (
		"xme_lwip_netif_ethernet"
		${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/sys_arch.c
		${XME_EXTERNAL_LWIP_PORT_DIR}/FreeRTOS/sys_arch.h
		xme_freertos_kernel
		xme_freertos_timers
	)
endif (${_XME_LWIP_PLATFORM} STREQUAL "freertos")
