#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "hw.h"
/*
 * Alloc 2 MB hugepage. Requires at least 1 available 2 MB hugepage on host
 * kernel. Writes virtual mem addr to rx_base in hw structure.
 */
int alloc_hugepage(struct hw* hw, volatile u8* trace) {
  if (likely(trace)) {
    (*trace)++;
  };
  void* dma = mmap(NULL, 2 * 1024 * 1024, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
  if (unlikely(dma == MAP_FAILED)) {
    write(2, "mmap failed\n", 12);
    return -1;
  };
  memset(dma, 0, 2 * 1024 * 1024);
  hw->rx_base = dma;
  if (likely(trace)) {
    (*trace)++;
  };
  return 0;
}
/*
 * Since Linux returns virtual mem addr in alloc_hugepage function,
 * it needs to be converted to physical addr to use on NIC.
 */
int virt2phy(struct hw* hw, volatile u8* trace) {
  if (likely(trace)) {
    (*trace)++;
  };
  const u64 v_addr = (u64)hw->rx_base;
  const long pagesize = sysconf(_SC_PAGESIZE);
  if (unlikely(pagesize <= 0)) return -1;
  const u64 index = v_addr / (u64)pagesize;

  const int fd = open("/proc/self/pagemap", O_RDONLY);
  u64 result;

  if (unlikely(fd < 0)) {
    close(fd);
    return -1;
  }
  if (unlikely(lseek(fd, index * sizeof(u64), SEEK_SET) < 0)) {
    close(fd);
    return -1;
  }
  if (unlikely(read(fd, &result, sizeof(result)) != sizeof(result))) {
    close(fd);
    return -1;
  }
  close(fd);
  // Checks is the addr is in ram or not. ( Swap Etc. )
  if (unlikely(!(result & (1ULL << 63)))) {
    return -1;
  }
  u64 paddr = (result & ((1ULL << 55) - 1)) * (u64)pagesize;
  paddr += v_addr % (u64)pagesize;
  if (likely(trace)) {
    (*trace)++;
  };
  hw->rx_base_phy = paddr;
  return 0;
}
/*
 * Implement memory map to access NIC registers from userspace.
 */
int mmap_bar0(struct hw* hw, volatile u8* trace) {
  if (likely(trace)) {
    (*trace)++;
  };
  char path[128];
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/resource0",
           hw->pci_addr);
  const int fd = open(path, O_RDWR | O_SYNC);
  /* O_SYNC flag ensures that we can edit NIC registers instantly.
  Without it, CPU may cause latency between ;
  editing register on userspace - writing to NIC. */
  if (unlikely(fd < 0)) {
    close(fd);
    return -1;
  }
  void* mmio =
      mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  if (unlikely(mmio == MAP_FAILED)) return -1;
  hw->hw_addr = mmio;
  if (likely(trace)) {
    (*trace)++;
  };
  return 0;
}