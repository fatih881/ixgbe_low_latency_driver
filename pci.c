#include "base.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
/*
* Taking the device from kernel's control and binds to terget_drv.
*/
unbind(const char *pci, const char *target_drv, volatile u8 *trace )
{
    if (likely(trace)) {
        (*trace)++;
    }
    char path[128];
    int fd;
    snprintf(path, sizeof(path), 
        "/sys/bus/pci/devices/%s/driver_override", pci);

    fd = open(path, O_WRONLY);
    if (unlikely(fd < 0)) return -1;
    
    write(fd, target_drv, strlen(target_drv));
    write(fd, "\n", 1);
    close(fd);
    snprintf(path, sizeof(path), 
        "/sys/bus/pci/devices/%s/driver/unbind", pci);

    fd = open(path, O_WRONLY);
    if (likely(fd >= 0)) {
        write(fd, pci, strlen(pci));
        close(fd);
    }

    fd = open("/sys/bus/pci/drivers_probe", O_WRONLY);
    if (unlikely(fd < 0)) return -2;
    write(fd, pci, strlen(pci));
    close(fd);
    
    if (likely(trace)) {
        (*trace)++;
    } return 0;

}