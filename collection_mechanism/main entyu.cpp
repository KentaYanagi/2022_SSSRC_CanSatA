#include "mbed.h"
PwmOut pinAFin(D9);
PwmOut pinARin(D10);
DigitalOut STBY(D11);
PwmOut servo(D6);



//関数設定
void driveMoter(float speedA){
    float outputA = abs(speedA);
    if(speedA > 0){
        pinAFin=outputA;
        pinARin=0;
    }else if (speedA < 0){
        pinAFin=0;
        pinARin=outputA;
    }else{
        pinAFin=0;
        pinARin=0;
    }
}


int main(){
    STBY = 1;
    servo.period_ms(20);
    int a;
    wait(5);
    for(a=500; a<=2400; a=a+210){
        servo.pulsewidth_us(a);
        driveMoter(0);
        wait(3);
        driveMoter(0.2);
        wait(1.2);
        driveMoter(0);
        wait(3);
        driveMoter(-0.2);
        wait(1.2);
        driveMoter(0);
        wait(5);
        }
}

