#include "mbed.h"
#include "string.h"
#include "MPU6050.h"

// VARIABLES DEFINITION
int time_turning_90_degrees = 430000;
double mainmotor_pwm_leftwidth = 1.0;
double mainmotor_pwm_rightwidth = 1.0;
double rack_pinion_pwm_width = 0.2;

float forward_distance_to_obstacle = 0;
float prevData = 0;
float dif = 0;
float gosa = 10;  // 許容する誤差の値
int maxcount = 3; // この回数だけ範囲内に収まったら以降の動作を実行する
int infraredsensor_interval_milisec = 150;

int servomotor_pulsewidth = 500;

// PIN DEFINITION
// SERIAL COMMUNICATION
Serial pc(SERIAL_TX, SERIAL_RX);
// CONDITION CHECKER
DigitalIn flightpin(D12);
DigitalOut nichrome_wire(D2);
// MOTOR
PwmOut AIN1(A6);
PwmOut AIN2(A5);
PwmOut BIN1(A2);
PwmOut BIN2(A1);
// INFRARED SENSOR
AnalogIn ranging(A4);
DigitalOut STBY_moving(D13);
// COLLECTION MECHANISM
PwmOut pinAFin(D9);
PwmOut pinARin(D10);
DigitalOut STBY_collecting(D11);
PwmOut servo(D6);
// MPU6050
MPU6050 accelerometer_mpu6050(D4, D5);// Position PIN (SDA, SCL)

// FUNCTION DECLARATION
// MOTOR MOVEMENT
void cansat_move(long int move_time, char direction[10]);
void cansat_turn(float degree, char direction[10]);
void mainmotor_driver_pin(char direction[10]);
// INFRARED SENSOR
float get_forward_distance_to_obstacle(void);
// COLLECTION MECHANISM
void rack_pinion_movement(char direction[10], int move_time);
void collection_mechanism(void);
// CONDITION CHECKER
void heatnichrome_wire(int nichrome_wire_triger);
void mpucheck();

// MOTOR_INFRARED_SENSOR
Timer moving_timer;
// CONDITION CHECKER
Timer count_down_for_nichrome_wire;

int main()
{
    // MORTOR MOVEMENT
    STBY_moving = 1;


    pc.printf("display the conditon of the flight pin\r\n");
    pc.printf("flight pin OFF\r\n");
    while (1)
    {
        if (flightpin.read() == 1)
        {
            pc.printf("flight pin ON\r\n");
            break;
        }
    }
    mpucheck();

/*
    pc.printf("wait for 60s from flight pin signal to heating nichrome_wire wire\r\n");
    wait(60);
    pc.printf("heating nichrome_wire start\r\n");
    nichrome_wire = 1;
    for (int i = 1; i <= 5; i++)
    {
        wait(1);
        pc.printf("heating nichrome_wire for %d second\r\n", i);
    }
    nichrome_wire = 0;
    pc.printf("heating nichrome_wire end\r\n");
*/
    pc.printf("start exploring\r\n");

    for (int i = 1; i <= 3; i++)
    {
        cansat_move(10000, "forward");
        mainmotor_driver_pin("stop");
        pc.printf("end exploring\r\n");
        pc.printf("start collecting\r\n");
        collection_mechanism();
        pc.printf("end collecting\r\n");
        pc.printf("start exploring\r\n");
        cansat_turn(45, "right");
        mainmotor_driver_pin("stop");
    }
    
    servo.pulsewidth_us(500);
    pc.printf("finished mission\r\n");
    return 0;
}

