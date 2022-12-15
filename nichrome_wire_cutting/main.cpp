#include "mbed.h"
#include "MPU6050.h"

Serial pc(SERIAL_TX, SERIAL_RX);
MPU6050 mpu(D4,D5);//(SDA,SCL)のピン配置
DigitalOut STBY(D13);
DigitalIn FlightPin(D12);
DigitalOut Nichrome(D2);
PwmOut AIN1(A5);
PwmOut AIN2(A6);
PwmOut BIN1(A2);
PwmOut BIN2(A1);
Timer countdown;
void heatnichrome(int);
void mpucheck(int);
void motorStop();
void motorForward();

int main() {
    int check = 0;
    // pc.printf("フライトピンの状態を表示します\r\n");
    // pc.printf("OFF\r\n");
    while (1){
        if (FlightPin.read() == 1){
            // pc.printf("ON\r\n");
            break;
        }
    }
    // pc.printf("モジュールの電源をONにします\r\n");
    mpucheck(check);
}

void heatnichrome(int check){
    if(check == 0){
        STBY = 1;
        printf("Start\n");
        Nichrome = 1;
    
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
    Nichrome = 0;
    printf("End\n");
    }
}

void mpucheck(int check){
        countdown.start();
        float filterCoefficient = 0.9; // ローパスフィルターの係数(これは環境によって要調整。1に近づけるほど平滑化の度合いが大きくなる。
        float lowpassValue = 0;
        float highpassValue = 0;
        float speed = 0;//加速度時から算出した速度
        float oldSpeed = 0;//ひとつ前の速度
        float oldAccel = 0;//ひとつ前の加速度
        float difference=0;//変位
        float timespan=0.01;//時間差
        int accel[3];//accelを3つの配列で定義。
        float Z_old = 0.0;

    while (1){
            mpu.readAccelData(accel); // 加速度の値をaccel[3]に代入
            int z = accel[2] + 1110;  // z軸方向の加速度
            float Z = z * 0.000597964111328125 - 9.8;

        if (check == 0){
            pc.printf("Z=%f Z_old=%f\r\n",Z,Z_old);
  
            wait(0.01);
        }
        if(abs(Z-Z_old) >= 10.0){
            if(countdown.read() >= 15){
                check = 1;
            }
        }
        Z_old = Z;
        if (check == 1){
            break;
            }
        if(countdown.read() >= 40){
        break;
            }   
    }
    heatnichrome(check);
}

void motorForward() {
    motorStop();
    AIN1 = 0;
    AIN2 = 1;
    BIN1 = 0;
    BIN2 = 1;
}

void motorStop() {
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}
