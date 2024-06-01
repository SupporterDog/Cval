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



void get_i2c_device(YB_Pcb_Car* car, int address);

void write_array(YB_Pcb_Car* car, int reg, unsigned char* data, int length);

void Ctrl_Car(YB_Pcb_Car* car, int l_dir, int l_speed, int r_dir, int r_speed);

void Car_Run(YB_Pcb_Car* car, int l_speed, int r_speed);

void Car_Stop(YB_Pcb_Car* car);

void Car_Left(YB_Pcb_Car* car, int l_speed, int r_speed);

void Car_Right(YB_Pcb_Car* car, int l_speed, int r_speed);

void Ctrl_Servo(YB_Pcb_Car* car, int servo_id, int angle) ;

int read_sensor(int pin);

void perform_car_run_and_turn(YB_Pcb_Car* car, int* sensor_state, int control);

void line_following(YB_Pcb_Car* car);