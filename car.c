#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>

#define I2C_ADDR 0x16

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
    // Create a buffer that includes the register address followed by the data
    unsigned char buffer[length + 1];
    buffer[0] = reg;
    for (int i = 0; i < length; ++i) {
        buffer[i + 1] = data[i];
    }
    
    // Write the buffer to the I2C device
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
    printf("Car_Run called with l_speed: %d, r_speed: %d\n", l_speed, r_speed);
    Ctrl_Car(car, 1, l_speed, 1, r_speed);
}

void Car_Stop(YB_Pcb_Car* car) {
    printf("Car_Stop called\n");
    unsigned char data[4] = {0, 0, 0, 0};
    write_array(car, 0x01, data, 4);
}

void Car_Back(YB_Pcb_Car* car, int l_speed, int r_speed) {
    printf("Car_Back called with l_speed: %d, r_speed: %d\n", l_speed, r_speed);
    Ctrl_Car(car, 0, l_speed, 0, r_speed);
}

void Car_Left(YB_Pcb_Car* car, int speed) {
    printf("Car_Left called with speed: %d\n", speed);
    Ctrl_Car(car, 0, speed, 1, speed);
}

void Car_Right(YB_Pcb_Car* car, int speed) {
    printf("Car_Right called with speed: %d\n", speed);
    Ctrl_Car(car, 1, speed, 0, speed);
}

void Ctrl_Servo(YB_Pcb_Car* car, int servo_id, int angle) {
    printf("Ctrl_Servo called with servo_id: %d, angle: %d\n", servo_id, angle);
    unsigned char data[2] = { servo_id, angle };
    write_array(car, 0x03, data, 2);
}

int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    YB_Pcb_Car car;
    get_i2c_device(&car, I2C_ADDR);

    printf("Running car forward...\n");
    Car_Run(&car, 100, 100);
    delay(2000); // 2초 동안 주행

    printf("Stopping car...\n");
    Car_Stop(&car);
    delay(2000);

    printf("Running car backward...\n");
    Car_Back(&car, 100, 100);
    delay(2000);

    printf("Stopping car...\n");
    Car_Stop(&car);
    delay(2000);

    printf("Turning car left...\n");
    Car_Left(&car, 100);
    delay(2000);

    printf("Stopping car...\n");
    Car_Stop(&car);
    delay(2000);

    printf("Turning car right...\n");
    Car_Right(&car, 100);
    delay(2000);

    printf("Stopping car...\n");
    Car_Stop(&car);
    delay(2000);

    printf("Controlling servo...\n");
    Ctrl_Servo(&car, 1, 90);
    delay(1000);
    Ctrl_Servo(&car, 1, 0);
    delay(1000);
    Ctrl_Servo(&car, 1, 180);
    delay(1000);

    printf("All tests completed.\n");

    return 0;
}
