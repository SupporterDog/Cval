#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>

// Define GPIO pins for the motor control
#define LEFT_MOTOR_DIR_PIN 0   // GPIO pin for left motor direction
#define LEFT_MOTOR_SPEED_PIN 1 // GPIO pin for left motor speed (PWM)
#define RIGHT_MOTOR_DIR_PIN 2  // GPIO pin for right motor direction
#define RIGHT_MOTOR_SPEED_PIN 3 // GPIO pin for right motor speed (PWM)

// Define GPIO pins for servo control
#define SERVO1_PIN 4
#define SERVO2_PIN 5
#define SERVO3_PIN 6
#define SERVO4_PIN 7

// Initialize the GPIO pins
void initGPIO() {
    wiringPiSetup();
    
    // Initialize motor control pins
    pinMode(LEFT_MOTOR_DIR_PIN, OUTPUT);
    softPwmCreate(LEFT_MOTOR_SPEED_PIN, 0, 180);
    pinMode(RIGHT_MOTOR_DIR_PIN, OUTPUT);
    softPwmCreate(RIGHT_MOTOR_SPEED_PIN, 0, 180);
    
    // Initialize servo control pins
    softPwmCreate(SERVO1_PIN, 0, 180);
    softPwmCreate(SERVO2_PIN, 0, 180);
    softPwmCreate(SERVO3_PIN, 0, 180);
    softPwmCreate(SERVO4_PIN, 0, 180);
}

// Control the movement of the car
void controlMovement(int leftDir, int leftSpeed, int rightDir, int rightSpeed) {
    digitalWrite(LEFT_MOTOR_DIR_PIN, leftDir);
    softPwmWrite(LEFT_MOTOR_SPEED_PIN, leftSpeed);
    digitalWrite(RIGHT_MOTOR_DIR_PIN, rightDir);
    softPwmWrite(RIGHT_MOTOR_SPEED_PIN, rightSpeed);
}

// Stop all motors
void stopAllMotors() {
    softPwmWrite(LEFT_MOTOR_SPEED_PIN, 0);
    softPwmWrite(RIGHT_MOTOR_SPEED_PIN, 0);
}

// Control the servo
void controlServo(int servo, int angle) {
    switch (servo) {
        case 1:
            softPwmWrite(SERVO1_PIN, angle);
            break;
        case 2:
            softPwmWrite(SERVO2_PIN, angle);
            break;
        case 3:
            softPwmWrite(SERVO3_PIN, angle);
            break;
        case 4:
            softPwmWrite(SERVO4_PIN, angle);
            break;
        default:
            printf("Invalid servo number\n");
            break;
    }
}

int main() {
    // Initialize GPIO
    initGPIO();
    
    // Example usage
    // Control movement: Left motor forward at speed 100, Right motor backward at speed 150
    controlMovement(1, 100, 0, 150);
    delay(5000); // Run for 5 seconds
    
    // Stop all motors
    stopAllMotors();
    delay(2000); // Wait for 2 seconds
    
    // Control servo 1 to angle 90
    controlServo(1, 90);
    delay(2000); // Wait for 2 seconds
    
    return 0;
}
