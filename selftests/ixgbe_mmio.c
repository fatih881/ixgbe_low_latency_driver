#include "../src/hw.h"
#include "../src/ixgbe.h"
#include "selftests.h"
/*
 * Entry point for self tests.
 * This function is designed to prevent overhead before initializing hardware.
 * For now, it's using LED registers.
 */
int ixgbe_run_diagnostic(const struct hw* hw) {
  const int read_val = ixgbe_test_mmio(hw);
  if (unlikely(read_val != 0)) return read_val;
  return 0;
}
/*
 * Currently, only logic for selftest.
 * Implemented as a helper for self tests entry for modularity.
 * Usage of this function will not affect hardware state because of backup at
 * start-reload backup logic.
 */
int ixgbe_test_mmio(const struct hw* hw) {
  const u32 origin_state = ixgbe_read_reg(hw, IXGBE_LEDCTL);
  if (unlikely(origin_state == 0xFFFFFFFF)) return -1;
  const u32 ledtest = IXGBE_LED_CONF(0, 0x8E) | IXGBE_LED_CONF(1, 0x8E) |
                      IXGBE_LED_CONF(2, 0x8E) | IXGBE_LED_CONF(3, 0x8E);
  ixgbe_write_reg(hw, IXGBE_LEDCTL, ledtest);
  u32 read_val = ixgbe_read_reg(hw, IXGBE_LEDCTL);
  if (unlikely((read_val & IXGBE_LED_RW_MASK) !=
               (ledtest & IXGBE_LED_RW_MASK))) {
    ixgbe_write_reg(hw, IXGBE_LEDCTL, origin_state);
    return -1;
  }
  ixgbe_write_reg(hw, IXGBE_LEDCTL, origin_state);
  read_val = ixgbe_read_reg(hw, IXGBE_LEDCTL);
  if (unlikely((read_val != origin_state))) return -1;
  return 0;
}
