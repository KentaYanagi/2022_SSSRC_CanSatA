#include "mbed.h"
​
Serial pc(USBTX, USBRX);
DigitalIn FlightPin(D12);
DigitalOut Nichrome(D2);
PwmOut pinAFin(D9);
PwmOut pinARin(D10);
DigitalOut STBY(D11);
PwmOut servo(D6);
​
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
​
​
int main(){
    //フライトピン
 // pc.printf("フライトピンの状態を表示します\r\n");
 //   pc.printf("OFF\r\n");
​
    while(1){
        if(FlightPin.read() == 1){
            pc.printf("ON\r\n");
            break;
        }
    }
        wait(60);
​
    pc.printf("start\r\n");
    Nichrome = 1;
    //wait(5);
    for(int i=1;i<5;i++){
        wait(1);
        pc.printf("%d second\r\n",i);
    }
    Nichrome = 0;
    pc.printf("end\r\n");
​
/*走行，測距，GPS
​
​
​
​
​
*/
​
//採取
    STBY = 1;                               //モータドライバON
    servo.period_ms(20);                    //採取機構回転装置回転
    int a;
    for(a=500; a<=2400; a=a+950){
        servo.pulsewidth_us(a);             //90度回転
        driveMoter(0);
        wait(3);
        driveMoter(0.2);                    //ラック降下
        wait(1.5);
        driveMoter(0);
        wait(3);
        driveMoter(-0.2);                   //ラック上昇
        wait(1.5);
        driveMoter(0);
        wait(3);
        }
    STBY = 0;
​
/*走行，測距，GPS
​
​
​
​
​
*/
​
STBY = 1;                               //モータドライバON
    servo.period_ms(20);                    //採取機構回転装置回転
    int a;
    for(a=500; a<=2400; a=a+950){
        servo.pulsewidth_us(a);             //90度回転
        driveMoter(0);
        wait(3);
        driveMoter(0.2);                    //ラック降下
        wait(1.5);
        driveMoter(0);
        wait(3);
        driveMoter(-0.2);                   //ラック上昇
        wait(1.5);
        driveMoter(0);
        wait(3);
        }
    STBY = 0;
​
/*走行，測距，GPS
​
​
​
​
​
*/
​
STBY = 1;                               //モータドライバON
    servo.period_ms(20);                    //採取機構回転装置回転
    int a;
    for(a=500; a<=2400; a=a+950){
        servo.pulsewidth_us(a);             //90度回転
        driveMoter(0);
        wait(3);
        driveMoter(0.2);                    //ラック降下
        wait(1.5);
        driveMoter(0);
        wait(3);
        driveMoter(-0.2);                   //ラック上昇
        wait(1.5);
        driveMoter(0);
        wait(3);
        }
    STBY = 0;
​
/*走行，測距，GPS
​
​
​
​
​
*/
}