#ifdef __linux__
#define _GNU_SOURCE
#include <linux/securebits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/capability.h>
#include <sys/errno.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "Rts.h"

void debug (char *label) {
  printf("%s: received signal in tid=%d\n", label, syscall(SYS_gettid));
}

void FlagDefaultsHook (void) {
  debug("keep_capabilities");
  int r = prctl(PR_SET_SECUREBITS, SECBIT_KEEP_CAPS, 0L, 0L, 0L);
  if (r < 0) {
    printf("keep_capabilities: %s\n", strerror(errno));
  }
}

void drop_except_bind (void) {
  debug("drop_except_bind");
  cap_user_header_t header = malloc(sizeof(*header));
  header->version = _LINUX_CAPABILITY_VERSION_3;
  header->pid = 0;

  uint32_t bind = 1 << CAP_NET_BIND_SERVICE;

  cap_user_data_t data = malloc(sizeof(*data));
  data->effective = bind;
  data->permitted = bind;
  data->inheritable = bind;

  int r = capset(header,data);

  free(header);
  free(data);

  if (r < 0) {
    printf("drop_except_bind: %s\n", strerror(errno));
  }
}

void send_signal (int tid, int sig) {
  printf("send signal to tid=%d\n", tid);
  int tgid = getpid();
  int r = syscall(SYS_tgkill, tgid, tid, sig);

  if (r < 0) {
    printf("send_signald: %d\n", strerror(errno));
  }
}

#else
void drop_except_bind (void) {
}
void send_signal (int tid, int sig) {
}
#endif
