#include "mbed.h"

//VARIABLES DEFINITION
float time_turning_90_degrees = 0.43;
double leftwidth = 0.5;
double rightwidth = 0.5;

//PIN DEFINITION
//SERIAL COMMUNICATION
Serial pc(SERIAL_TX, SERIAL_RX);
//CONDITION CHECKER
DigitalIn flightpin(D12);
DigitalOut nichrome_wire(D2);
//MOTOR
PwmOut AIN1(A5);
PwmOut AIN2(A6);
PwmOut BIN1(A2);
PwmOut BIN2(A1);
//INFRARED SENSOR
AnalogIn ranging(A4);
DigitalOut STBY_moving(D13);
//COLLECTION MECHANISM
PwmOut pinAFin(D9);
PwmOut pinARin(D10);
DigitalOut STBY_collecting(D11);
PwmOut servo(D6);

//FUNCTION DECLARATION
//MOTOR MOVEMENT
void motor_movement()
void forward_for_x_microsec(long int);
void backward_for_x_microsec(long int);
void stop(void);
void turn_right_x_degrees(float);
void turn_right_x_degrees(float);
float get_forward_distance(void);

//MOTOR_INFRARED_SENSOR
Timer moving_timer;



int main()
{
    //MORTOR MOVEMENT
    STBY_moving = 1;


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

    pc.printf("start exploring\r\n");



    return 0;
}

void forward_for_x_microsec(long int x)
{
    stop();
    moving_timer.reset();
    moving_timer.start();
    wait_ms(1000);
    moving_timer.stop();
    
    while (x <= (moving_timer.read_ms() + 1000))
    {
        moving_timer.start();
        AIN1 = 0;
        AIN2 = leftwidth;
        BIN1 = 0;
        BIN2 = rightwidth;
        wait_ms(1000);
        moving_timer.stop();

        if(){
            break;
        }
    }
    Stop();
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
    float a,b,c,d,e;
    a = -1.135;
    c = 7.0796;
    d = ranging.read();
    b = pow(d,a);
    e = c*b;
    return e;
}
