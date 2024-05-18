#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // POSIX 라이브러리의 write 함수 포함

#define I2C_ADDR 0x16

// 라인 트래킹 센서 핀 설정 (WiringPi 번호 체계 사용)
#define SENSOR_RIGHT1 0  // 물리적 핀 11, GPIO 17
#define SENSOR_RIGHT2 7  // 물리적 핀 7, GPIO 4
#define SENSOR_LEFT1 2   // 물리적 핀 13, GPIO 27
#define SENSOR_LEFT2 3   // 물리적 핀 15, GPIO 22

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

void write_array(YB_Pcb_Car* car, int reg, unsigned char* data, int length) {
    unsigned char buffer[length + 1];
    buffer[0] = reg;
    for (int i = 0; i < length; ++i) {
        buffer[i + 1] = data[i];
    }
    if (write(car->_device, buffer, length + 1) != length + 1) {
        printf("write_array I2C error\n");
    } else {
        printf("write_array: Data written to reg %d\n", reg);
    }
}

void Ctrl_Car(YB_Pcb_Car* car, int l_dir, int l_speed, int r_dir, int r_speed) {
    printf("Ctrl_Car called with l_dir: %d, l_speed: %d, r_dir: %d, r_speed: %d\n", l_dir, l_speed, r_dir, r_speed);
    unsigned char data[4] = { l_dir, l_speed, r_dir, r_speed };
    write_array(car, 0x01, data, 4);
}

void Car_Run(YB_Pcb_Car* car, int l_speed, int r_speed) {
    Ctrl_Car(car, 1, l_speed, 1, r_speed);
}

void Car_Stop(YB_Pcb_Car* car) {
    unsigned char data[4] = {0, 0, 0, 0};
    write_array(car, 0x01, data, 4);
}

void Car_Back(YB_Pcb_Car* car, int l_speed, int r_speed) {
    Ctrl_Car(car, 0, l_speed, 0, r_speed);
}

void Car_Left(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 0, speed, 1, speed);
}

void Car_Right(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 1, speed, 0, speed);
}

void Car_Spin_Left(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 0, speed, 1, speed);
}

void Car_Spin_Right(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 1, speed, 0, speed);
}

void Ctrl_Servo(YB_Pcb_Car* car, int servo_id, int angle) {
    unsigned char data[2] = { servo_id, angle };
    write_array(car, 0x03, data, 2);
}

int read_sensor(int pin) {
    return digitalRead(pin);
}

void line_following(YB_Pcb_Car* car) {
    int left1, left2, right1, right2;

    while (1) {
        left1 = read_sensor(SENSOR_LEFT1);
        left2 = read_sensor(SENSOR_LEFT2);
        right1 = read_sensor(SENSOR_RIGHT1);
        right2 = read_sensor(SENSOR_RIGHT2);

        if (right1 == LOW) { // 오른쪽 첫 번째 센서가 검은색 라인을 감지
            Car_Spin_Right(car, 70);
            delay(500); // 45도 회전
        } else if (right2 == LOW) { // 오른쪽 두 번째 센서가 검은색 라인을 감지
            Car_Run(car, 70, 70); // 직진
        } else if (left1 == LOW) { // 왼쪽 첫 번째 센서가 검은색 라인을 감지
            Car_Spin_Left(car, 70);
            delay(500); // 45도 회전
        } else if (left2 == LOW) { // 왼쪽 두 번째 센서가 검은색 라인을 감지
            Car_Back(car, 70, 70); // 후진
        } else if (left1 == HIGH && left2 == HIGH && right1 == HIGH && right2 == HIGH) { // 모든 센서가 흰색 라인을 감지
            Car_Spin_Right(car, 70);
            delay(1000); // 90도 회전
        } else { // 모든 센서가 흰색을 감지
            Car_Stop(car);
        }
        delay(100);
    }
}

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    pinMode(SENSOR_LEFT1, INPUT);
    pinMode(SENSOR_LEFT2, INPUT);
    pinMode(SENSOR_RIGHT1, INPUT);
    pinMode(SENSOR_RIGHT2, INPUT);

    YB_Pcb_Car car;
    get_i2c_device(&car, I2C_ADDR);

    printf("Starting line following...\n");
    line_following(&car);

    return 0;
}
