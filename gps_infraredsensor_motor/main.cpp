// Motor & GPS
#include "mbed.h"
// only GPS
#include "getGPS.h"
//統合試験
//#include "math.h"
// Exit関数
//#include "stdlib.h"
// Sleep関数 milli second
//#include "windows.h" //これはまず使えない

//この下代入してね！
long int t15 = 29180; //revolution time 15times
double gg1 = 135.508111; //goal latitude
double gg2 = 34.545111;  //goal longitude
double leftwidth = 0.5;
double rightwidth = 0.5;

// only GPS
Serial pc(SERIAL_TX, SERIAL_RX); //試験用

GPS gps(D1, D0);

// only Motor

PwmOut AIN1(A5);
PwmOut AIN2(A6);
PwmOut BIN1(A2);
PwmOut BIN2(A1);

// only infraredsensor
AnalogIn ranging(A4);

// GPS & infraredsensor
DigitalOut STBY(D13);

Timer moving_timer;

void Forward(int);
void Backward(int);
void Stop(void);
void Turn(int);
double a_ra(double);

//地質により変動
double rt = 0.43; // rotate ∏/2 time

double g11, g21, g31, g41, g51, g61, g71, g81, g91; // g1(着地地点） g2(スタート向き計測) g3(中間地点) g4(採取地点1) g5(中間地点) g6(採取地点2) g7(中間地点) g8(採取地点3）　g9(中間地点)
double g12, g22, g32, g42, g52, g62, g72, g82, g92;
double sg31, sg32, sg41, sg42, sg51, sg52, sg61, sg62, sg71, sg72, sg81, sg82;
double sp11, sp12, sp21, sp22, sp31, sp32, sp41, sp42, sp51, sp52, sp61, sp62, sp71, sp72, sp81, sp82, sp91, sp92;
double l1, l2, l3, l4, l5, l6, l7, l8, l9;
double ras1, ras2, ras3, ras4, ras5, ras6, ras7, ras8, ras9;
double rac1, rac2, rac3, rac4, rac5, rac6, rac7, rac8, rac9;
double sin1, sin2, sin3, sin4, sin5, sin6, sin7, sin8, sin9;
double cos1, cos2, cos3, cos4, cos5, cos6, cos7, cos8, cos9;
double ra, a;
double a1, a2, a3, a4, a5, a6, a7, a8, a9;
double ea12, ea23, ea34, ea45, ea56, ea67, ea78, ea89;
double ca32, ca43, ca54, ca65, ca67, ca78, ca89;
long int t3, t4, t5, t6, t7, t8, t9;
double td3, td4, td5, td6, td7, td8, td9;
long int tt3, tt4, tt5, tt6, tt7, tt8, tt9;
double ttd3, ttd4, ttd5, ttd6, ttd7, ttd8, ttd9;
double r12, r23, r34;

