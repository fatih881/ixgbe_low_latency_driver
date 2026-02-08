#ifndef PCI_H
#define PCI_H
#include "base.h"

u8 unbind(const char* pci, const char* target_drv, volatile u8* trace);
#endif