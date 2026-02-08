#include <unistd.h>

#include "base.h"
#include "hw.h"
#include "ixgbe.h"
/*
 * Writing registers following 82599 datasheet page 166.
 */
int ixgbe_probe(const struct hw* hw, volatile u8* trace) {
  if (likely(trace)) {
    (*trace)++;
  };
  /* Disable Interrupts */
  ixgbe_write_reg(hw, IXGBE_EIMC, 0x7FFFFFFF);
  u32 err = ixgbe_read_reg(hw, IXGBE_EIMS);
  if (unlikely(err != 0)) return -1;
  /* Global Reset */
  u32 ctrl = ixgbe_read_reg(hw, IXGBE_CTRL);
  ctrl |= (IXGBE_CTRL_RST | IXGBE_CTRL_LRST);
  ixgbe_write_reg(hw, IXGBE_CTRL, ctrl);
  usleep(10000);
  err = ixgbe_read_reg(hw, IXGBE_CTRL);
  if (unlikely((err & IXGBE_CTRL_RST) | (err & IXGBE_CTRL_LRST))) return -1;
  u8 i;
  for (i = 0; i < 50; i++) {
    const u32 eeprom = ixgbe_read_reg(hw, IXGBE_EEC);
    if (eeprom & IXGBE_EEC_ARD) goto eeprom_ok;
    usleep(1000);
  }
  if (unlikely((i == 50))) return -1;
eeprom_ok:
  /* EEPROM read duration is 20ms at max */
  for (i = 0; i < 50; i++) {
    const u32 dmaidone = ixgbe_read_reg(hw, IXGBE_RDRXCTL);
    if (dmaidone & IXGBE_RDRXCTL_DMAIDONE) goto dmaiok;
    usleep(1000);
  }
  if (unlikely((i == 50))) return -1;
dmaiok:
  /*  Proof Of Progress */
  const u32 ledctl = ixgbe_read_reg(hw, IXGBE_LEDCTL);
  const u32 blink = IXGBE_LED_CONF(0, 0x8E) | IXGBE_LED_CONF(1, 0x8E) |
                    IXGBE_LED_CONF(2, 0x8E) | IXGBE_LED_CONF(3, 0x8E);
  ixgbe_write_reg(hw, IXGBE_LEDCTL, blink);
  err = ixgbe_read_reg(hw, IXGBE_LEDCTL);
  if (unlikely((err & IXGBE_LED_RW_MASK) != (blink & IXGBE_LED_RW_MASK)))
    return -1;
  sleep(10);
  ixgbe_write_reg(hw, IXGBE_LEDCTL, ledctl);
  err = ixgbe_read_reg(hw, IXGBE_LEDCTL);
  if (unlikely((err != ledctl))) return -1;
  /*
   * LEDCTL part is temporary, added for proving the work.
   * findings are;
   * Since we are managing 1 port, the second port's LED's didn't turn on green
   * light. The LEDs are not blinked since they're blinking with traffic, and we
   * don't have one. This means all the previous configurations are successful.
   */
  if (likely(trace)) {
    (*trace)++;
  };
  return 0;
}