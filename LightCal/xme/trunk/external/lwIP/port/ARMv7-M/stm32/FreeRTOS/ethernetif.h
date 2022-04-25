#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

// TODO: Defines in the stelaris hw_ethernet.h. Needed for the stm32_interrupt function
#define MAC_TR_NEWTX            0x00000001  // New Transmission
#define MAC_O_TR                0x00000038  // Ethernet MAC Transmission

err_t ethernetif_init(struct netif *netif);
struct netif *ethernetif_register(void);
int ethernetif_poll(void);
void Set_MAC_Address(unsigned char* macadd);
void stm32_ethernetif_input(struct netif *netif);

#endif // #ifndef __ETHERNETIF_H__
