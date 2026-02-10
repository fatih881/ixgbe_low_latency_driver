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
  ssize_t w;
  if (unlikely(pci == NULL)) return -1;
  const size_t pci_len = strnlen(pci, 64);
  if (unlikely(pci_len >= 64)) return -1;
  if (unlikely(target_drv == NULL || strnlen(target_drv, 64) >= 64)) return -1;
  int len = snprintf(path, sizeof(path),
                     "/sys/bus/pci/devices/%s/driver/unbind", pci);
  if (unlikely((len < 0 || (size_t)len >= sizeof(path)))) {
    return -1;
  }
  int fd = open(path, O_WRONLY);
  if (fd >= 0) {
    w = write(fd, pci, pci_len);
    if (unlikely(w != (ssize_t)pci_len)) {
      close(fd);
      return -1;
    }
    close(fd);
  }
  len = snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/driver_override",
                 pci);
  if (unlikely((len < 0 || (size_t)len >= sizeof(path)))) {
    return -1;
  }
  fd = open(path, O_WRONLY);
  if (unlikely(fd < 0)) {
    return -1;
  }
  char buf[64];
  len = snprintf(buf, sizeof(buf), "%s\n", target_drv);
  if (unlikely((len < 0 || (size_t)len >= sizeof(buf)))) {
    close(fd);
    return -1;
  }
  w = write(fd, buf, len);
  if (unlikely(w != (ssize_t)len)) {
    close(fd);
    return -1;
  }
  close(fd);

  fd = open("/sys/bus/pci/drivers_probe", O_WRONLY);
  if (unlikely(fd < 0)) return -1;
  w = write(fd, pci, pci_len);
  if (unlikely(w != (ssize_t)pci_len)) {
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}