#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);


#include <Wire.h>
#include "RTClib.h"
void saatAl();
RTC_DS1307 rtc;



#include <MapFloat.h>
#include <SPI.h>
#include <MPU9250.h>
#define SPI_CLOCK 8000000  // 8MHz clock works.
#define SS_PIN   10
#define INT_PIN  3
#define LED      13
#define WAITFORINPUT(){            \
    while(!Serial.available()){};  \
    while(Serial.available()){     \
      Serial.read();             \
    };                             \
  }                                  \

  MPU9250 mpu(SPI_CLOCK, SS_PIN);
  float  x, y, z;
  void imuAl();



  String telemetryNano = "";

  void readSerialPort();
  String data;


#include <Servo.h>

  byte servoPin = 9;
  Servo servo;

  void setup()
  {
    Serial.begin(115200);

    ss.begin(GPSBaud);

    ///////////////////////////////////////////////////////////////////////////////
    // RTC SETUP
    //while (!Serial);
    if (! rtc.begin())
    {
      //    Serial.println("Couldn't find RTC");
      // while (1);
    }
    if (! rtc.isrunning())
    {
      //    Serial.println("RTC is NOT running!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }



    /////////////////////////////////////////////////////////////////
    //  IMU SETUP
    pinMode(INT_PIN, INPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    SPI.begin();

    //  Serial.println("Press any key to continue");
    //  WAITFORINPUT();

    mpu.init(true);

    uint8_t wai = mpu.whoami();
    if (wai == 0x71) {
      // Serial.println("Successful connection");
    }
    else {
      Serial.print("Failed connection: ");
      Serial.println(wai, HEX);
    }

    uint8_t wai_AK8963 = mpu.AK8963_whoami();
    if (wai_AK8963 == 0x48) {
      //Serial.println("Successf,zul connection to mag");
    }
    else {
      // Serial.print("Failed connection to mag: ");
      Serial.println(wai_AK8963, HEX);
    }

    mpu.calib_acc();
    mpu.calib_mag();

    //  Serial.println("Send any char to begin main loop.");
    //  WAITFORINPUT();

    servo.attach(servoPin);

    servo.writeMicroseconds(1100); // send "stop" signal to ESC.



  }

  void loop()
  {

    if (Serial.available())
    {
      servo.writeMicroseconds(1200); // Send signal to ESC.
    }


    saatAl();
    imuAl();

    //printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    //printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);

    telemetryNano = telemetryNano + gps.location.lat() + "," + gps.location.lng();

    smartDelay(1000);
    // if (millis() > 5000 && gps.charsProcessed() < 10)
    //Serial.println(F("No GPS data received: check wiring"));







    Serial.println(telemetryNano);

    telemetryNano = "";
    delay(500);
  }


  static void smartDelay(unsigned long ms)
  {
    unsigned long start = millis();
    do
    {
      while (ss.available())
        gps.encode(ss.read());
    } while (millis() - start < ms);
  }

  static void printFloat(float val, bool valid, int len, int prec)
  {
    if (!valid)
    {
      //while (len-- > 1)
      Serial.print('-');
      Serial.print(' ');
    }
    else
    {
      //  Serial.print(val, prec);
      // telemetryNano = telemetryNano + "," + val + "," + prec;
      int vi = abs((int)val);
      int flen = prec + (val < 0.0 ? 2 : 1); // . and -
      flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
      for (int i = flen; i < len; ++i)
        Serial.print(' ');
    }
    smartDelay(0);
  }
  static void printStr(const char *str, int len)
  {
    int slen = strlen(str);
    for (int i = 0; i < len; ++i)
      Serial.print(i < slen ? str[i] : ' ');
    smartDelay(0);
  }





  void saatAl()
  {

    DateTime now = rtc.now();
    //    Serial.print(now.year(), DEC);
    //    Serial.print('/');
    //    Serial.print(now.month(), DEC);
    //    Serial.print('/');
    //    Serial.print(now.day(), DEC);
    //    Serial.print(' ');
    //    Serial.print(now.hour(), DEC);
    //    Serial.print(':');
    //    Serial.print(now.minute(), DEC);
    //    Serial.print(':');
    //    Serial.print(now.second(), DEC);
    telemetryNano = telemetryNano + now.year() + "/" + now.month() +  "/" + now.day() + " " + now.hour() + ":" +  now.minute() + ":" + now.second();
  }



  void imuAl()
  {
    mpu.read_all();
    x = mapFloat(mpu.accel_data[0], -1, 1, 0, 180);
    y = mapFloat(mpu.accel_data[1], -1, 1, 0, 180);
    z = mapFloat(mpu.accel_data[2], -1, 1, 0, 180);
    Serial.print(" ");
    // Serial.print(x);  Serial.print(' ');
    // Serial.print(y);  Serial.print(' ');
    // Serial.print(z);  Serial.print(' ');
    telemetryNano = telemetryNano + "," + x + "," + y + "," + z + ",";
  }
