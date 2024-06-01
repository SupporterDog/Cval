#ifndef TRACKING_H
#define TRACKING_H

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
#define l_spin 1
#define straight 2
#define r_spin 3
#define turn 4

typedef struct {
    int _device;
    int _addr;
} YB_Pcb_Car;

// Function to initialize the I2C device with the given address
void get_i2c_device(YB_Pcb_Car* car, int address);

// Function to write an array of data to the specified register
void write_array(YB_Pcb_Car* car, int reg, unsigned char* data, int length);

// Function to control the car with the specified directions and speeds
void Ctrl_Car(YB_Pcb_Car* car, int l_dir, int l_speed, int r_dir, int r_speed);

// Function to move the car forward with the specified speeds
void Car_Run(YB_Pcb_Car* car, int l_speed, int r_speed);

// Function to stop the car
void Car_Stop(YB_Pcb_Car* car);

// Function to move the car backward with the specified speeds
void Car_Back(YB_Pcb_Car* car, int l_speed, int r_speed);

// Function to turn the car left with the specified speeds
void Car_Left(YB_Pcb_Car* car, int l_speed, int r_speed);

// Function to turn the car right with the specified speeds
void Car_Right(YB_Pcb_Car* car, int l_speed, int r_speed);

// Function to control the servo with the specified servo ID and angle
void Ctrl_Servo(YB_Pcb_Car* car, int servo_id, int angle);

// Function to read the state of the sensor at the given pin
int read_sensor(int pin);

// Function to handle specific turning and moving logic
void perform_car_run_and_turn(YB_Pcb_Car* car, int* sensor_state, int control);

// Function for line following logic
void line_following(YB_Pcb_Car* car);

#endif // TRACKING_H
