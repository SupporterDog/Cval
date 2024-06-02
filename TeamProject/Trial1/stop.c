#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// I2C 장치 경로 (일반적으로 /dev/i2c-1)
#define I2C_DEVICE_PATH "/dev/i2c-1"
// I2C 장치 주소
#define I2C_DEVICE_ADDR 0x16

// i2cset 명령어에 해당하는 함수
void i2cset_command(int fd, int reg, int value) {
    unsigned char data[2];
    data[0] = reg;
    data[1] = value;

    if (ioctl(fd, I2C_SLAVE, I2C_DEVICE_ADDR) < 0) {
        perror("ioctl failed to select device");
        exit(EXIT_FAILURE);
    }

    if (write(fd, data, 2) != 2) {
        perror("write failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int fd;

    // I2C 장치 열기
    if ((fd = open(I2C_DEVICE_PATH, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        exit(EXIT_FAILURE);
    }

    // i2cset 명령어 실행
    i2cset_command(fd, 0x01, 0x01);
    i2cset_command(fd, 0x01, 0x32);
    i2cset_command(fd, 0x01, 0x32);

    // I2C 장치 닫기
    close(fd);

    return 0;
}
