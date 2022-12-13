#include "mbed.h"

// VARIABLES DEFINITION
float time_turning_90_degrees = 0.43;
double leftwidth = 1.0;
double rightwidth = 1.0;

float data;
int count = 0;
float prevData = 0;
float dif = 0;
float gosa = 10;  // 許容する誤差の値
int maxcount = 3; // この回数だけ範囲内に収まったら以降の動作を実行する
float infraredsensor_interval = 0.15;

// PIN DEFINITION
// SERIAL COMMUNICATION
Serial pc(SERIAL_TX, SERIAL_RX);
// CONDITION CHECKER
DigitalIn flightpin(D12);
DigitalOut nichrome_wire(D2);
// MOTOR
PwmOut AIN1(A5);
PwmOut AIN2(A6);
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

// FUNCTION DECLARATION
// MOTOR MOVEMENT
void motor_movement();
void cansat_move_for_x_microsec(long int);
void backward_for_x_microsec(long int);
void stop(void);
void turn_right_x_degrees(float);
void turn_right_x_degrees(float);
float get_forward_distance(void);
void forward_motor_driver_pin(void);
void backward_motor_driver_pin(void);
void stop_motor_driver_pin(void);
void turn_right_motor_driver_pin(void);
void turn_left_motor_driver_pin(void);

// MOTOR_INFRARED_SENSOR
Timer moving_timer;

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
            pc.printf("flifght pin ON\r\n");
            break;
        }
    }

    pc.printf("wait for 60s from flight pin signal to heating nichrome wire\r\n");
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
    pc.printf("start exploring\r\n");

    return 0;
}

void cansat_move_for_x_microsec(long int x)
{
    stop();
    moving_timer.reset();
    moving_timer.start();
    wait_ms(1000);
    moving_timer.stop();

    
    
        moving_timer.start();
        AIN1 = 0;
        AIN2 = leftwidth;
        BIN1 = 0;
        BIN2 = rightwidth;
        wait_ms(1000);
        moving_timer.stop();


    while (x <= (moving_timer.read_ms() + 1000))
    {
        wait(infraredsensor_interval);
        data = dis();
        if (data <= 50)
        {
            dif = data - prevData;
            printf("誤差%f、データ%f\r\n", dif, data);
            if (-gosa < dif && dif < gosa)
            {
                printf("%d回目の誤差検知クリアしました\r\n", count + 1);
                count++;
            }
            else
            {
                motorStop();
                printf("誤差を検知したので止まります\r\n");
                count = 0;
            }
            if (count >= maxcount)
            {
                printf("誤差検知のループを抜けました\r\n");
                break;
            }
            prevData = data;
        }
        else
        {
            break;
        }
    }

    if (data <= 30)
    {

        motorStop();
        printf("回避します。%f\r\n", data);
        printf("後進\r\n");
        motorBack();
        wait(1);
        printf("右旋回\r\n");
        motorRight();
        printf("前進\r\n");
        motorForward();
        wait(3); /*並走時の前進距離設定、フローチャートなら10秒*/
        printf("左旋回\r\n");
        motorLeft();
    }

    else
    {

        printf("前進し続ける%f\r\n", data);
        motorForward();
    }
}

void backward_for_x_microsec(long int x)
{
    stop();
    AIN1 = leftwidth;
    AIN2 = 0;
    BIN1 = rightwidth;
    BIN2 = 0;
    wait_ms(x);
    Stop();
}

void turn_right_x_degrees(float x)
{
    Stop();
    AIN1 = leftwidth;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = rightwidth;
    wait_ms(x);
    Stop();
}
void stop()
{
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}

float get_forward_distance(void)
{
    float a, b, c, d, e;
    a = -1.135;
    c = 7.0796;
    d = ranging.read();
    b = pow(d, a);
    e = c * b;
    return e;
}

void forward_motor_driver_pin(void)
{
    AIN1 = leftwidth;
    AIN2 = 0;
    BIN1 = rightwidth;
    BIN2 = 0;
}

void backward_motor_driver_pin(void)
{
    AIN1 = 0;
    AIN2 = leftwidth;
    BIN1 = 0;
    BIN2 = rightwidth;
}

void stop_motor_driver_pin(void)
{
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}

void turn_right_motor_driver_pin(void)
{
    AIN1 = 0;
    AIN2 = leftwidth;
    BIN1 = rightwidth;
    BIN2 = 0;
}

void turn_left_motor_driver_pin(void)
{
    AIN1 = leftwidth;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = rightwidth;
}
