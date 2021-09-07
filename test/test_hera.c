#include "vendor/unity.h"
#include "../src/hera.h"

#include <stdio.h>
#include <unistd.h>

void setUp(void)
{
}

void tearDown(void)
{
}

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

#ifdef DEBUG
  printf("Got %d devices:\n", count);
  for (int i = 0; i < count; ++i) {
    printf("  %s\n", ifrs[i].ifr_name);
  }
#endif // DEBUG

  close(sock);
}

static void test_get_devices_highlevel(void)
{
  int sock = iw_get_socket();
  char names[12][IFNAMSIZ];
  int count = iw_get_devices(sock, names, 12);
  TEST_ASSERT_MESSAGE(count > 0, "Failed to get devices.");

#ifdef DEBUG
  printf("Got %d devices:\n", count);
  for (int i = 0; i < count; ++i) {
    printf("  %s\n", names[i]);
  }
#endif // DEBUG

  close(sock);
}

int main(void)
{
   UnityBegin("test/test_hera.c");

   RUN_TEST(test_get_iw_socket);
   RUN_TEST(test_get_devices_lowlevel);
   RUN_TEST(test_get_devices_highlevel);

   return UnityEnd();
}
