#include "mbed.h"
#include "getGPS.h"

GPS::GPS(PinName gpstx,PinName gpsrx): _gps(gpstx,gpsrx)
{
    latitude = 0;
    longitude = 0;
    _gps.baud(GPSBAUD);
    _gps.printf("$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");
}

bool GPS::getgps()
{
    char gps_data[256];
    int i;
    
    do {
        while(_gps.getc() != '$'); //$マークまで読み飛ばし
        i = 0;

        /* gpa_data初期化 */
        for(int j = 0; j < 256; j++)
            gps_data[j] = '\0';

        /* NMEAから一行読み込み */
        while((gps_data[i] = _gps.getc()) != '\r') {
            i++;
            if(i == 256) {
                i = 255;
                break;
            }
        }
    } while(strstr(gps_data, "GPGGA") == NULL); //GGAセンテンスまで一行ずつ読み込み続ける
    
    int rlock;
    char ns,ew;
    double w_time, raw_longitude, raw_latitude;
    int satnum;
    double hdop;

    if(sscanf(gps_data, "GPGGA, %lf, %lf, %c, %lf, %c, %d, %d, %lf", &w_time, &raw_latitude, &ns, &raw_longitude, &ew, &rlock, &satnum, &hdop) > 1) {
        /* 座標1（度部分） */
        int latitude_dd = (int)(raw_latitude / 100);
        int longitude_dd = (int)(raw_longitude / 100);

        /* 座標2（分部分 → 度） */
        double latitude_md = (raw_latitude - latitude_dd * 100) / 60;
        double longitude_md = (raw_longitude - longitude_dd * 100) / 60;

        /* 座標1 + 2 */
        latitude = latitude_dd + latitude_md;
        longitude = longitude_dd + longitude_md;

        return true;
    } else
        return false; //GGAセンテンスの情報が欠けている時
}
