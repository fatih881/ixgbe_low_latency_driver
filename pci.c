#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "base.h"
/*
 * Taking the device from kernel's control and binds to target_drv.
 */
int unbind(const char* pci, const char* target_drv) {
  char path[128];
  if (unlikely(pci == NULL || strnlen(pci, 64) >= 64)) return -1;
  if (unlikely(target_drv == NULL || strnlen(target_drv, 64) >= 64)) return -1;
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/driver/unbind", pci);

  int fd = open(path, O_WRONLY);
  if (likely(fd >= 0)) {
    write(fd, pci, strnlen(pci, 64));
    close(fd);
  }
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/driver_override", pci);

  fd = open(path, O_WRONLY);
  if (unlikely(fd < 0)) {
    close(fd);
    return -1;
  }
  char buf[64];
  const int len = snprintf(buf, sizeof(buf), "%s\n", target_drv);
  if (unlikely((len < 0 || (size_t)len >= sizeof(buf)))) {
    close(fd);
    return -1;
  }
  write(fd, buf, len);
  close(fd);
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/driver/unbind", pci);

  fd = open(path, O_WRONLY);
  if (likely(fd >= 0)) {
    write(fd, pci, strnlen(pci, 64));
    close(fd);
  }

  fd = open("/sys/bus/pci/drivers_probe", O_WRONLY);
  if (unlikely(fd < 0)) {
    close(fd);
    return -2;
  }
  write(fd, pci, strnlen(pci, 64));
  close(fd);

  return 0;
}