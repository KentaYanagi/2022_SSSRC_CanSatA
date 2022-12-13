#include "mbed.h"
DigitalOut pin(D2);

DigitalOut STBY(D13);
PwmOut AIN1(A5);
PwmOut AIN2(A6);
PwmOut BIN1(A2);
PwmOut BIN2(A1);

void motorStop();
void motorForward();

int main(){
    STBY = 1;
    printf("Start\n");
    pin = 1;
    
    for(int i=0;i<5;i++){
        wait(1);
        printf("%d second\n", i+1);    
    }

    printf("%s\r\n", "走行開始");
    motorForward();
    for(int i=0;i<5;i++){
        wait(1);
        printf("%d秒目\r\n", i+1);
    }
    
    motorStop();
    pin = 0;
    printf("End\n");
}

void motorForward() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 1;
    BIN2 = 0;
}

void motorStop() {
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}
