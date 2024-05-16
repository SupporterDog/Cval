#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define I2C_ADDR 0x16

int i2c_fd;

void write_u8(int reg, int data) {
    if (wiringPiI2CWriteReg8(i2c_fd, reg, data) < 0) {
        perror("write_u8 I2C error");
    }
}

void write_reg(int reg) {
    if (wiringPiI2CWrite(i2c_fd, reg) < 0) {
        perror("write_reg I2C error");
    }
}

void write_array(int reg, unsigned char* data, int length) {
    for (int i = 0; i < length; i++) {
        if (wiringPiI2CWriteReg8(i2c_fd, reg + i, data[i]) < 0) {
            perror("write_array I2C error");
            break;
        }
    }
}

void Ctrl_Car(int l_dir, int l_speed, int r_dir, int r_speed) {
    unsigned char data[4] = { l_dir, l_speed, r_dir, r_speed };
    write_array(0x01, data, 4);
}

void Control_Car(int speed1, int speed2) {
    int dir1 = (speed1 < 0) ? 0 : 1;
    int dir2 = (speed2 < 0) ? 0 : 1;
    Ctrl_Car(dir1, abs(speed1), dir2, abs(speed2));
}

void Car_Run(int speed1, int speed2) {
    Ctrl_Car(1, speed1, 1, speed2);
}

void Car_Stop() {
    write_u8(0x02, 0x00);
}

void Car_Back(int speed1, int speed2) {
    Ctrl_Car(0, speed1, 0, speed2);
}

void Car_Left(int speed1, int speed2) {
    Ctrl_Car(0, speed1, 1, speed2);
}

void Car_Right(int speed1, int speed2) {
    Ctrl_Car(1, speed1, 0, speed2);
}

void Car_Spin_Left(int speed1, int speed2) {
    Ctrl_Car(0, speed1, 1, speed2);
}

void Car_Spin_Right(int speed1, int speed2) {
    Ctrl_Car(1, speed1, 0, speed2);
}

void Ctrl_Servo(int id, int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    unsigned char data[2] = { id, angle };
    write_array(0x03, data, 2);
}

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }
    
    i2c_fd = wiringPiI2CSetup(I2C_ADDR);
    if (i2c_fd == -1) {
        printf("Failed to initiate I2C communication\n");
        return 1;
    }

    // 테스트 코드
    printf("Car Run...\n");
    Car_Run(100, 100);
    delay(2000);

    printf("Car Stop...\n");
    Car_Stop();
    delay(2000);

    printf("Car Back...\n");
    Car_Back(100, 100);
    delay(2000);

    printf("Car Stop...\n");
    Car_Stop();
    delay(2000);

    printf("Car Left...\n");
    Car_Left(100, 100);
    delay(2000);

    printf("Car Stop...\n");
    Car_Stop();
    delay(2000);

    printf("Car Right...\n");
    Car_Right(100, 100);
    delay(2000);

    printf("Car Stop...\n");
    Car_Stop();
    delay(2000);

    printf("Car Spin Left...\n");
    Car_Spin_Left(100, 100);
    delay(2000);

    printf("Car Stop...\n");
    Car_Stop();
    delay(2000);

    printf("Car Spin Right...\n");
    Car_Spin_Right(100, 100);
    delay(2000);

    printf("Car Stop...\n");
    Car_Stop();
    delay(2000);

    printf("Servo Control...\n");
    Ctrl_Servo(1, 90);
    delay(1000);
    Ctrl_Servo(1, 0);
    delay(1000);
    Ctrl_Servo(1, 180);
    delay(1000);

    printf("All tests completed.\n");

    return 0;
}
