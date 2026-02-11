#ifndef SELFTEST_H
#define SELFTEST_H
#include "../src/hw.h"
int ixgbe_run_diagnostic(const struct hw* hw);
int ixgbe_test_mmio(const struct hw* hw);
#endif