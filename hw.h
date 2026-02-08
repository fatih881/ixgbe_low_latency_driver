#ifndef HW_H
#define HW_H

#include "base.h"
struct hw {
  const char* pci_addr;
  u8* hw_addr;
  void* rx_base;
  u64 rx_base_phy;
  u32 rx_head;
  u32 rx_tail;
  volatile u32* rdt_reg_addr;
  volatile u32* rdh_reg_addr;
};
extern struct hw ixgbe_adapter;
struct trace {
  volatile u8 unbind_counter;
  volatile u8 alloc_hugepage_counter;
  volatile u8 virt2phy_counter;
  volatile u8 mmap_bar0_counter;
  volatile u8 probe_counter;
};
extern struct trace debug_trace;

int alloc_hugepage(struct hw* hw, volatile u8* trace);
int virt2phy(struct hw* hw, volatile u8* trace);
int mmap_bar0(struct hw* hw, volatile u8* trace);
#endif