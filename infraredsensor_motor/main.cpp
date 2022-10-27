#include <mbed.h>
#include <math.h>
AnalogIn ranging(A4);
DigitalOut STBY(D13);
DigitalOut AIN1(A5);
DigitalOut AIN2(A6);
DigitalOut BIN1(A2);
DigitalOut BIN2(A1);
void motorForward(void);
void motorStop(void);
void motorRight(void);
void motorLeft(void);
void motorBack(void);
float dis(void);
Serial pc(USBTX,USBRX);
void motorForward() {
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 1;
    BIN2 = 0;
}
void motorRight() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 1;
    wait(0.43);/*旋回角度設定　90度*/
    motorStop();
}
void motorLeft() {
    motorStop();
    AIN1 = 0;
    AIN2 = 1;
    BIN1 = 1;
    BIN2 = 0;
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
    wait(0.2);
    return e;
    }
int main(void){
    float data;
    int count = 0;
    int prevData = 0;
    int dif = 0;
    int gosa = 10; //許容する誤差の値
    int maxcount = 3; //この回数だけ範囲内に収まったら以降の動作を実行する
    STBY=1;
    wait(10);
    while(1){
        while(1){
            data=dis();
            dif = data - prevData;
            if(-gosa < dif && dif < gosa){
               count++;
            } else {
                count = 0;
                }
            if(count >= maxcount) break;
            prevData = data;
        }
        if(data>=20&&data<60){
            printf("右%f\n",data);
            motorRight();
            printf("前進\n");
            motorForward();
            wait(3);/*並走時の前進距離設定*/
            printf("左\n");
            motorLeft();
            data=dis();
            if(data>60){
                printf("前進%f\n",data);
                motorForward();
                wait(1);/*障害物を乗り越える前進距離設定*/
                printf("止まります\n");
                motorStop();/*GPS取得*/
                wait(30);
                printf("終了してください\n");
        }
    }
    else if(data<20){
        printf("後ろ%f\n",data);
    motorBack();
    }
    else{
        printf("前進し続ける%f\n",data);
    motorForward();
    }
    }
}