#ifndef HERA_H
#define HERA_H

#include <linux/wireless.h>

int iw_get_socket();
int iw_get_we_string(int sock, char *ifname, char *dest, size_t dest_size);
// Get devices as ifreqs written into your buffer.
int iw_get_devices_(int sock, char *dest, size_t size);
// Get device names written into your own char buffers of IFNAMSIZ.
int iw_get_devices(int sock, char (*dest)[IFNAMSIZ], size_t length);

#endif // HERA_H
