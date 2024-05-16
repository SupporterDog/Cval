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
}

void write_u8(YB_Pcb_Car* car, int reg, int data) {
    if (wiringPiI2CWriteReg8(car->_device, reg, data) == -1) {
        printf("write_u8 I2C error\n");
    }
}

void write_reg(YB_Pcb_Car* car, int reg) {
    if (wiringPiI2CWrite(car->_device, reg) == -1) {
        printf("write_reg I2C error\n");
    }
}

void write_array(YB_Pcb_Car* car, int reg, int* data, int length) { //배열 형태의 데이터를 레지스터 주소에 연속적으로 쓰는 함수
    for (int i = 0; i < length; ++i) {
        if (wiringPiI2CWriteReg8(car->_device, reg + i, data[i]) == -1) {
            printf("write_array I2C error\n");
            return;
        }
    }
}

void Ctrl_Car(YB_Pcb_Car* car, int* dir, int* speed) {
    int reg = 0x01;
    int data[8] = { dir[0], speed[0], dir[1], speed[1], dir[2], speed[2], dir[3], speed[3] };
    write_array(car, reg, data, 8);
}

void Control_Car(YB_Pcb_Car* car, int* speed) {
    int dir[4];
    for (int i = 0; i < 4; ++i) {
        dir[i] = speed[i] < 0 ? 0 : 1;
    }
    Ctrl_Car(car, dir, speed);
}

void Car_Run(YB_Pcb_Car* car, int* speed) {
    Control_Car(car, speed);
}

void Car_Stop(YB_Pcb_Car* car) {
    int reg = 0x02;
    write_u8(car, reg, 0x00);
}

void Car_Back(YB_Pcb_Car* car, int* speed) {
    int dir[4];
    for (int i = 0; i < 4; ++i) {
        dir[i] = 0;
    }
    Ctrl_Car(car, dir, speed);
}

void Car_Left(YB_Pcb_Car* car, int* speed) {
    int dir[4] = { 0, 1, 1, 0 };
    Ctrl_Car(car, dir, speed);
}

void Car_Right(YB_Pcb_Car* car, int* speed) {
    int dir[4] = { 1, 0, 0, 1 };
    Ctrl_Car(car, dir, speed);
}

void Car_Spin_Left(YB_Pcb_Car* car, int* speed) {
    int dir[4] = { 0, 1, 0, 1 };
    Ctrl_Car(car, dir, speed);
}

void Car_Spin_Right(YB_Pcb_Car* car, int* speed) {
    int dir[4] = { 1, 0, 1, 0 };
    Ctrl_Car(car, dir, speed);
}

void Ctrl_Servo(YB_Pcb_Car* car, int* angle) {
    int reg = 0x03;
    write_array(car, reg, angle, 4);
}

int main() {
    wiringPiSetup();
    YB_Pcb_Car car;
    get_i2c_device(&car, 0x16, 1);

    int speed[4] = { 100, 100, 100, 100 };

    Car_Run(&car, speed);
    delay(2000); // 2초 동안 주행
    Car_Stop(&car);

    return 0;
}
