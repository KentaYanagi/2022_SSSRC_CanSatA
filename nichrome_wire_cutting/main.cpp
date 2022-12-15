#include "mbed.h"
#include "MPU6050.h"

Serial pc(SERIAL_TX, SERIAL_RX);
MPU6050 accelerometer_mpu6050(D4,D5);//(SDA,SCL)のピン配置
DigitalOut STBY(D13);
DigitalIn flightpin(D12);
DigitalOut nichrome_wire(D2);
PwmOut AIN1(A5);
PwmOut AIN2(A6);
PwmOut BIN1(A2);
PwmOut BIN2(A1);
Timer count_down_for_nichrome_wire;
void heatnichrome_wire(int);
void mpucheck();
void motorStop();
void motorForward();

int main() {

    STBY = 1;
    
    // pc.printf("フライトピンの状態を表示します\r\n");
    // pc.printf("OFF\r\n");
    while (1){
        if (flightpin.read() == 1){
            // pc.printf("ON\r\n");
            break;
        }
    }
    // pc.printf("モジュールの電源をONにします\r\n");
    mpucheck();
}

void heatnichrome_wire(int nichrome_wire_triger){
    if(nichrome_wire_triger == 1){
        printf("Start\n");
        nichrome_wire = 1;
    
    for(int i=0;i<5;i++){
        wait(1);
        printf("%d second\n", i+1);    
    }

    printf("%s\r\n", "走行開始");
    cansat_move(5000, "forward");
    
    mainmotor_driver_pin("stop");
    nichrome_wire = 0;
    printf("End\n");
    }
}

void mpucheck (void) {

    count_down_for_nichrome_wire.start();

    int nichrome_wire_triger = 0;
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
            accelerometer_mpu6050.readAccelData(accel); // 加速度の値をaccel[3]に代入
            int z = accel[2] + 1110;  // z軸方向の加速度
            float Z = z * 0.000597964111328125 - 9.8;

        if (nichrome_wire_triger == 0){
            pc.printf("Z=%f Z_old=%f\r\n",Z,Z_old);
  
            wait(0.01);
        }
        if(abs(Z-Z_old) >= 10.0){
            if(count_down_for_nichrome_wire.read() >= 15){
                nichrome_wire_triger = 1;
            }
        }
        Z_old = Z;
        if (nichrome_wire_triger == 1){
            break;
            }
        if(count_down_for_nichrome_wire.read() >= 40){
        break;
            }   
    }
    heatnichrome_wire(nichrome_wire_triger);
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
