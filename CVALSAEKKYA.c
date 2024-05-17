#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int _device;
    int _addr;
} YB_Pcb_Car;

void get_i2c_device(YB_Pcb_Car* car, int address) {
    car->_addr = address;
    car->_device = wiringPiI2CSetup(car->_addr); // 주어진 I2C 주소로 I2C 장치 초기화
    if (car->_device == -1) {
        printf("Failed to initialize I2C device\n");
        exit(1);
    }
    printf("I2C device initialized at address 0x%X\n", car->_addr);
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

void Ctrl_Car(YB_Pcb_Car* car, int* data) {
    int reg = 0x01;
    write_array(car, reg, data, 4);
}

void Car_Run(YB_Pcb_Car* car, int* data) {
    Ctrl_Car(car, data);
}

void Car_Stop(YB_Pcb_Car* car) {
    int reg = 0x02;
    int stop_data = 0x00;
    if (wiringPiI2CWriteReg8(car->_device, reg, stop_data) == -1) {
        printf("Car_Stop I2C error\n");
    } else {
        printf("Car stopped.\n");
    }
}

void Car_Left(YB_Pcb_Car* car, int* data) {
    int adjusted_data[4] = {data[0], data[1], !data[2], data[3]};
    Ctrl_Car(car, adjusted_data);
}

void Car_Right(YB_Pcb_Car* car, int* data) {
    int adjusted_data[4] = {!data[0], data[1], data[2], data[3]};
    Ctrl_Car(car, adjusted_data);
}

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    YB_Pcb_Car car;
    get_i2c_device(&car, 0x16);

    int run_data[4] = {1, 100, 1, 100}; // 앞 방향으로 100의 속도로 주행
    printf("Running car...\n");
    Car_Run(&car, run_data);
    delay(2000); // 2초 동안 주행
    printf("Stopping car...\n");
    Car_Stop(&car);

    return 0;
}
