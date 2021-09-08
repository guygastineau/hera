#include "vendor/unity.h"
#include "../src/hera.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int sock;
static char ifname[IFNAMSIZ];

void setUp(void)
{
}

void tearDown(void)
{
}

// Try to find a wireless interface.
// We basically just search for the letter 'w' in the if names.
// This is good enough for now, but we should probably do better.
#define NAMES_LENGTH 12
static int most_likely_wireless_nic(int sock, char dest[IFNAMSIZ])
{
  char names[NAMES_LENGTH][IFNAMSIZ];
  int count = iw_get_devices(sock, names, NAMES_LENGTH);
  for (int i = 0; i < count && i < NAMES_LENGTH; ++i) {
    // Check the assembly to make sure it optimizes the mem access to a register.
    for (int j = 0; j < IFNAMSIZ && names[i][j]; ++j) {
      if (names[i][j] == 'w') {
        if (!memcpy(dest, names[i], IFNAMSIZ)) {
          return -1;
        }
        return 0;
      }
    }
  }
  return -1;
}
#undef NAMES_LENGTH

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

// Internal test! Just make sure we can determine the name,
// and store it globally (so we don't repeat ourselves).
static void test_get_wireless_device(void)
{
  int err = most_likely_wireless_nic(sock, ifname);
  TEST_ASSERT_MESSAGE(err == 0, "Failed to get wireless interface name!");
}

static void test_get_we_string(void)
{
  char we_name[IFNAMSIZ];
  int err = iw_get_we_string(sock, ifname, we_name, IFNAMSIZ);
  TEST_ASSERT_MESSAGE(err == 0, "Failed to get wireless extensions string.");
}

static void test_commit(void)
{
  // This will probably fail when no changes have been made.
  // It is largely only usefull for forcing changes during debugging.
  TEST_IGNORE();
  TEST_ASSERT(iw_commit(sock, ifname) >= 0);
}

static void test_get_network_id(void)
{
  // This will likely fail unless you have old hardware.
  TEST_IGNORE();
  TEST_ASSERT(iw_get_network_id(sock, ifname) >= 0);
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
  RUN_TEST(test_commit);
  RUN_TEST(test_get_network_id);

  return UnityEnd();
  close(sock);
}
