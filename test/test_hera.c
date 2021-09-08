#include "vendor/unity.h"
#include "../src/hera.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void setUp(void)
{
}

void tearDown(void)
{
}

// Try to find a wireless interface.
// We basically just search for the letter 'w' in the if names.
// This is good enough for now, but we should probably do better.
#define NAME_LENGTH 12
static char *most_likely_wireless_nic(int sock)
{
  char *dest;
  char names[NAME_LENGTH][IFNAMSIZ];
  int count = iw_get_devices(sock, names, NAME_LENGTH);
  for (int i = 0; i < count && i < NAME_LENGTH; ++i) {
    // Check the assembly to make sure it optimizes the mem access to a register.
    for (int j = 0; j < IFNAMSIZ && names[i][j]; ++j) {
      if (names[i][j] == 'w') {
        dest = malloc(IFNAMSIZ);
        if (!dest) {
          return NULL;
        }
        if (!memcpy(dest, names[i], IFNAMSIZ)) {
          free(dest);
          return NULL;
        }
        return dest;
      }
    }
  }
  return NULL;
}
#undef NAME_LENGTH

// pen and test it (then leave it for )
static void test_get_iw_socket(void)
{
  int sock = iw_get_socket();
  TEST_ASSERT(sock >= 0);
  close(sock);
}

static void test_get_devices_lowlevel(void)
{
  int sock = iw_get_socket();
  struct ifreq ifrs[12];
  int count = iw_get_devices_(sock, (char *)ifrs, sizeof(ifrs));
  TEST_ASSERT_MESSAGE(count > 0, "Failed to get devices.");
  close(sock);
}

static void test_get_devices_highlevel(void)
{
  int sock = iw_get_socket();
  char names[12][IFNAMSIZ];
  int count = iw_get_devices(sock, names, 12);
  TEST_ASSERT_MESSAGE(count > 0, "Failed to get devices.");
  close(sock);
}

// Internal test! Just make sure we can determine the name.
static void test_get_wireless_device(void)
{
  int sock = iw_get_socket();
  char * ifname = most_likely_wireless_nic(sock);
  TEST_ASSERT_NOT_NULL_MESSAGE(ifname, "Failed to get wireless interface name!");
  free(ifname);
  close(sock);
}



int main(void)
{
   UnityBegin("test/test_hera.c");

   RUN_TEST(test_get_iw_socket);
   RUN_TEST(test_get_devices_lowlevel);
   RUN_TEST(test_get_devices_highlevel);
   RUN_TEST(test_get_wireless_device);

   return UnityEnd();
}
