#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    int _device;
    int _addr;
} YB_Pcb_Car;

void get_i2c_device(YB_Pcb_Car* car, int address, int i2c_bus) {
    car->_addr = address;
    car->_device = wiringPiI2CSetup(car->_addr); // 주어진 I2C 주소로 I2C 장치 초기화
    if (car->_device == -1) {
        printf("Failed to initialize I2C device\n");
        exit(1);
    }
    printf("I2C device initialized at address 0x%X\n", car->_addr);
}

void write_u8(YB_Pcb_Car* car, int data) {
    if (wiringPiI2CWrite(car->_device, data) == -1) {
        printf("write_u8 I2C error\n");
    } else {
        printf("write_u8: Data %d written successfully\n", data);
    }
}

void write_reg(YB_Pcb_Car* car, int reg) {
    if (wiringPiI2CWrite(car->_device, reg) == -1) {
        printf("write_reg I2C error\n");
    } else {
        printf("write_reg: Register %d written successfully\n", reg);
    }
}

void write_array(YB_Pcb_Car* car, int reg, int* data, int length) {
    for (int i = 0; i < length; ++i) {
        if (wiringPiI2CWriteReg8(car->_device, reg + i, data[i]) == -1) {
            printf("write_array I2C error at index %d\n", i);
            return;
        } else {
            printf("write_array: Data %d written to reg %d\n", data[i], reg + i);
        }
    }
}

void Ctrl_Car(YB_Pcb_Car* car, int* speed) {
    int reg = 0x01;
    write_array(car, reg, speed, 4);
}

void Car_Run(YB_Pcb_Car* car, int* speed) {
    Ctrl_Car(car, speed);
}

void Car_Stop(YB_Pcb_Car* car) {
    int speed[4] = {0, 0, 0, 0};
    Ctrl_Car(car, speed);
}

void Car_Left(YB_Pcb_Car* car, int* speed) {
    int adjusted_speed[4] = {speed[0], speed[1], -speed[2], -speed[3]};
    Ctrl_Car(car, adjusted_speed);
}

void Car_Right(YB_Pcb_Car* car, int* speed) {
    int adjusted_speed[4] = {-speed[0], -speed[1], speed[2], speed[3]};
    Ctrl_Car(car, adjusted_speed);
}

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    YB_Pcb_Car car;
    get_i2c_device(&car, 0x16, 1);

    int speed[4] = { 100, 100, 100, 100 };

    printf("Running car...\n");
    Car_Run(&car, speed);
    delay(2000); // 2초 동안 주행
    printf("Stopping car...\n");
    Car_Stop(&car);

    return 0;
}
