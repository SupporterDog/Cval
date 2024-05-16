#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Device address
int fd;
const int deviceAddress = 0x16;

// GPIO pin definitions (update these based on your specific hardware setup)
const int LEFT_SENSOR_PIN = 1;   // Example GPIO pins
const int RIGHT_SENSOR_PIN = 2;

// Function declarations
void setupI2C();
void writeByte(int reg, int data);
void writeArray(int reg, const int *data, int length);
void ctrlCar(int l_dir, int l_speed, int r_dir, int r_speed);
void controlCar(int speed1, int speed2);
void carRun(int speed1, int speed2);
void carStop();
void carBack(int speed1, int speed2);
void carLeft(int speed1, int speed2);
void carRight(int speed1, int speed2);
void carSpinLeft(int speed1, int speed2);
void carSpinRight(int speed1, int speed2);
void ctrlServo(int id, int angle);

int main() {
    setupI2C();

    // Example usage: Run the car forward at speed levels 50 for both motors
    carRun(50, 50);
    delay(2000); // Run for 2 seconds

    // Stop the car
    carStop();

    return 0;
}

void setupI2C() {
    fd = wiringPiI2CSetup(deviceAddress);
    if (fd == -1) {
        fprintf(stderr, "Failed to init I2C communication.\n");
        exit(1);
    }
    printf("I2C communication successfully setup.\n");
}

void writeByte(int reg, int data) {
    if (wiringPiI2CWriteReg8(fd, reg, data) == -1) {
        fprintf(stderr, "Failed to write byte to I2C device.\n");
    }
}

void writeArray(int reg, const int *data, int length) {
    for (int i = 0; i < length; i++) {
        writeByte(reg, data[i]);
    }
}

void ctrlCar(int l_dir, int l_speed, int r_dir, int r_speed) {
    int data[4] = {l_dir, l_speed, r_dir, r_speed};
    writeArray(0x01, data, 4);
}

void controlCar(int speed1, int speed2) {
    int dir1 = speed1 >= 0 ? 1 : 0;
    int dir2 = speed2 >= 0 ? 1 : 0;
    ctrlCar(dir1, abs(speed1), dir2, abs(speed2));
}

void carRun(int speed1, int speed2) {
    controlCar(speed1, speed2);
}

void carStop() {
    writeByte(0x02, 0x00);
}

void carBack(int speed1, int speed2) {
    controlCar(-speed1, -speed2);
}

void carLeft(int speed1, int speed2) {
    controlCar(-speed1, speed2);
}

void carRight(int speed1, int speed2) {
    controlCar(speed1, -speed2);
}

void carSpinLeft(int speed1, int speed2) {
    controlCar(-speed1, speed2);
}

void carSpinRight(int speed1, int speed2) {
    controlCar(speed1, -speed2);
}

void ctrlServo(int id, int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    int data[2] = {id, angle};
    writeArray(0x03, data, 2);
}
