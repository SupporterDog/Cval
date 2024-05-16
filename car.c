#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdlib.h>

#define I2C_DEVICE_ADDRESS 0x16  // I2C 장치 주소

int file;

// I2C 버스 열기
void open_i2c() {
    file = wiringPiI2CSetup(I2C_DEVICE_ADDRESS);
    if (file < 0) {
        perror("Error opening i2c bus");
        return;
    }
}

// 8비트 데이터를 I2C 레지스터에 쓰기
void write_u8(uint8_t reg, uint8_t data) {
    if (wiringPiI2CWriteReg8(file, reg, data) < 0) {
        perror("Error writing to i2c device");
        return;
    }
}

// 배열 데이터를 I2C 레지스터에 쓰기
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

// 자동차 제어 명령 보내기
void ctrl_car(uint8_t l_dir, uint8_t l_speed, uint8_t r_dir, uint8_t r_speed) {
    uint8_t reg = 0x01;
    uint8_t data[4] = {l_dir, l_speed, r_dir, r_speed};
    write_array(reg, data, sizeof(data));
}

// 자동차 속도 및 방향 제어
void control_car(int speed1, int speed2) {
    uint8_t dir1 = speed1 < 0 ? 0 : 1;
    uint8_t dir2 = speed2 < 0 ? 0 : 1;
    ctrl_car(dir1, abs(speed1), dir2, abs(speed2));
}

// 자동차 전진
void car_run(int speed1, int speed2) {
    ctrl_car(1, speed1, 1, speed2);
}

// 자동차 정지
void car_stop() {
    uint8_t reg = 0x02;
    write_u8(reg, 0x00);
}

// 자동차 후진
void car_back(int speed1, int speed2) {
    ctrl_car(0, speed1, 0, speed2);
}

// 자동차 좌회전
void car_left(int speed1, int speed2) {
    ctrl_car(0, speed1, 1, speed2);
}

// 자동차 우회전
void car_right(int speed1, int speed2) {
    ctrl_car(1, speed1, 0, speed2);
}

// 자동차 좌회전 스핀
void car_spin_left(int speed1, int speed2) {
    ctrl_car(0, speed1, 1, speed2);
}

// 자동차 우회전 스핀
void car_spin_right(int speed1, int speed2) {
    ctrl_car(1, speed1, 0, speed2);
}

// 서보 모터 제어
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

    // 예시 사용법
    control_car(50, 50); // 양쪽 모터를 속도 50으로 전진
    sleep(2); // 2초 동안 전진
    car_stop(); // 자동차 정지

    close(file); // I2C 버스 닫기
    return 0;
}
