## Motor単体試験
#include "mbed.h"
DigitalOut STBY(D13);
DigitalOut AIN1(A5);
DigitalOut AIN2(A6);
DigitalOut BIN1(A2);
DigitalOut BIN2(A1);
void motorForward(void);
void motorStop(void);
void motorRight(void);
void motorLeft(void);
int main() {
    STBY = 1;
    while(1) {
        motorForward();
        wait(5);
        motorStop();
        wait(1);
        motorRight();
        wait(5);
        motorStop();
        wait(1);
        motorForward();
        wait(5);
        motorStop();
        wait(1);
        motorLeft();
        wait(5);
        motorStop();
        wait(1);
    }
}
void motorForward() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 1;
    BIN2 = 0;
}
void motorRight() {
    motorStop();
    AIN1 = 0;
    AIN2 = 1;
    BIN1 = 1;
    BIN2 = 0;
}
void motorLeft() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 1;
}
void motorStop() {
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}