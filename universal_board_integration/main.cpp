#include "mbed.h"

//VARIABLES DEFINITION
float time_turning_90_degrees = 0.43;

//PIN DEFINITION
//SERIAL COMMUNICATION
Serial pc(SERIAL_TX, SERIAL_RX);
//CONDITION CHECKER
DigitalIn flightpin(D12);
DigitalOut nichrome_wire(D2);
//MOTOR
DigitalOut AIN1(A5);
DigitalOut AIN2(A6);
DigitalOut BIN1(A2);
DigitalOut BIN2(A1);
//INFRARED SENSOR
AnalogIn ranging(A4);
DigitalOut STBY(D13);
//COLLECTION MECHANISM
PwmOut pinAFin(D9);
PwmOut pinARin(D10);
DigitalOut STBY(D11);
PwmOut servo(D6);

//FUNCTION DECLARATION
//MOTOR MOVEMENT
void motorForward(void);
void motorStop(void);
void motorRight(void);
void motorLeft(void);
void motorBack(void);
float dis(void);

void motorForward() {
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
    wait(0.43);/*旋回角度設定　90度*/
    motorStop();
}
void motorLeft() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 1;
    wait(0.43);/*旋回角度設定 90度*/
    motorStop();
}
void motorStop() {
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
    wait(3);/*停止時間設定*/
    }
void motorBack(){
    AIN1 = 0;
    AIN2 = 1;
    BIN1 = 0;
    BIN2 = 1;
}
    
float dis(void){
    float a,b,c,d,e;
    a=-1.135;
    c=7.0796;
    d=ranging.read();
    b=pow(d,a);
    e=c*b;
    wait(0.05);
    return e;
}

//COLLECTION MECHANISM
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

Timer moving_timer;

int main()
{
    //MORTOR MOVEMENT
    STBY = 1;


    pc.printf("display the conditon of the flight pin\r\n");
    pc.printf("flight pin OFF\r\n");
    while(1){
        if(flightpin.read() == 1){
            pc.printf("flifght pin ON\r\n");
            break;
        }
    }
    pc.printf("wait for 60s from flight pin signal to heating nichrome wire\r\n");
    wait(60);

    pc.printf("heating nichrome_wire start\r\n");
    nichrome_wire = 1;
    for(int i=1;i<=5;i++){
        wait(1);
        pc.printf("heating nichrome_wire for %d second\r\n",i);
    }
    nichrome_wire = 0;
    pc.printf("heating nichrome_wire end\r\n");

    pc.printf("start exploring for about 2 min\r\n");

    float data;
    int count = 0;
    float prevData = 0;
    float dif = 0;
    float gosa = 10; //許容する誤差の値
    int maxcount = 3; //この回数だけ範囲内に収まったら以降の動作を実行する

    moving_timer.reset();
    moving_timer.start();

    wait(10);

    moving_timer.stop();

    while(moving_timer.read() < 120){

        moving_timer.start();

        while(1){
            wait(0.25);/*測距する間隔*/
            count = 0;
            data=dis();
            if(data<=50){
                dif = data - prevData;
                printf("誤差%f、データ%f\r\n",dif,data);
                if(-gosa < dif && dif < gosa){
                    printf("%d回目の誤差検知クリアしました\r\n", count+1);
                    count++; 
                } 
                else {
                    motorStop();
                    printf("誤差を検知したので止まります\r\n");
                    count = 0;
                }
                if(count >= maxcount) {
                    printf("誤差検知のループを抜けました\r\n");
                    break;
                }
                prevData = data;
            }
            else{
                break;
            }
                
        }

        if(data<=30){
            motorStop();
            printf("後進%f\r\n",data);
            motorBack();
            wait(1);
            printf("右旋回%f\r\n",data);
            motorRight();
            printf("前進\r\n");
            motorForward();
            wait(3);/*並走時の前進距離設定、フローチャートなら10秒*/
            printf("左旋回\r\n");
            motorLeft();
        }
    
        else{
            printf("前進し続ける%f\r\n",data);
            motorForward(); 
        }

        moving_timer.stop();
    }

    pc.printf("end exploring\r\n");
    pc.printf("start collecting\r\n");

    //COLLECTION MECHANISM
    servo.period_ms(20);
    int a;
    for(a=500; a<=2400; a=a+950){
        servo.pulsewidth_us(a);
        driveMoter(0);
        wait(3);
        driveMoter(0.2);
        wait(1.5);
        driveMoter(0);
        wait(3);
        driveMoter(-0.2);
        wait(1.5);
        driveMoter(0);
        wait(3);
    }

    pc.printf("end collecting\r\n");
    pc.printf("finished mission\r\n");

    return 0;
}