int main()
{
    STBY = 1;
    Stop();

    pc.printf("GPS_first start\r\n");
    while (1)
    {
        if (gps.getgps())
        { // GPSモジュールの機能確認
            pc.printf("GPS_first OK\r\n");
            break;
        }
    }

    pc.printf("Got GPS_first\r\nWaiting for 50s\r\n");
    wait_ms(50000);

    while (1)
    {
        if (gps.getgps())
        { //落下地点のGPS取得
            g11 = gps.latitude;
            g12 = gps.longitude;
            pc.printf("Complete getting GPS\r\n");
            pc.printf("g11=%f\r\n", g11);
            pc.printf("g12=%f\r\n", g12);
            break;
        }
    }

    Forward(); //移動30s
    pc.printf("Moving 30s\r\n");
    wait_ms(30000);
    Stop();
    pc.printf("Stop 50s\r\n");
    wait_ms(50000);

    while (1)
    {
        if (gps.getgps())
        { //方向認知のためのGPS取得
            g21 = gps.latitude;
            g22 = gps.longitude;
            pc.printf("Complete getting GPS_turn\r\n");
            pc.printf("g21=%f\r\n", g21);
            pc.printf("g22=%f\r\n", g22);
            break;
        }
    }

    //方向検知のためのベクトル検知
    sp11 = g21 - g11; // sp11=self_posture_11
    pc.printf("sp11=%f\r\n", sp11);
    sp12 = g22 - g12;
    pc.printf("sp12=%f\r\n", sp12);
    l1 = sqrt(pow(sp11, 2.0) + pow(sp12, 2.0));
    pc.printf("l1=%f\r\n", l1);
    cos1 = sp11 / l1;
    pc.printf("cos1=%f\r\n", cos1);
    sin1 = sp12 / l1;
    pc.printf("sin1=%f\r\n", sin1);
    ras1 = asin(sin1);
    pc.printf("ras1=%f\r\n", ras1);
    rac1 = acos(cos1);
    pc.printf("rac1=%f\r\n", rac1);

    if (sin1 >= 0 && cos1 >= 0)
    {
        ra = ras1;
    }

    if (sin1 >= 0 && cos1 < 0)
    {
        ra = rac1;
    }

    if (sin1 < 0)
    {
        ra = 2 * 3.141593 - rac1;
    }

    pc.printf("ra=%f\r\n", ra);

    a_ra(ra);
    a1 = a;
    pc.printf("a1=%f\r\n", a1);

    Forward(); //移動30s
    pc.printf("Moving 30s\r\n");
    wait_ms(30000);
    Stop();
    wait_ms(50000);
    pc.printf("Stop 50s\r\n");

    pc.printf("Started getting GPS_recogError\r\n");
    while (1)
    {
        if (gps.getgps())
        { //誤差検知のためのGPS取得
            g31 = gps.latitude;
            g32 = gps.longitude;
            pc.printf("Complete getting GPS_recogError\r\n");
            pc.printf("g31=%f\r\n", g31);
            pc.printf("g32=%f\r\n", g32);
            break;
        }
    }
    //誤差検知のためのベクトル検出
    sp21 = g31 - g21; // sp11=self_posture_11
    pc.printf("sp21=%f\r\n", sp21);
    sp22 = g32 - g22;
    pc.printf("sp22=%f\r\n", sp22);
    l2 = sqrt(pow(sp21, 2.0) + pow(sp22, 2.0));
    pc.printf("l2=%f\r\n", l2);
    cos2 = sp21 / l2;
    pc.printf("cos2=%f\r\n", cos2);
    sin2 = sp22 / l2;
    pc.printf("sin2=%f\r\n", sin2);
    ras2 = asin(sin2);
    pc.printf("ras2=%f\r\n", ras2);
    rac2 = acos(cos2);
    pc.printf("rac2=%f\r\n", rac2);

    if (sin2 >= 0 && cos2 >= 0)
    {
        ra = ras2;
    }
    if (sin2 >= 0 && cos2 < 0)
    {
        ra = rac2;
    }
    if (sin2 < 0)
    {
        ra = 2 * 3.141593 - rac1;
    }

    a_ra(ra);
    a2 = a;
    pc.printf("a2=%f\r\n", a2);
    ea12 = a2 - a1;
    pc.printf("ea12=%f\r\n", ea12);

    wait_ms(5000);

    r12 = l2 / l1; //土壌地質変化
    pc.printf("r12=%f\r\n", r12);

    sg31 = gg1 - g31; // Goalまでのベクトル検出
    pc.printf("sg31=%f\r\n", sg31);
    sg32 = gg2 - g32;
    pc.printf("sg32=%f\r\n", sg32);

    l3 = sqrt(pow(sg31, 2.0) + pow(sg32, 2.0)); // Goalまでの距離
    pc.printf("l3=%f\r\n", l3);
    cos3 = sg31 / l3;
    pc.printf("cos3=%f\r\n", cos3);
    sin3 = sg32 / l3;
    pc.printf("sin3=%f\r\n", sin3);
    ras3 = asin(sin3);
    pc.printf("ras3=%f\r\n", ras3);
    rac3 = acos(cos3);
    pc.printf("rac3=%f\r\n", rac3);

    if (sin3 >= 0 && cos3 >= 0)
    {
        ra = ras3;
    }
    if (sin3 >= 0 && cos2 < 0)
    {
        ra = rac3;
    }
    if (sin3 < 0)
    {
        ra = 2 * 3.141593 - rac3;
    }

    a_ra(ra);
    a3 = a;
    pc.printf("a3=%f\r\n", a3);

    ca32 = a3 - a2 + ea12; // Goalまでの角度
    pc.printf("ca32=%f\r\n", ca32);

    ttd3 = ca32 / 360 / 15 * t15; // Goal方向までの回転時間
    tt3 = (long int)ttd3;
    pc.printf("tt3=%d\r\n", tt3);

    Turn(); // Goalまでの角度まで回転
    pc.printf("Turning\r\n");
    wait_ms(tt3);
    Stop();
    pc.printf("Stop\r\n");
    wait_ms(5000);

    td3 = l3 / l2 * 30000; // Goalまでの時間

    if (l2 < 0.01)
        td3 = 60000;

    pc.printf("td3=%f\r\n", td3);
    t3 = (long int)td3;

    pc.printf("t3=%d\r\n", t3);

    Forward(); //移動t3
    pc.printf("Moving\r\n");
    wait_ms(t3);
    Stop();
    pc.printf("Stop 50s\r\n");
    wait_ms(50000);

    pc.printf("Started getting GPS_goal1\r\n");
    while (1)
    {
        if (gps.getgps())
        { //予想Goal地点1でのGPS取得
            g41 = gps.latitude;
            g42 = gps.longitude;
            pc.printf("Complete getting GPS_goal1\r\n");
            pc.printf("g41=%f\r\n", g41);
            pc.printf("g42=%f\r\n", g42);
            break;
        }
    }

    //位置情報検証 地球の赤道半径を6378.137kmとする 1度=111.3195km 1m=0.00000899322度
    //位置情報検証 地球の極半径を6356.752kmとする 1度=110.574km 1m=0.00000902956度  0.00000003634度の誤差

    if (g41 > gg1 - 0.0000899322 && g41 < gg1 + 0.0000899322 && g42 > gg2 - 0.0000902956 && g42 < gg2 + 0.0000902956)
    {
        pc.printf("Reached Goal\r\n");
        exit(0);
    }
    else
    {
        pc.printf("Didn't reach Goal. Continue to move to Goal2\r\n");
    }

    sp41 = g41 - g31; //誤差検出のためのベクトル検出
    pc.printf("sp41=%f\r\n", sp41);
    sp42 = g42 - g32;
    pc.printf("sp42=%f\r\n", sp42);
    l3 = sqrt(pow(sp41, 2.0) + pow(sp42, 2.0));
    pc.printf("l3=%f\r\n", l3);
    cos3 = sp31 / l3;
    pc.printf("cos3=%f\r\n", cos3);
    sin3 = sp32 / l3;
    pc.printf("sin3=%f\r\n", sin3);
    ras3 = asin(sin3);
    pc.printf("ras3=%f\r\n", ras3);
    rac3 = acos(cos3);
    pc.printf("ras3=%f\r\n", ras3);

    if (sin3 >= 0 && cos3 >= 0)
    {
        ra = ras3;
    }
    if (sin3 >= 0 && cos3 < 0)
    {
        ra = rac3;
    }
    if (sin3 < 0)
    {
        ra = 2 * 3.141593 - rac3;
    }

    a_ra(ra);
    a3 = a;
    pc.printf("a3=%f\r\n", a3);

    ea23 = a3 - a2;
    pc.printf("ea23=%f\r\n", ea23);

    wait_ms(5000);

    r23 = (l3 / t3) / (l2 / 30); //土壌地質変化
    pc.printf("r23=%f\r\n", r23);

    sg41 = gg1 - g41; // Goalまでのベクトル検出
    pc.printf("sg41=%f\r\n", sg41);
    sg42 = gg2 - g42;
    pc.printf("sg42=%f\r\n", sg42);

    l4 = sqrt(pow(sg41, 2.0) + pow(sg42, 2.0)); // Goalまでの距離
    pc.printf("l4=%f\r\n", l4);
    cos4 = sg41 / l4;
    pc.printf("cos4=%f\r\n", cos4);
    sin4 = sg42 / l4;
    pc.printf("sin4=%f\r\n", sin4);
    ras4 = asin(sin4);
    pc.printf("ras4=%f\r\n", ras4);
    rac4 = acos(cos4);
    pc.printf("rac4=%f\r\n", rac4);

    if (sin4 >= 0 && cos4 >= 0)
    {
        ra = ras4;
    }
    if (sin4 >= 0 && cos4 < 0)
    {
        ra = rac4;
    }
    if (sin4 < 0)
    {
        ra = 2 * 3.141593 - rac4;
    }

    a_ra(ra);
    a4 = a;
    pc.printf("a4=%f\r\n", a4);

    ca43 = a4 - a3 + ea23; // Goalまでの角度
    pc.printf("ca43=%f\r\n", ca43);

    ttd4 = ca32 / 360 / 15 * t15; // Goal方向までの回転時間
    tt4 = (long int)ttd4;
    pc.printf("tt4=%f\r\n", tt4);

    Turn(); // Goalまでの角度まで回転
    pc.printf("Turning\r\n");
    wait_ms(tt4);
    Stop();
    pc.printf("Stop 5s\r\n");
    wait_ms(5000);

    td4 = l4 / l3 * t3; // Goalまでの時間
    if (l3 < 0.01)
        td4 = 60000;
    t4 = (long int)td4;
    pc.printf("t4=%f\r\n", t4);

    Forward(); //移動t3
    pc.printf("Moving\r\n");
    wait_ms(t4);
    Stop();
    pc.printf("Stop 50s\r\n");
    wait_ms(50000);

    pc.printf("Started getting GPS_goal2\r\n");
    while (1)
    {
        if (gps.getgps())
        { //予想Goal地点1でのGPS取得
            g51 = gps.latitude;
            g52 = gps.longitude;
            pc.printf("Complete getting GPS_goal2\r\n");
            pc.printf("g51=%f\r\n", g51);
            pc.printf("g52=%f\r\n", g52);
            break;
        }
    }

    //位置情報検証 地球の赤道半径を6378.137kmとする 1度=111.3195km 1m=0.00000899322度
    //位置情報検証 地球の極半径を6356.752kmとする 1度=110.574km 1m=0.00000902956度  0.00000003634度の誤差

    if (g51 > gg1 - 0.0000899322 && g51 < gg1 + 0.0000899322 && g52 > gg2 - 0.0000902956 && g52 < gg2 + 0.0000902956)
    {
        pc.printf("Complete moving to Goal2\r\n");
        exit(0);
    }
    else
    {
        pc.printf("Didn't reach Goal2. Continue to move to Goal3. Next is the last.\r\n");
    }

    sp41 = g51 - g41; //誤差検出のためのベクトル検出
    pc.printf("sp41=%f\r\n", sp41);
    sp42 = g52 - g42;
    pc.printf("sp42=%f\r\n", sp42);
    l4 = sqrt(pow(sp41, 2.0) + pow(sp42, 2.0));
    pc.printf("l4=%f\r\n", l4);
    cos4 = sp41 / l4;
    pc.printf("cos4=%f\r\n", cos4);
    sin4 = sp42 / l4;
    pc.printf("sin4=%f\r\n", sin4);
    ras4 = asin(sin4);
    pc.printf("ras4=%f\r\n", ras4);
    rac4 = acos(cos4);
    pc.printf("ras4=%f\r\n", ras4);

    if (sin4 >= 0 && cos4 >= 0)
    {
        ra = ras4;
    }
    if (sin4 >= 0 && cos4 < 0)
    {
        ra = rac4;
    }
    if (sin4 < 0)
    {
        ra = 2 * 3.141593 - rac4;
    }

    a_ra(ra);
    a4 = a;
    pc.printf("a4=%f\r\n", a4);

    ea34 = a4 - a3;
    pc.printf("ea34=%f\r\n", ea34);

    wait_ms(5000);

    r34 = (l4 / t4) / (l3 / t3); //土壌地質変化
    pc.printf("r34=%f\r\n", r34);

    sg51 = gg1 - g51; // Goalまでのベクトル検出
    pc.printf("sg51=%f\r\n", sg51);
    sg52 = gg2 - g52;
    pc.printf("sg52=%f\r\n", sg52);

    l5 = sqrt(pow(sg51, 2.0) + pow(sg52, 2.0)); // Goalまでの距離
    pc.printf("l5=%f\r\n", l5);
    cos5 = sg51 / l5;
    pc.printf("cos5=%f\r\n", cos5);
    sin5 = sg52 / l5;
    pc.printf("sin5=%f\r\n", sin5);
    ras5 = asin(sin5);
    pc.printf("ras5=%f\r\n", ras5);
    rac5 = acos(cos5);
    pc.printf("rac5=%f\r\n", rac5);
    if (sin5 >= 0 && cos5 >= 0)
    {
        ra = ras5;
    }
    if (sin5 >= 0 && cos5 < 0)
    {
        ra = rac5;
    }
    if (sin5 < 0)
    {
        ra = 2 * 3.141593 - rac5;
    }

    a_ra(ra);
    a5 = a;
    pc.printf("a5=%f\r\n", a5);

    ca54 = a5 - a4 + ea34; // Goalまでの角度
    pc.printf("ca54=%f\r\n", ca54);

    ttd5 = ca43 / 360 / 15 * t15; // Goal方向までの回転時間
    tt5 = (long int)ttd5;
    pc.printf("tt5=%d\r\n", tt5);

    Turn(); // Goalまでの角度まで回転
    pc.printf("Turning\r\n");
    wait_ms(tt5);
    Stop();
    pc.printf("Stop 5s\r\n");
    wait_ms(5000);

    td5 = l5 / l4 * t4; // Goalまでの時間
    if (l4 < 0.01)
        td5 = 60000;
    t5 = (long int)td5;
    pc.printf("t5=%d\r\n", t5);

    Forward(); //移動t5
    pc.printf("Moving\r\n");
    wait_ms(t5);
    Stop();
    pc.printf("Stop 50s\r\n");
    wait_ms(50000);

    pc.printf("Started getting GPS_goal3\r\n");
    while (1)
    {
        if (gps.getgps())
        { //予想Goal地点1でのGPS取得
            g61 = gps.latitude;
            g62 = gps.longitude;
            pc.printf("Complete getting GPS_goal3\r\n");
            pc.printf("g61=%f\r\n", g61);
            pc.printf("g62=%f\r\n", g62);
            break;
        }
    }

    //位置情報検証 地球の赤道半径を6378.137kmとする 1度=111.3195km 1m=0.00000899322度
    //位置情報検証 地球の極半径を6356.752kmとする 1度=110.574km 1m=0.00000902956度  0.00000003634度の誤差

    if (g61 > gg1 - 0.0000899322 && g61 < gg1 + 0.0000899322 && g62 > gg2 - 0.0000902956 && g62 < gg2 + 0.0000902956)
    {
        pc.printf("Reached Goal3\r\n");
        exit(0);
    }
    else
    {
        pc.printf("Didn't reach Goal3. Finished.\r\n");
    }
    return 0;
}

void Forward(long int x)
{
    Stop();
    moving_timer.reset();
    moving_timer.start();
    wait_ms(1000);
    moving_timer.stop();
    while (x < (moving_timer.read_ms() + 1000))
    {
        moving_timer.start();
        AIN1 = leftwidth;
        AIN2 = 0;
        BIN1 = rightwidth;
        BIN2 = 0;
        wait_ms(100);
        moving_timer.stop();

        if(){
            break;
        }
    }
    Stop();
}

void Backward(long int x)
{
    Stop();
    AIN1 = 0;
    AIN2 = leftwidth;
    BIN1 = 0;
    BIN2 = rightwidth;
    wait_ms(x);
    Stop();
}

void Turn(long int x)
{
    Stop();
    AIN1 = leftwidth;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = rightwidth;
    wait_ms(x);
    Stop();
}
void Stop()
{
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}

double a_ra(double ra)
{
    a = ra * 360 / (2 * 3.141593);
    return a;
}