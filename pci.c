#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "base.h"
/*
 * Taking the device from kernel's control and binds to target_drv.
 */
int unbind(const char* pci, const char* target_drv, volatile u8* trace) {
  if (likely(trace)) {
    (*trace)++;
  }
  char path[128];
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/driver/unbind", pci);

  int fd = open(path, O_WRONLY);
  if (likely(fd >= 0)) {
    write(fd, pci, strlen(pci));
    close(fd);
  }
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/driver_override", pci);

  fd = open(path, O_WRONLY);
  if (unlikely(fd < 0)) return -1;

  char buf[64];
  const int len = snprintf(buf, sizeof(buf), "%s\n", target_drv);
  write(fd, buf, len);
  close(fd);
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/driver/unbind", pci);

  fd = open(path, O_WRONLY);
  if (likely(fd >= 0)) {
    write(fd, pci, strlen(pci));
    close(fd);
  }

  fd = open("/sys/bus/pci/drivers_probe", O_WRONLY);
  if (unlikely(fd < 0)) return -2;
  write(fd, pci, strlen(pci));
  close(fd);

  if (likely(trace)) {
    (*trace)++;
  }
  return 0;
}