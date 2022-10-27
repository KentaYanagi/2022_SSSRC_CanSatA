#include "mbed.h"

Serial pc(USBTX, USBRX);
DigitalIn FlightPin(D12);
DigitalOut Nichrome(D2);
int main(){
  pc.printf("フライトピンの状態を表示します\r\n");
    pc.printf("OFF\r\n");
    while(1){
        if(FlightPin.read() == 1){
            pc.printf("ON\r\n");
            break;
        }
    }
  //  wait(60);

    pc.printf("start\r\n");
    Nichrome = 1;
    for(int i=1;i<10;i++){
        wait(1);
        pc.printf("%d second\r\n",i);
    }
    Nichrome = 0;
    pc.printf("end\r\n");
    }