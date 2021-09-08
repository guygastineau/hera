#ifndef HERA_H
#define HERA_H

#include <linux/wireless.h>

int iw_get_socket(void);
void iw_close_socket(int sock);

// Commit pending changes to driver. Does this require root?
int iw_commit(int sock, char *ifname);
// Get wireless extensions name.
int iw_get_we_string(int sock, char *ifname, char *dest, size_t dest_size);

// Get device names written into your own char buffers of IFNAMSIZ.
int iw_get_devices(int sock, char (*dest)[IFNAMSIZ], size_t length);
// Get devices as ifreqs written into your buffer.
int iw_get_devices_(int sock, char *dest, size_t size);

// This is marked as pre 802.11.  Maybe it is outdated.
int iw_set_network_id(int sock, char *ifname, int nwid);
int iw_get_network_id(int sock, char *ifname);

#endif // HERA_H
