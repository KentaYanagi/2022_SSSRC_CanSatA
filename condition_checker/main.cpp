#include "mbed.h"
#include "MPU6050.h"

Serial pc(SERIAL_TX, SERIAL_RX);
MPU6050 accelerometer_mpu6050(D4,D5);//(SDA,SCL)のピン配置
DigitalIn flightpin(D12);
DigitalOut nichrome_wire(D2);
Timer count_down_for_nichrome_wire;
void heatnichrome_wire(int);
void mpucheck(int);

int main() {
    int check = 0;
    // pc.printf("フライトピンの状態を表示します\r\n");
    // pc.printf("OFF\r\n");
    while (1){
        if (flightpin.read() == 1){
            // pc.printf("ON\r\n");
            break;
        }
    }
    // pc.printf("モジュールの電源をONにします\r\n");
    mpucheck(check);
}

void heatnichrome_wire(int check){
    if(check == 0){
    // pc.printf("start/r/n");
    nichrome_wire = 1;
    wait(5.0);
    nichrome_wire = 0;
    // pc.printf("end\r\n");
    }
}

void mpucheck(int check){
    count_down_for_nichrome_wire.start();
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

        if (check == 0){
            pc.printf("Z=%f Z_old=%f\r\n",Z,Z_old);
            // ローパスフィルター(現在の値 = 係数 * ひとつ前の値 ＋ (1 - 係数) * センサの値)
            // lowpassValue = lowpassValue * filterCoefficient + a * (1 - filterCoefficient);
            // ハイパスフィルター(センサの値 - ローパスフィルターの値)//
            // highpassValue = a - lowpassValue;

            // 速度計算(加速度を台形積分する)
            // speed = ((highpassValue + oldAccel) * timespan) / 2 + speed;
            // oldAccel = highpassValue;

            // 変位計算(速度を台形積分する)
            // difference = ((speed + oldSpeed) * timespan) / 2 + difference;
            // oldSpeed = speed;

            // pc.printf(" speed %f difference %f\r\n",speed,difference);//速度と加速度を表示
            // pc.printf("%f,",speed);//速度を表示
            // pc.printf("speed %f diference %f\r\n",speed,difference);//速度と変位を表示
            wait(0.01);
        }
        if(abs(Z-Z_old) >= 10.0){
            if(count_down_for_nichrome_wire.read() >= 5){
                check = 1;
            }
        }
        Z_old = Z;
        if (check == 1){
            break;
            }
        if(count_down_for_nichrome_wire.read() >= 25){
        break;
            }   
    }
    printf("a\n");
}
