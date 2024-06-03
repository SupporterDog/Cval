#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>       // wiringPi 라이브러리 포함
#include <wiringPiI2C.h>    // wiringPi I2C 라이브러리 포함

#define I2C_ADDR 0x16

typedef struct {
    int _device;
    int _addr;
} YB_Pcb_Car;

void get_i2c_device(YB_Pcb_Car* car, int address) {
    car->_addr = address;
    car->_device = wiringPiI2CSetup(car->_addr);
    if (car->_device == -1) {
        printf("Failed to initialize I2C device\n");
        exit(1);
    }
    printf("I2C device initialized at address 0x%X\n", car->_addr);
}

void write_array(YB_Pcb_Car* car, int reg, unsigned char* data, int length) {
    unsigned char buffer[length + 1];
    buffer[0] = reg;
    for (int i = 0; i < length; ++i) {
        buffer[i + 1] = data[i];
    }
    if (write(car->_device, buffer, length + 1) != length + 1) {
        printf("write_array I2C error\n");
    } else {
        //printf("write_array: Data written to reg %d\n", reg);
    }
}

void Ctrl_Servo(YB_Pcb_Car* car, int servo_id, int angle) {
    printf("Ctrl_Servo called with servo_id: %d, angle: %d\n", servo_id, angle);
    unsigned char data[2] = { servo_id, angle };
    write_array(car, 0x03, data, 2);
}

int main() {
    wiringPiSetup();  // wiringPi 초기화
    YB_Pcb_Car car;
    get_i2c_device(&car, I2C_ADDR);

    int servo_id = 1;  // 제어할 서보 모터의 ID
    while (1) {
        Ctrl_Servo(&car, servo_id, 60); // 왼쪽 (30도 기준)
        delay(1000); // 1000ms 대기
        Ctrl_Servo(&car, servo_id, 90); // 앞쪽 (0도 기준)
        delay(1000); // 1000ms 대기
        Ctrl_Servo(&car, servo_id, 120); // 오른쪽 (-30도 기준)
        delay(1000); // 1000ms 대기
    }

    return 0;
}
