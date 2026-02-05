#ifndef IXGBE_H
#define IXGBE_H

#include "base.h"
#include "hw.h"

static inline u32 ixgbe_read_reg(struct hw *hw, u32 reg) {
    return *((volatile u32 *)(hw->hw_addr + reg));
}

static inline void ixgbe_write_reg(struct hw *hw, u32 reg, u32 val) {
    *((volatile u32 *)(hw->hw_addr + reg)) = val;
}
#endif