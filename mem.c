#include <sys/mman.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "hw.h"
/*
 * Alloc 2 MB hugepage. Requires at least 1 available 2 MB hugepage on host kernel.
 * Writes virtual mem addr to rx_base in hw structure.
 */
alloc_hugepage(struct hw *hw) 
{
    void *dma = mmap(NULL,
         2 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
         if(unlikely(dma == MAP_FAILED)) {
            write(2, "mmap failed\n", 12); 
            return -1;
         };
    memset(dma, 0, 2 * 1024 * 1024);
    hw->rx_base = dma;
    return 0;
};
/*
 * Since Linux returns virtual mem addr in alloc_hugepage function,
 * it needs to be converted to physical addr to use on NIC.
 */
virt2phy(struct hw *hw)
{   
    u64 v_addr = (u64)hw->rx_base;
    u64 index = v_addr / sysconf(_SC_PAGESIZE);

    int fd = open("/proc/self/pagemap", O_RDONLY);
    u64 result;
    u64 paddr;

    if (unlikely(fd < 0)) 
        return ~0ULL;
    if (unlikely(lseek(fd, index * sizeof(u64), SEEK_SET) < 0)) {
        close(fd);
        return ~0ULL;
    }
    if (unlikely(read(fd, &result, sizeof(result)) != sizeof(result))) {
        close(fd);
        return ~0ULL;
    }
    close(fd);
    // Checks is the addr is in ram or not. ( Swap Etc. )
    if (unlikely(!(result & (1ULL << 63)))) {
        return ~0ULL;
    }
    paddr = (result & ((1ULL << 55) - 1)) * sysconf(_SC_PAGESIZE);
    paddr += v_addr % sysconf(_SC_PAGESIZE);
    return paddr;
};