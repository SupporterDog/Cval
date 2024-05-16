#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <stdlib.h>

#define I2C_DEVICE_ADDRESS 0x16  // I2C 장치 주소

int file;

// I2C 버스 열기
void open_i2c() {
    file = wiringPiI2CSetup(I2C_DEVICE_ADDRESS);
    if (file < 0) {
        perror("Error opening I2C bus");
        exit(1);  // 오류 발생 시 프로그램 종료
    }
}

// 8비트 데이터를 I2C 레지스터에 쓰기
void write_u8(uint8_t reg, uint8_t data) {
    if (wiringPiI2CWriteReg8(file, reg, data) < 0) {
        perror("Error writing to I2C device");
    }
}

// 모터 설정 함수 (모터 번호: 1~4, 방향: 0(후진), 1(전진), 속도: 0~255)
void set_motor(uint8_t motor_num, uint8_t direction, uint8_t speed) {
    uint8_t dir_reg = 0x01 + (motor_num - 1) * 2;  // 모터 방향 레지스터
    uint8_t speed_reg = 0x02 + (motor_num - 1) * 2; // 모터 속도 레지스터

    write_u8(dir_reg, direction);
    write_u8(speed_reg, speed);
}

// 자동차 제어 함수
void control_car(uint8_t m1_speed, uint8_t m1_dir, 
                 uint8_t m2_speed, uint8_t m2_dir, 
                 uint8_t m3_speed, uint8_t m3_dir, 
                 uint8_t m4_speed, uint8_t m4_dir) {
    set_motor(1, m1_dir, m1_speed);
    set_motor(2, m2_dir, m2_speed);
    set_motor(3, m3_dir, m3_speed);
    set_motor(4, m4_dir, m4_speed);
}

// 자동차 정지 함수
void car_stop() {
    control_car(0, 1, 0, 1, 0, 1, 0, 1);  // 모든 모터를 속도 0으로 설정
}

// 자동차 전진
void car_run(uint8_t speed) {
    control_car(speed, 1, speed, 1, speed, 1, speed, 1);  // 모든 모터 전진
}

// 자동차 후진
void car_back(uint8_t speed) {
    control_car(speed, 0, speed, 0, speed, 0, speed, 0);  // 모든 모터 후진
}

// 자동차 좌회전
void car_left(uint8_t speed) {
    control_car(0, 0, speed, 1, 0, 0, speed, 1);  // 왼쪽 모터 후진, 오른쪽 모터 전진
}

// 자동차 우회전
void car_right(uint8_t speed) {
    control_car(speed, 1, 0, 0, speed, 1, 0, 0);  // 왼쪽 모터 전진, 오른쪽 모터 후진
}

// 자동차 좌회전 스핀
void car_spin_left(uint8_t speed) {
    control_car(0, 0, speed, 1, 0, 0, speed, 1);  // 왼쪽 모터 후진, 오른쪽 모터 전진
}

// 자동차 우회전 스핀
void car_spin_right(uint8_t speed) {
    control_car(speed, 1, 0, 0, speed, 1, 0, 0);  // 왼쪽 모터 전진, 오른쪽 모터 후진
}

int main() {
    open_i2c();

    // 예시 사용법: 모든 모터를 속도 50으로 전진
    car_run(50);
    sleep(2);  // 2초 동안 전진

    // 예시 사용법: 자동차 정지
    car_stop();
    sleep(1);  // 1초 동안 정지

    // 예시 사용법: 모든 모터를 속도 50으로 후진
    car_back(50);
    sleep(2);  // 2초 동안 후진

    // 예시 사용법: 자동차 정지
    car_stop();

    close(file);  // I2C 버스 닫기
    return 0;
}
