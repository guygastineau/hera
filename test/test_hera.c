#include "vendor/unity.h"
#include "../src/hera.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int sock;

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

// Open and test it (then leave it for )
static void test_get_iw_socket(void)
{
  //sock = iw_get_socket();
  TEST_ASSERT(sock >= 0);
}

static void test_get_devices_lowlevel(void)
{
  struct ifreq ifrs[12];
  int count = iw_get_devices_(sock, (char *)ifrs, sizeof(ifrs));
  TEST_ASSERT_MESSAGE(count > 0, "Failed to get devices.");
}

static void test_get_devices_highlevel(void)
{
  char names[12][IFNAMSIZ];
  int count = iw_get_devices(sock, names, 12);
  TEST_ASSERT_MESSAGE(count > 0, "Failed to get devices.");
}

// Internal test! Just make sure we can determine the name.
static void test_get_wireless_device(void)
{
  char * ifname = most_likely_wireless_nic(sock);
  TEST_ASSERT_NOT_NULL_MESSAGE(ifname, "Failed to get wireless interface name!");
  free(ifname);
}

static void test_get_we_string(void)
{
  char we_name[IFNAMSIZ];
  char *ifname = most_likely_wireless_nic(sock);
  int err = iw_get_we_string(sock, ifname, we_name, IFNAMSIZ);
  TEST_ASSERT_MESSAGE(err == 0, "Failed to get wireless extensions string.");
  free(ifname);
}

int main(void)
{
  sock = iw_get_socket();
  UnityBegin("test/test_hera.c");

  RUN_TEST(test_get_iw_socket);
  RUN_TEST(test_get_devices_lowlevel);
  RUN_TEST(test_get_devices_highlevel);
  RUN_TEST(test_get_wireless_device);
  RUN_TEST(test_get_we_string);

  return UnityEnd();
  close(sock);
}
