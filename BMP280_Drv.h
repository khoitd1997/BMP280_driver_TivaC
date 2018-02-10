#ifndef _BMP280_DRV_
#define _BMP280_DRV_

//enum of all the sensors' settings
enum comProtocol {SPI, I2C};

//measurement settings based on the datasheet of BMP280
enum measureSettings{Custom, handLow, handDynamic, weatherStat, elevDetec, dropDetec, indoorNav};
enum operMode{Sleep, Forced, Normal};

//use log for array indexing in oversampling


typedef struct bmp280Sensor bmp280;

//represent one bmp280 sensor along with its settings
struct bmp280Sensor 
{

};

//initialize a bmp280 struct
//will be use in a big switch statement for initializing based on
//both predefined option and customizable value 
struct bmp280Sensor bmp280Init();

#endif