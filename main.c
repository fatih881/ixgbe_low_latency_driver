#include <unistd.h>

#include "base.h"
#include "hw.h"
#include "ixgbe.h"
#include "pci.h"
struct trace debug_trace __attribute__((aligned(64))) = {0};
struct hw ixgbe_adapter __attribute__((aligned(64))) = {0};

int main(const int argc, char** argv) {
  if (unlikely(argc < 2)) {
    write(STDERR_FILENO,
          "usage: ./binary <pci_addr>. use lspci for PCI addr.\n", 52);
    return -1;
  }
  ixgbe_adapter.pci_addr = argv[1];
  // Driver should be changed for another PCI direct access modes.
  int err = unbind(ixgbe_adapter.pci_addr, "uio_pci_generic",
                   &debug_trace.unbind_counter);
  if (unlikely(err != 0)) {
    return -err;
  }
  err = alloc_hugepage(&ixgbe_adapter, &debug_trace.alloc_hugepage_counter);
  if (unlikely(err != 0)) {
    return -err;
  }
  err = virt2phy(&ixgbe_adapter, &debug_trace.virt2phy_counter);
  if (unlikely(err != 0)) {
    return -err;
  }
  err = mmap_bar0(&ixgbe_adapter, &debug_trace.mmap_bar0_counter);
  if (unlikely(err != 0)) {
    return -err;
  }
  err = ixgbe_probe(&ixgbe_adapter, &debug_trace.probe_counter);
  if (unlikely(err != 0)) {
    return -err;
  }
}