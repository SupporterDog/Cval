#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int _device;
    int _addr;
} YB_Pcb_Car;

// I2C 장치 초기화
void initialize_i2c_device(YB_Pcb_Car* car, int address) {
    car->_addr = address;
    car->_device = wiringPiI2CSetup(car->_addr);
    if (car->_device == -1) {
        printf("Failed to initialize I2C device\n");
        exit(1);
    }
}

// 8비트 데이터 쓰기
void write_u8(YB_Pcb_Car* car, int reg, int data) {
    if (wiringPiI2CWriteReg8(car->_device, reg, data) == -1) {
        printf("write_u8 I2C error\n");
    }
}

// 레지스터에 쓰기
void write_reg(YB_Pcb_Car* car, int reg) {
    if (wiringPiI2CWrite(car->_device, reg) == -1) {
        printf("write_reg I2C error\n");
    }
}

// 배열 형태의 데이터를 레지스터 주소에 연속적으로 쓰는 함수
void write_array(YB_Pcb_Car* car, int reg, int* data, int length) {
    for (int i = 0; i < length; ++i) {
        if (wiringPiI2CWriteReg8(car->_device, reg + i, data[i]) == -1) {
            printf("write_array I2C error\n");
            return;
        }
    }
}

// 모터 제어
void control_motors(YB_Pcb_Car* car, int* speeds) {
    int reg = 0x01;
    write_array(car, reg, speeds, 4);
}

// 주행 시작
void start_driving(YB_Pcb_Car* car, int* speeds) {
    control_motors(car, speeds);
}

// 주행 종료
void stop_driving(YB_Pcb_Car* car) {
    int reg = 0x02;
    write_u8(car, reg, 0x00);
}

int main() {
    wiringPiSetup();
    YB_Pcb_Car car;
    initialize_i2c_device(&car, 0x16);

    int speeds[4] = { 100, 100, 100, 100 };

    start_driving(&car, speeds);
    delay(2000); // 2초 동안 주행
    stop_driving(&car);

    return 0;
}

