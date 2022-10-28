// Motor単体試験
#include "mbed.h"

//これ以下に記入
speedL=0.5; speedR=0.8;

DigitalOut STBY(D13);
PwmOut AIN1(A5);
PwmOut AIN2(A6);
PwmOut BIN1(A2);
PwmOut BIN2(A1);

void motorForward(void);
void motorStop(void);
void motorTurnR(void);

int main() {
    STBY = 1;
    while(1) {
        motorForward();
        wait(50);
        motorStop();
        wait(5);
        motorTurnR();
        wait(10);
        motorStop();
        wait(5);
    }
}
void motorForward(void) {
    AIN1 = speedL;
    AIN2 = 0;
    BIN1 = speedR;
    BIN2 = 0;
}
    
void motorTurnR(void) {
    AIN1 = speedL;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = speedR;
}

void motorStop(void) {
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}