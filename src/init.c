#include <errno.h>
#include <unistd.h>

#include "base.h"
#include "hw.h"
#include "ixgbe.h"
/*
 * Initialize ASIC following 82599 datasheet page 166.
 */
int ixgbe_probe(const struct hw* hw) {
  u32 delay = 10;
  /* Disable Interrupts */
  ixgbe_write_reg(hw, IXGBE_EIMC, 0x7FFFFFFF);
  u32 err = ixgbe_read_reg(hw, IXGBE_EIMS);
  if (unlikely(err != 0)) return -EIO;
  /* Global Reset */
  u32 ctrl = ixgbe_read_reg(hw, IXGBE_CTRL);
  ctrl |= (IXGBE_CTRL_RST | IXGBE_CTRL_LRST);
  ixgbe_write_reg(hw, IXGBE_CTRL, ctrl);
  /* ~10.23 ms at total. +~10ms means malfunctional behavior. */
  delay = 10;
  for (u8 i = 0; i < 15; i++) {
    err = ixgbe_read_reg(hw, IXGBE_CTRL);
    if (likely(!(err & (IXGBE_CTRL_RST | IXGBE_CTRL_LRST)))) break;
    usleep(delay);
    if (likely(delay < 1000)) delay *= 2;
  }
  if (unlikely(err & (IXGBE_CTRL_RST | IXGBE_CTRL_LRST))) return -ETIMEDOUT;
  /* Reset delay value since it'll continue from Global Reset. */
  delay = 10;
  for (u8 i = 0; i < 23; i++) {
    const u32 eeprom = ixgbe_read_reg(hw, IXGBE_EEC);
    if (eeprom & IXGBE_EEC_ARD) goto eeprom_ok;
    usleep(delay);
    if (likely(delay < 1000)) delay *= 2;
  }
  return -ENODEV;
eeprom_ok:
  /* ~20.47 ms at total. +~20ms means malfunctional behavior. */
  delay = 10;
  for (u8 i = 0; i < 23; i++) {
    const u32 dmaidone = ixgbe_read_reg(hw, IXGBE_RDRXCTL);
    if (dmaidone & IXGBE_RDRXCTL_DMAIDONE) goto dmaiok;
    usleep(delay);
    if (likely(delay < 1000)) delay *= 2;
  }
  return -ETIMEDOUT;
dmaiok:;
  return 0;
}