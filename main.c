#include "base.h"
#include "hw.h"
#include "pci.h"
#include <stddef.h>
#include <unistd.h>
struct trace debug_trace __attribute__((aligned(64))) = {0};
struct hw ixgbbe_adapter __attribute__((aligned(64))) = {0};

int main(int argc, char **argv){
    if (unlikely(argc < 2 )){
        write(STDERR_FILENO,
            "usage: ./binary <pci_addr>. use lspci for PCI addr.\n", 52);
        return -1;
    };
    ixgbbe_adapter.pci_addr = argv[1];
    // Driver should be changed for another PCI direct acccess modes.
    int err = unbind(ixgbbe_adapter.pci_addr, "uio_pci_generic",&debug_trace.unbind_counter);
    if (unlikely(err != 0)){
        return -err;
    }
    err = alloc_hugepage(&ixgbbe_adapter, &debug_trace.alloc_hugepage_counter);
    if (unlikely(err != 0)){
        return -err;
    }
    err = virt2phy(&ixgbbe_adapter, &debug_trace.virt2phy_counter);
    if (unlikely(err != 0)){
        return -err;
    }
    err = mmap_bar0(&ixgbbe_adapter,&debug_trace.mmap_bar0_counter);
    if (unlikely(err != 0)){
        return -err;
    }
};

