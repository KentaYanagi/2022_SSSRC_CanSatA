/*このコードの使い方
1．まずライン75と441にコメントアウト記号(このコメントに使っているもの)を記入する
2．CanSatが20回転する時間(小数点第3位までできれば)をライン24のt20に代入する
3．1．でのコメントアウト記号を外す
4．ライン63と73にコメントアウト記号(このコメントに使っているもの)を記入する
5．さあ実験スタート
試験中に各アクションごとにシリアル通信で問題や動作を表示するのでテラタームで確認する
問題が発生したら柳に連絡するように！
*/



//Motor & GPS
#include "mbed.h"
//only GPS
#include "getGPS.h"
//統合試験
//#include "math.h"
//Exit関数
//#include "stdlib.h"
//Sleep関数 milli second
//#include "windows.h" //これはまず使えない
//この下代入してね！
int t20= 6000 ;
double gg1=135.508111;
double gg2=34.545111;

//only GPS
Serial pc(SERIAL_TX, SERIAL_RX); //試験用

GPS gps(D1, D0);

//only Motor
DigitalOut STBY(D13);
DigitalOut AIN1(A5);
DigitalOut AIN2(A6);
DigitalOut BIN1(A2);
DigitalOut BIN2(A1);

void motorForward(void);
void motorStop(void);
void motorTurn(void);
double a_ra (double);

//地質により変動
double rt= 0.43; //rotate ∏/2 time

double g11,g21,g31,g41,g51,g61,g71,g81,g91; //g1(着地地点） g2(スタート向き計測) g3(中間地点) g4(採取地点1) g5(中間地点) g6(採取地点2) g7(中間地点) g8(採取地点3）　g9(中間地点)
double g12,g22,g32,g42,g52,g62,g72,g82,g92;
double sg31,sg32,sg41,sg42,sg51,sg52,sg61,sg62,sg71,sg72,sg81,sg82;
double sp11, sp12, sp21, sp22, sp31, sp32, sp41, sp42, sp51, sp52, sp61, sp62, sp71, sp72, sp81, sp82, sp91, sp92;
double l1,l2,l3,l4,l5,l6,l7,l8,l9;
double ras1,ras2,ras3,ras4,ras5,ras6,ras7,ras8,ras9;
double rac1,rac2,rac3,rac4,rac5,rac6,rac7,rac8,rac9;
double sin1,sin2,sin3,sin4,sin5,sin6,sin7,sin8,sin9;
double cos1,cos2,cos3,cos4,cos5,cos6,cos7,cos8,cos9;
double ra, a;
double a1,a2,a3,a4,a5,a6,a7,a8,a9;
double ea12, ea23, ea34, ea45, ea56, ea67, ea78, ea89;
double ca32,ca43,ca54,ca65,ca67,ca78,ca89;
int t3,t4,t5,t6,t7,t8,t9;
int tt3,tt4,tt5,tt6,tt7,tt8,tt9;
double r12,r23,r34;
/*
int main () {
    STBY = 1;
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 1;
    wait_ms(t20);
    return 0;
}
*/


