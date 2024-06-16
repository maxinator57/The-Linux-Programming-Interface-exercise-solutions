#include <linux/reboot.h>
#include <stdint.h>
#include <stdio.h>


void PrintHexDate(uint32_t hexDate) {
    printf(
        "%x.%x.%x\n",
        hexDate >> 24,
        (hexDate >> 16) & 0xff,
        hexDate & 0xffff
    );
}


int main() {
    printf("%x\n", LINUX_REBOOT_MAGIC1);

    printf("%x ", LINUX_REBOOT_MAGIC2);
    PrintHexDate(LINUX_REBOOT_MAGIC2);

    printf("%-8x ", LINUX_REBOOT_MAGIC2A);
    PrintHexDate(LINUX_REBOOT_MAGIC2A);

    printf("%x ", LINUX_REBOOT_MAGIC2B);
    PrintHexDate(LINUX_REBOOT_MAGIC2B);

    printf("%x ", LINUX_REBOOT_MAGIC2C);
    PrintHexDate(LINUX_REBOOT_MAGIC2C);
}
