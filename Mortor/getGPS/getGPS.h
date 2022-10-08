#ifndef GPS_H
#define GPS_H

#define GPSBAUD 9600 //ボーレート

class GPS {
public:
    GPS(PinName gpstx,PinName gpsrx);
    
    bool getgps();

    double longitude;
    double latitude;
    
private:
    Serial _gps;
};
 
#endif //GPS_H