void cansat_move(long int move_time, char direction[10])
{
    if(strcmp(direction, "forward") == 0)
    {
        moving_timer.reset();
        moving_timer.start();
        wait_ms(1000);
        moving_timer.stop();
        int evasive_action_count = 1;
        int count = 0;

        while (move_time >= (moving_timer.read_ms() + 1000*evasive_action_count))
        {
            while(1){
                forward_distance_to_obstacle = get_forward_distance_to_obstacle();
                if (forward_distance_to_obstacle <= 50)
                {
                    dif = forward_distance_to_obstacle - prevData;
                    printf("誤差%f、データ%f\r\n", dif, forward_distance_to_obstacle);
                    if (-gosa < dif && dif < gosa)
                    {
                        printf("%d回目の誤差検知クリアしました\r\n", count + 1);
                        count++;
                    }
                    else
                    {
                        mainmotor_driver_pin("stop");
                        printf("誤差を検知したので,誤差検知をやり直します 安全のため停止しました\r\n");
                        count = 0;
                    }
                    if (count >= maxcount)
                    {
                        printf("誤差検知のループを抜けました\r\n");
                        break;
                    }
                    prevData = forward_distance_to_obstacle;
                }
                else
                {
                    break;
                }
            }

            if (forward_distance_to_obstacle <= 30)
            {
                moving_timer.stop();
                mainmotor_driver_pin("stop");
                printf("回避します。衝突までの距離は%fcm\r\n", forward_distance_to_obstacle);
                printf("後退\r\n");
                mainmotor_driver_pin("backward");
                wait(1);
                mainmotor_driver_pin("stop");
                wait(1.5);
                printf("右旋回\r\n");
                cansat_turn(90.0, "right");
                mainmotor_driver_pin("stop");
                wait(1.5);
                printf("前進\r\n");
                mainmotor_driver_pin("forward");
                wait(1);
                mainmotor_driver_pin("stop");
                wait(1.5);
                printf("左旋回\r\n");
                cansat_turn(90.0, "left");
                evasive_action_count++;
            }

            else
            {

                printf("前進し続ける. 前方の障害物までの距離は%f\r\n", forward_distance_to_obstacle);
                moving_timer.start();
                mainmotor_driver_pin("forward");
                wait_ms(infraredsensor_interval_milisec);
                moving_timer.stop();
                moving_timer.start();
            }
        }
    }
    else if(strcmp(direction, "backward") == 0)
    {
        moving_timer.reset();
        moving_timer.start();
        wait_ms(1000);
        moving_timer.stop();
        mainmotor_driver_pin("backward");
        while (move_time <= (moving_timer.read_ms() + 1000))
        {
            moving_timer.start();
            mainmotor_driver_pin("backward");
            wait_ms(10);
            moving_timer.stop();
        }
    }    
}


void cansat_turn (float degree, char direction[10])
{
    int turn_time;
    turn_time = (int)(time_turning_90_degrees * degree / 90);
    if (strcmp(direction, "left") == 0)
    {
        mainmotor_driver_pin("left");
    }
    else if (strcmp(direction, "right") == 0)
    {
        mainmotor_driver_pin("right");
    }
    wait_us(turn_time);
}

void mainmotor_driver_pin(char direction[10])
{
    if (strcmp(direction, "forward") == 0)
    {
        AIN1 = mainmotor_pwm_leftwidth;
        AIN2 = 0;
        BIN1 = mainmotor_pwm_rightwidth;
        BIN2 = 0;
    }
    else if (strcmp(direction, "backward") == 0)
    {
        AIN1 = 0;
        AIN2 = mainmotor_pwm_leftwidth;
        BIN1 = 0;
        BIN2 = mainmotor_pwm_rightwidth;
    }
    else if (strcmp(direction, "stop") == 0)
    {
        AIN1 = 0;
        AIN2 = 0;
        BIN1 = 0;
        BIN2 = 0;
    }
    else if (strcmp(direction, "left") == 0)
    {
        AIN1 = 0;
        AIN2 = mainmotor_pwm_leftwidth;
        BIN1 = mainmotor_pwm_rightwidth;
        BIN2 = 0;
    }
    else if (strcmp(direction, "right") == 0)
    {
        AIN1 = mainmotor_pwm_leftwidth;
        AIN2 = 0;
        BIN1 = 0;
        BIN2 = mainmotor_pwm_rightwidth;
    }
}

// INFRARED SENSOR
float get_forward_distance_to_obstacle(void)
{
    float a, b, c, d, e;
    a = -1.135;
    c = 7.0796;
    d = ranging.read();
    b = pow(d, a);
    e = c * b;
    return e;
}

// COLLECTION MECANISM
void rack_pinion_movement(char direction[10], int move_time){
    printf("f\r\n");
    if(strcmp(direction, "down") == 0){
        pinAFin=rack_pinion_pwm_width;
        pinARin=0;
    }else if(strcmp(direction, "up") == 0){
        pinAFin=0;
        pinARin=rack_pinion_pwm_width;
    }else if(strcmp(direction, "stop") == 0){
        pinAFin=0;
        pinARin=0;
    }
    wait_ms(move_time);

    pinAFin=0;
    pinARin=0;
}

void collection_mechanism(void)
{
    servo.period_ms(20);
    STBY_collecting = 1;

    for(int i = 0; i<3; i++)
    {
        servo.pulsewidth_us(servomotor_pulsewidth);
        servomotor_pulsewidth += 210;
        rack_pinion_movement("stop", 3000);
        rack_pinion_movement("down", 1200);
        rack_pinion_movement("stop", 3000);
        rack_pinion_movement("up", 1200);
        rack_pinion_movement("stop", 3000);
    }
}

// CONDITION CHECKER
void mpucheck (void) {
    count_down_for_nichrome_wire.start();
    wait(3.0);
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
            if(count_down_for_nichrome_wire.read() >= 3){
                nichrome_wire_triger = 1;
            }
        }
        Z_old = Z;
        if (nichrome_wire_triger == 1){
            break;
            }
        if(count_down_for_nichrome_wire.read() >= 30){
            nichrome_wire_triger = 1;
            break;
            }   
    }
    heatnichrome_wire(nichrome_wire_triger);
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