int main () {
    
    

    STBY = 1;

        if (gps.getgps()){  //GPSモジュールの機能確認
        motorStop();
        pc.printf("Started getting GPS_first\nWaiting for 5min\n");
        wait_ms(5000);
        }
        else {
        pc.printf("Problem01\n");
        exit(0);
        }
            
        if (gps.getgps()){   //落下地点のGPS取得
        g11=gps.latitude;
        g12=gps.longitude;
        pc.printf("Complete getting GPS\n");
        }
        else {
        
        
        pc.printf("Problem02\n");
        exit(0);
        }
        
        motorForward();      //移動30s
        wait_ms(30000);
        motorStop();
        wait_ms(5000);
        
        if (gps.getgps()) {  //方向認知のためのGPS取得
        g21=gps.latitude;
        g22=gps.longitude;
        pc.printf("Complete getting GPS_turn\n");
        }
        else {
        
        pc.printf("Problem03\n");
        exit(0);
        }
        
                        //方向検知のためのベクトル検知
        sp11=g21-g11; //sp11=self_posture_11 
        sp12=g22-g12;
        l1=sqrt(pow(sp11, 2.0)+pow(sp12, 2.0));
        cos1=sp11/l1;
        sin1=sp12/l1;
        ras1 = asin(sin1);
        rac1 = acos(cos1);
        if(sin1>=0 && cos1>=0){
            ra=ras1;
        }
        if (sin1>=0 && cos1<0){
            ra=rac1;
            }
        if (sin1<0){
            ra=2*3.141593-rac1;
            }

        a_ra(ra);
        
        a1=a;
        
        
        
        motorForward();   //移動30s
        wait_ms(30000);
        motorStop();
        wait_ms(5000);
            
        if (gps.getgps()) {   //誤差検知のためのGPS取得
        g31=gps.latitude;
        g32=gps.longitude;
        pc.printf("Complete getting GPS_recogError\n");
        }
        else {
        pc.printf("Problem04\n");
        exit(0);
        }
        
                            //誤差検知のためのベクトル検出
        sp21=g31-g21; //sp11=self_posture_11
        sp22=g32-g22;
        l2=sqrt(pow(sp21, 2.0)+pow(sp22, 2.0));
        cos2=sp21/l2;
        sin2=sp22/l2;
        ras2 = asin(sin2);
        rac2 = acos(cos2);
        if(sin2>=0 && cos2>=0){
            ra=ras2;
        }
        if (sin2>=0 && cos2<0){
            ra=rac2;
            }
        if (sin2<0){
            ra=2*3.141593-rac1;
            }
        
        a_ra(ra);
        
        a2=a;

        ea12=a2-a1;

        r12=l2/l1;      //土壌地質変化
        pc.printf("r12=%f",r12);

        sg31=gg1-g31;           //Goalまでのベクトル検出
        sg32=gg2-g32;

        l3=sqrt(pow(sp31, 2.0)+pow(sp32, 2.0)); //Goalまでの距離
        cos3=sg31/l3;
        sin3=sg32/l3;
        ras3 = asin(sin3);
        rac3 = acos(cos3);
        if(sin3>=0 && cos3>=0){
            ra=ras3;
        }
        if (sin3>=0 && cos2<0){
            ra=rac3;
            }
        if (sin3<0){
            ra=2*3.141593-rac3;
            }

        a_ra(ra);

        a3=a;

        ca32=a3-a2+ea12;    //Goalまでの角度

        tt3=ca32/360/20*t20; //Goal方向までの回転時間

        motorTurn();        //Goalまでの角度まで回転
        wait_ms(tt3);
        motorStop();


        t3=l3/l2*30000;         //Goalまでの時間

        motorForward();   //移動t3
        wait_ms(t3);
        motorStop();

        wait_ms(5000);
        if(gps.getgps()) {   //予想Goal地点1でのGPS取得
        g41=gps.latitude;
        g42=gps.longitude;
        pc.printf("Complete getting GPS_goal1\n");
        }
        else {
        pc.printf("Problem05\n");
        exit(0);
        }
        
        //位置情報検証 地球の赤道半径を6378.137kmとする 1度=111.3195km 1m=0.00000899322度
        //位置情報検証 地球の極半径を6356.752kmとする 1度=110.574km 1m=0.00000902956度  0.00000003634度の誤差

        if(g41>gg1-0.00000899322 && g41<gg1+0.00000899322 && g42>gg2-0.00000902956 && g42<gg2+0.00000902956){
            pc.printf("Reached Goal\n");
            exit(0);
        }
        else {
        pc.printf("Didn't reach Goal. Continue to move to Goal2\n");
        }  


        sp41=g41-g31;   //誤差検出のためのベクトル検出
        sp42=g42-g32;
        l3=sqrt(pow(sp41, 2.0)+pow(sp42, 2.0));;
        cos3=sp31/l3;
        sin3=sp32/l3;
        ras3 = asin(sin3);
        rac3 = acos(cos3);
        if(sin3>=0 && cos3>=0){
            ra=ras3;
        }
        if (sin3>=0 && cos3<0){
            ra=rac3;
            }
        if (sin3<0){
            ra=2*3.141593-rac3;
            }
        
        a_ra(ra);
        
        a3=a;

        ea23=a3-a2;

        r23=(l3/t3)/(l2/30);      //土壌地質変化
        pc.printf("r23=%f",r23);

        sg41=gg1-g41;           //Goalまでのベクトル検出
        sg42=gg2-g42;

        l4=sqrt(pow(sp41, 2.0)+pow(sp42, 2.0)); //Goalまでの距離
        cos4=sg41/l4;
        sin4=sg42/l4;
        ras4 = asin(sin4);
        rac4 = acos(cos4);
        if(sin4>=0 && cos4>=0){
            ra=ras4;
        }
        if (sin4>=0 && cos4<0){
            ra=rac4;
            }
        if (sin4<0){
            ra=2*3.141593-rac4;
            }

        a_ra(ra);

        a4=a;

        ca43=a4-a3+ea23;    //Goalまでの角度

        tt4=ca32/360/20*t20; //Goal方向までの回転時間

        motorTurn();        //Goalまでの角度まで回転
        wait_ms(tt4);
        motorStop();


        t4=l4/l3*t3;         //Goalまでの時間

        motorForward();   //移動t3
        wait_ms(t4);
        motorStop();

        wait_ms(5000);
        if(gps.getgps()) {   //予想Goal地点1でのGPS取得
        g51=gps.latitude;
        g52=gps.longitude;
        pc.printf("Complete getting GPS_goal2\n");
        }
        else {
        pc.printf("Problem06\n");
        exit(0);
        }
        
        //位置情報検証 地球の赤道半径を6378.137kmとする 1度=111.3195km 1m=0.00000899322度
        //位置情報検証 地球の極半径を6356.752kmとする 1度=110.574km 1m=0.00000902956度  0.00000003634度の誤差

        if(g51>gg1-0.00000899322 && g51<gg1+0.00000899322 && g52>gg2-0.00000902956 && g52<gg2+0.00000902956){
            pc.printf("Complete moving to Goal2\n");
            exit(0);
        }
        else {
        pc.printf("Didn't reach Goal2. Continue to move to Goal3. Next is the last.\n");
        }
        
        sp41=g51-g41;   //誤差検出のためのベクトル検出
        sp42=g52-g42;
        l4=sqrt(pow(sp41, 2.0)+pow(sp42, 2.0));
        cos4=sp41/l4;
        sin4=sp42/l4;
        ras4 = asin(sin4);
        rac4 = acos(cos4);
        if(sin4>=0 && cos4>=0){
            ra=ras4;
        }
        if (sin4>=0 && cos4<0){
            ra=rac4;
            }
        if (sin4<0){
            ra=2*3.141593-rac4;
            }
        
        a_ra(ra);
        
        a4=a;

        ea34=a4-a3;

        r34=(l4/t4)/(l3/t3);      //土壌地質変化
        pc.printf("r34=%f",r34);

        sg51=gg1-g51;           //Goalまでのベクトル検出
        sg52=gg2-g52;

        l5=sqrt(pow(sp51, 2.0)+pow(sp52, 2.0));//Goalまでの距離
        cos5=sg51/l5;
        sin5=sg52/l5;
        ras5 = asin(sin5);
        rac5 = acos(cos5);
        if(sin5>=0 && cos5>=0){
            ra=ras5;
        }
        if (sin5>=0 && cos5<0){
            ra=rac5;
            }
        if (sin5<0){
            ra=2*3.141593-rac5;
            }

        a_ra(ra);

        a5=a;

        ca54=a5-a4+ea34;    //Goalまでの角度

        tt5=ca43/360/20*t20; //Goal方向までの回転時間

        motorTurn();        //Goalまでの角度まで回転
        wait_ms(tt5);
        motorStop();


        t5=l5/l4*t4;         //Goalまでの時間

        motorForward();   //移動t3
        wait_ms(t5);
        motorStop();

        wait_ms(5000);
        if(gps.getgps()) {   //予想Goal地点1でのGPS取得
        g61=gps.latitude;
        g62=gps.longitude;
        pc.printf("Complete getting GPS_goal3\n");
        }
        else {
        pc.printf("Problem07\n");
        exit(0);
        }
        
        //位置情報検証 地球の赤道半径を6378.137kmとする 1度=111.3195km 1m=0.00000899322度
        //位置情報検証 地球の極半径を6356.752kmとする 1度=110.574km 1m=0.00000902956度  0.00000003634度の誤差

        if(g61>gg1-0.00000899322 && g61<gg1+0.00000899322 && g62>gg2-0.00000902956 && g62<gg2+0.00000902956){
            pc.printf("Reached Goal3\n");
            exit(0);
        }
        else {
        pc.printf("Didn't reach Goal3. Finished.\n");
        }
    return 0;
}

