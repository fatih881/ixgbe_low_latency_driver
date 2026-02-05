#ifndef HW_H
#define HW_H

#include "base.h"
struct hw {
    u8 *hw_addr;
    void *rx_base;
    u64 rx_base_phy;
    u32 rx_head;
    u32 rx_tail;
    volatile u32 *rdt_reg_addr;
    volatile u32 *rdh_reg_addr;
};

struct trace
{
    volatile u8 unbind;
};
extern struct trace debug_trace;

int alloc_hugepage(struct hw *hw);
int virt2phy(struct hw *hw);
#endif