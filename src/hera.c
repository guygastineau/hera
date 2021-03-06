#include "hera.h"

#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

// ONLY USE WITH LITERALS YOU CRAZIES!
#define MIN(X, Y) (X < Y) ? X : Y

int iw_get_socket(void)
{
  int sock;
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
    return sock;
  }
  return -1;
}

void iw_close_socket(int sock)
{
  close(sock);
}



static int iw_call_with_name(int sock, char *ifname, int cmd, struct iwreq *req)
{
  strncpy(req->ifr_name, ifname, IFNAMSIZ);
  req->ifr_name[IFNAMSIZ - 1] = 0;
  return ioctl(sock, cmd, req);
}

// Mostly not needed.
int iw_commit(int sock, char *ifname)
{
  if (!ifname) {
    return -1;
  }
  struct iwreq req;
  return iw_call_with_name(sock, ifname,SIOCSIWCOMMIT, &req);
}

int iw_get_we_string(int sock, char *ifname, char *dest, size_t dest_size)
{
  struct iwreq req;
  req.u.name[0] = 0;

  size_t name_size = MIN(IFNAMSIZ, dest_size);

  if (iw_call_with_name(sock, ifname, SIOCGIWNAME, &req) < 0) {
    return -1;
  }
  if (!strncpy(dest, req.u.name, name_size)) {
    return -1;
  }
  dest[name_size - 1] = 0;
  return 0;
}



// Get devices as ifreqs written into your buffer.
int iw_get_devices_(int sock, char *dest, size_t size)
{
  size_t written;
  struct ifconf ifc;

  if (!dest) {
    return -1;
  }

  ifc.ifc_len = size;
  ifc.ifc_buf = dest;

  if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
    return -1;
  }
  // Safely return how many we wrote.
  written = ifc.ifc_len / sizeof(struct ifreq);
  return (written <= size / sizeof(struct ifreq))
    ? (int)written
    : -1
    ;
}

// Get device names written into your own char buffers of IFNAMSIZ.
int iw_get_devices(int sock, char (*dest)[IFNAMSIZ], size_t length)
{
  int status;
  struct ifreq *ifrs;

  status = -1;

  if (!dest) {
    return status;
  }

  // Maybe this could be static?
  // I don't even know the max number of interfaces the kernel supports.
  ifrs = calloc(length, sizeof(struct ifreq));
  if (!ifrs) {
    return status;
  }

  status = iw_get_devices_(sock, (char *)ifrs, length * sizeof(struct ifreq));
  if (status < 0) {
    goto cleanup;
  }

  // The driver will return null terminated strings with a max
  // size of `IFNAMSIZ' (right?), so just memcpy should be fine here.
  for (int i = 0; i < status; ++i) {
    if(memcpy(dest[i], ifrs[i].ifr_name, IFNAMSIZ) != dest[i]) {
      status = -1;
      goto cleanup;
    }
  }

  // Fail or return how many we wrote.
 cleanup:
  free(ifrs);
  return status;
}



// I don't know what this does, and I am not comfortable testing it right now.
int iw_set_network_id(int sock, char *ifname, int nwid);


int iw_get_network_id(int sock, char *ifname)
{
  struct iwreq req;
  req.u.param = (struct iw_param){ 0, 0, 0, 0 };

  if (iw_call_with_name(sock, ifname, SIOCGIWNWID, &req) < 0) {
    return -1;
  }
  return req.u.param.value;
}
