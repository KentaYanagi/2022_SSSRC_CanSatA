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

int main(void){
    float data;
    int count = 0;
    float prevData = 0;
    float dif = 0;
    float gosa = 10; //許容する誤差の値
    int maxcount = 3; //この回数だけ範囲内に収まったら以降の動作を実行する

    STBY=1;
    wait(10);
    while(1){
        while(1){
            wait(0.15);/*測距する間隔*/
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
            printf("回避します。%f\r\n",data);
            printf("後進\r\n");
            motorBack();
            wait(1);
            printf("右旋回\r\n");
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
     }
    }
