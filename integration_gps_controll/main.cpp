#include "mbed.h" //mbed Standard Library
#include "getGPS.h" //GPS Library
//#include "math.h" //Math Library

//この下にCanSatno15回の旋回時間，目標座標を記入する,フライトピンの信号から切断開始まで時間
int t15= 29180;
double gg1=135.508111;
double gg2=34.545111;
int flight_cut = 10000;

//condition checker
Serial pc(USBTX, USBRX); //tx, rx Flight Pin
DigitalIn flightpin(D12); 
//nichrome_wire
DigitalOut nichrome_wire(D2);
//rack and pinion
PwmOut pinAFin(D9);
PwmOut pinARin(D10);
DigitalOut STBY(D11);
//rotate collector
PwmOut servo(D6);
//detect distance
AnalogIn ranging(A4);
//gps
Serial pc(SERIAL_TX, SERIAL_RX); //試験用 GPS
GPS gps(D1, D0); //GPS object

void driveMoter(float);  //rack and pinion



int main(){
    //フライトピン
    pc.printf("Started CanSat\r\n");
    pc.printf("Show flightpin_condition\r\n");
    pc.printf("OFF\r\n");

    while(1){
        if(flightpin.read() == 1){
            pc.printf("ON\r\n");
            break;
        }
    }
        wait(60);

    pc.printf("nichrome_wire was heated\r\n");
    nichrome_wire = 1;
    //wait(5);
    for(int i=1;i<5;i++){
        wait(1);
        pc.printf("%d second\r\n",i);
    }
    nichrome_wire = 0;
    pc.printf("end\r\n");

/*走行，測距，GPS
​
​
​
​
​
*/

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

/*走行，測距，GPS
​
​
​
​
​
*/

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

/*走行，測距，GPS
​
​
​
​
​
*/

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

/*走行，測距，GPS
​
​
​
​
​
*/
}

//rack and pinion
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

void up_down_collector(void) {
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
}
