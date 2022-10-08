/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "getGPS.h"

Serial pc(SERIAL_TX, SERIAL_RX);
GPS gps(D1, D0);

int main ()
{
    pc.printf("\r\n\GPS Start\r\n");
    
    //0.1秒ごとに現在地取得　
    while(1) {
        if(gps.getgps())
            pc.printf("(%lf, %lf)\r\n", gps.latitude, gps.longitude);
        
        else
            pc.printf("No Date\r\n"); //データ取得に失敗
        
        wait(0.1);
    }
}
