#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

#define I2C_ADDR 0x16

#define NUM_READINGS 10
#define SENSOR_RIGHT2 0  
#define SENSOR_RIGHT1 7  
#define SENSOR_LEFT1 2   
#define SENSOR_LEFT2 3   

typedef struct {
    int _device;
    int _addr;
} YB_Pcb_Car;

typedef struct {
    double Kp;
    double Ki;
    double Kd;
    double integral;
    double previous_error;
} PID;

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
        printf("write_array: Data written to reg %d\n", reg);
    }
}

void Ctrl_Car(YB_Pcb_Car* car, int l_dir, int l_speed, int r_dir, int r_speed) {
    unsigned char data[4] = { l_dir, l_speed, r_dir, r_speed };
    write_array(car, 0x01, data, 4);
}

void Car_Run(YB_Pcb_Car* car, int base_speed, int correction) {
    int left_speed = base_speed - correction;
    int right_speed = base_speed + correction;
    Ctrl_Car(car, 1, left_speed, 1, right_speed);
}

void Car_Stop(YB_Pcb_Car* car) {
    unsigned char data[4] = {0, 0, 0, 0};
    write_array(car, 0x01, data, 4);
}

void Car_Back(YB_Pcb_Car* car, int l_speed, int r_speed) {
    Ctrl_Car(car, 0, l_speed, 0, r_speed);
}

void Car_Left(YB_Pcb_Car* car, int l_speed, int r_speed) {
    Ctrl_Car(car, 0, l_speed, 1, r_speed);
}

void Car_Right(YB_Pcb_Car* car, int l_speed, int r_speed) {
    Ctrl_Car(car, 1, l_speed, 0, r_speed);
}

void Ctrl_Servo(YB_Pcb_Car* car, int servo_id, int angle) {
    unsigned char data[2] = { servo_id, angle };
    write_array(car, 0x03, data, 2);
}

int read_sensor(int pin) {
    return digitalRead(pin);
}

void PID_Init(PID* pid, double Kp, double Ki, double Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0;
    pid->previous_error = 0.0;
}

double PID_Compute(PID* pid, double setpoint, double measured_value) {
    double error = setpoint - measured_value;
    pid->integral += error;
    double derivative = error - pid->previous_error;
    double output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    pid->previous_error = error;
    return output;
}

void line_following(YB_Pcb_Car* car) {
    PID pid;
    PID_Init(&pid, 2.0, 0.0, 1.0);  // 튜닝된 PID 파라미터 설정

    int base_speed = 70;  // 기본 속도 증가
    int left1, left2, right1, right2;
    int error, correction;
    
    while (1) {
        left1 = read_sensor(SENSOR_LEFT1);
        left2 = read_sensor(SENSOR_LEFT2);
        right1 = read_sensor(SENSOR_RIGHT1);
        right2 = read_sensor(SENSOR_RIGHT2);

        // 센서 상태를 기반으로 오차 계산 (왼쪽 센서는 음수, 오른쪽 센서는 양수)
        error = (left1 * -2 + left2 * -1 + right1 * 1 + right2 * 2);

        // PID 제어를 통해 보정 값 계산
        correction = (int)PID_Compute(&pid, 0, error);

        // 모터 속도 조절
        Car_Run(car, base_speed, correction);

        delay(10);  // 짧은 지연 시간
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
