#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>

int main() {
    int fd = wiringPiI2CSetup(0x16);
    if (fd == -1) {
        printf("Failed to init I2C communication.\n");
        return -1;
    }
    printf("I2C communication successfully setup.\n");

    int result = wiringPiI2CRead(fd);
    if (result == -1) {
        printf("Failed to read from the i2c bus.\n");
    } else {
        printf("Data read from i2c bus: %d\n", result);
    }

    return 0;
}
