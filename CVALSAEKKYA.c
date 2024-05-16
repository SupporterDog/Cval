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
    car->_device = wiringPiI2CSetup(car->_addr);
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

void write_array(YB_Pcb_Car* car, int reg, int* data, int length) {
    for (int i = 0; i < length; ++i) {
        if (wiringPiI2CWriteReg8(car->_device, reg + i, data[i]) == -1) {
            printf("write_array I2C error\n");
            return;
        }
    }
}

void Ctrl_Car(YB_Pcb_Car* car, int l_dir, int l_speed, int r_dir, int r_speed) {
    int reg = 0x01;
    int data[4] = { l_dir, l_speed, r_dir, r_speed };
    write_array(car, reg, data, 4);
}

void Control_Car(YB_Pcb_Car* car, int speed1, int speed2) {
    int dir1 = speed1 < 0 ? 0 : 1;
    int dir2 = speed2 < 0 ? 0 : 1;
    Ctrl_Car(car, dir1, abs(speed1), dir2, abs(speed2));
}

void Car_Run(YB_Pcb_Car* car, int speed1, int speed2) {
    Ctrl_Car(car, 1, speed1, 1, speed2);
}

void Car_Stop(YB_Pcb_Car* car) {
    int reg = 0x02;
    write_u8(car, reg, 0x00);
}

void Car_Back(YB_Pcb_Car* car, int speed1, int speed2) {
    Ctrl_Car(car, 0, speed1, 0, speed2);
}

void Car_Left(YB_Pcb_Car* car, int speed1, int speed2) {
    Ctrl_Car(car, 0, speed1, 1, speed2);
}

void Car_Right(YB_Pcb_Car* car, int speed1, int speed2) {
    Ctrl_Car(car, 1, speed1, 0, speed2);
}

void Car_Spin_Left(YB_Pcb_Car* car, int speed1, int speed2) {
    Ctrl_Car(car, 0, speed1, 1, speed2);
}

void Car_Spin_Right(YB_Pcb_Car* car, int speed1, int speed2) {
    Ctrl_Car(car, 1, speed1, 0, speed2);
}

void Ctrl_Servo(YB_Pcb_Car* car, int id, int angle) {
    if (angle < 0) {
        angle = 0;
    } else if (angle > 180) {
        angle = 180;
    }
    int reg = 0x03;
    int data[2] = { id, angle };
    write_array(car, reg, data, 2);
}

int main() {
    wiringPiSetup();
    YB_Pcb_Car car;
    get_i2c_device(&car, 0x16, 1);

    Car_Run(&car, 100, 100);
    delay(2000); // 2초 동안 주행
    Car_Stop(&car);

    return 0;
}