void motorForward() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 1;
    BIN2 = 0;
}

void motorTurn() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 1;
}
void motorStop() {
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}

double a_ra (double ra){
    a=ra*360/(2*3.141593);
    return a;
    }
    
    

/*
int main()
{
    //only Motor
    STBY = 1;
    //only GPS
    
    pc.printf("\r\n\GPS Start\r\n");　 //試験用
    
    double g11,g21,g31,g41,g51,g61,g71,g81,g91; //g1(着地地点） g2(スタート向き計測) g3(中間地点) g4(採取地点1) g5(中間地点) g6(採取地点2) g7(中間地点) g8(採取地点3）　g9(中間地点)
    double g12,g22,g32,g42,g52,g62,g72,g82,g92;
    //1(始発終着地点）＋3(採取地点)+4(中間地点)+1(向き計測地点)=9
    
    double time = 41.67
    // 35/2/(21*0.3/15)=41.666666666667
    
    /* 1秒ごとに現在地を取得してターミナル出力 */
    /*while(1) {
        if(gps.getgps()) //現在地取得
            pc.printf("(%lf, %lf)\r\n", gps.latitude, gps.longitude);//緯度と経度を出力
        
        else
            pc.printf("No data\r\n");//データ取得に失敗した場合
        
        wait(1);
    }
*/
/*
    while(1) {
        if (gos.getgps()){
            motorStop();
            pc.printf("位置情報データ取得開始\n5秒間待機中")
            wait(5);
        else {
            pc.printf("問題発生01");
            break;
            }
            if (gps.getgps()){
            
                g11=gps.latitude;
                g12=gps.longitude;
        　　　　　　　　    pc.printf("落下地点取得完了");
                motorForward();
        　　　　　　　　    wait(2.0);
                motorStop();
                wait(5);
                }
                else {
            pc.printf("問題発生02");
            break;
            }
            if (gps.getgps())
            g21=gps.latitude;
            g22=gps.longitude;
            pc.printf("落下地点取得完了");
            sp11=g21-g11; //sp11=self_posture_11
            sp12=g22-g12;
            
            l01=(sp11^2+sp12^2)^(1/2);
            
            sp21=g41-g21;
            sp22=g42-g22;
            
                    
            
            
            
            wait(41.7) //timeより
            
            
            
            
            g21=gps.latitude;
            g22=gps.longitude;
            
            g31=gps.latitude;
            g32=gps.longitude;
            
            g41=gps.latitude;
            g42=gps.longitude;
            
            g51=gps.latitude;
            g52=gps.longitude;
            
            g61=gps.latitude;
            g71=gps.latitude;
            g81=gps.latitude;
            g91=gps.latitude;
            
    return 0;
}

void motorFoward() {
    motorStop();
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
}
void motorLeft() {
    motorStop();
    AIN1 = 1;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 1;
}
void motorStop() {
    AIN1 = 0;
    AIN2 = 0;
    BIN1 = 0;
    BIN2 = 0;
}

double get_angle(double x,double y){


*/