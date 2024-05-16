#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdlib.h>

#define I2C_DEVICE_ADDRESS 0x16

int file;

void open_i2c() {
    file = wiringPiI2CSetup(I2C_DEVICE_ADDRESS);
    if (file < 0) {
        perror("Error opening i2c bus");
        return;
    }
}

void write_u8(uint8_t reg, uint8_t data) {
    if (wiringPiI2CWriteReg8(file, reg, data) < 0) {
        perror("Error writing to i2c device");
        return;
    }
}

void write_array(uint8_t reg, uint8_t *data, int len) {
    if (wiringPiI2CWriteReg8(file, reg, data[0]) < 0) {
        perror("Error writing to i2c device");
        return;
    }
    if (write(file, data + 1, len - 1) != (len - 1)) {
        perror("Error writing to i2c device");
        return;
    }
}

void ctrl_car(uint8_t l_dir, uint8_t l_speed, uint8_t r_dir, uint8_t r_speed) {
    uint8_t reg = 0x01;
    uint8_t data[4] = {l_dir, l_speed, r_dir, r_speed};
    write_array(reg, data, sizeof(data));
}

void control_car(int speed1, int speed2) {
    uint8_t dir1 = speed1 < 0 ? 0 : 1;
    uint8_t dir2 = speed2 < 0 ? 0 : 1;
    ctrl_car(dir1, abs(speed1), dir2, abs(speed2));
}

void car_run(int speed1, int speed2) {
    ctrl_car(1, speed1, 1, speed2);
}

void car_stop() {
    uint8_t reg = 0x02;
    write_u8(reg, 0x00);
}

void car_back(int speed1, int speed2) {
    ctrl_car(0, speed1, 0, speed2);
}

void car_left(int speed1, int speed2) {
    ctrl_car(0, speed1, 1, speed2);
}

void car_right(int speed1, int speed2) {
    ctrl_car(1, speed1, 0, speed2);
}

void car_spin_left(int speed1, int speed2) {
    ctrl_car(0, speed1, 1, speed2);
}

void car_spin_right(int speed1, int speed2) {
    ctrl_car(1, speed1, 0, speed2);
}

void ctrl_servo(uint8_t id, int angle) {
    uint8_t reg = 0x03;
    if (angle < 0) {
        angle = 0;
    } else if (angle > 180) {
        angle = 180;
    }
    uint8_t data[2] = {id, angle};
    write_array(reg, data, sizeof(data));
}

int main() {
    open_i2c();

    // Example usage
    control_car(50, 50); // Move forward with both motors at speed 50
    sleep(2); // Keep moving for 2 seconds
    car_stop(); // Stop the car

    close(file);
    return 0;
}
