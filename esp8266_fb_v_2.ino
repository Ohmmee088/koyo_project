 ////FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <MMA8453_n0m1.h>
#include <TridentTD_LineNotify.h>                                   //เรียกใช้ไลบราลี่ <TridentTD_LineNotify.h>  

#define FIREBASE_HOST "ledesp-6227e-default-rtdb.firebaseio.com" //Without http:// or https:// schemes
#define FIREBASE_AUTH "EF8H2EeV6pRVg0gl0vVVeLVKRKMzXUdythKiiN8Z"
#define WIFI_SSID "NICA14_WIFI2.4GHz"
#define WIFI_PASSWORD "nicaonline"
//#define WIFI_SSID "Homestay_2.4GHz"
//#define WIFI_PASSWORD "0825982645"
#define LINE_TOKEN  "EiF3ikaC2tDdF5EnHLX1sjjUcmOZQ3PDBx5RaDbljKj"  // บรรทัดที่ 13 ใส่ รหัส TOKEN ที่ได้มาจากข้างบน

MMA8453_n0m1 accel;
//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseJson json;

String Text_String1 = "ชั่วโมงที่ผ่านมามอเตอร์ทำงาน ";  // สร้างตัวแปรชื่อ Num_String1 ชนิด String (ชุดอักขระ)
String Text_String2 = " นาที";  // สร้างตัวแปรชื่อ Num_String1 ชนิด String (ชุดอักขระ)
String Print_String;  // สร้างตัวแปรชื่อ Print_String ชนิด String (ชุดอักขระ)
int somm = 0 ;
int x1 = 1 ;
int x0 = 0 ;
int AA = 20 ;
int counter01 , counter02 , counter03, startofwork ;
float rx , ry, rz , xx , yy , zz ;
int SSR01 = D7; // ขา D7
int SSR02 = D8; // ขา D8

int Lon = -4 ;
int Hon =  4 ;

int Loff = -7 ;
int Hoff =  7 ;

void printResult(FirebaseData &data);

void setup()
{
  pinMode(SSR01, OUTPUT);   digitalWrite(SSR01, LOW);
  pinMode(SSR02, OUTPUT);   digitalWrite(SSR02, LOW);
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  LINE.setToken(LINE_TOKEN);
  accel.setI2CAddr(0x1C); //change your device address if necessary, default is 0x1C  D1=SCL     D2=SDA
  accel.dataMode(true, 2); //enable highRes 10bit, 2g range [2g,4g,8g]
  Serial.println("XYZ Data Example");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);


  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  /*
    This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
    Firewall that allows only GET and POST requests.

    Firebase.enableClassicRequest(firebaseData, true);
  */


}

//////////////////////////////////////////////
void loop()
{
  digitalWrite(SSR02, LOW);
  delay (3300);
  accel.update();
  digitalWrite(SSR02, HIGH);

  /*  xx = accel.x();
    rx = xx * 0.3214285714285714 ;
    Serial.print("x: ");
    Serial.print(rx);
    yy = accel.y();
    ry = yy * 0.3214285714285714 ;
    Serial.print(" y: ");
    Serial.print(ry);      */

  zz = accel.z();
  rz = zz * 0.3214285714285714 ;
  Serial.print(" z: ");
  Serial.println(rz);
  delay(3300);
  digitalWrite(SSR02, LOW);

  if ( rz > Lon && rz < Hon )                          //เงื่อนไขเริ่มทำงาน เทียบกับแก่น XYZ แก้ขอบเขตการทำงานของเครื่อง ค่าต้องมากกว่าหรือเท่ากับ 5 และต้องน้อยกว่าหรือเท่ากับ 15
  {
    counter02  = 0;
    counter01++;
    if ( counter01 > 20 )  //5นาที
    {
      Serial.print("มอเตอร์ทำงาน ");
      digitalWrite(SSR01, HIGH);
      delay (500);                                      // หน่วงเวลา 2.5 วินาที
      counter01  = 0;

      if (somm < 1)
      {
        Serial.println(" ");
        LINE.notify("มอเตอร์ทำงาน ");
        Serial.print("LINE ส่งค่า สถานะทำงาน");
        Firebase.setInt(firebaseData, "path",  x1);
        Serial.println("firebaseDataส่งค่า H/L");

        somm = 2;
      }
    }
  }

  if (rz > (-90) && rz < Loff || rz > Hoff && rz < 90    )             //เงื่อนไขหยุดทำงาน เทียบกับแก่น XYZ แก้ขอบเขตการทำงานของเครื่อง ค่าต้องมากกว่าหรือเท่ากับ 5 และต้องน้อยกว่าหรือเท่ากับ 15
  {
    counter01  = 0;
    counter02++;
    if ( counter02 > 20 )
    {
      digitalWrite(SSR01, LOW);
      Serial.print("มอเตอร์ หยุด ทำงาน     ");
      delay (500);                                            // หน่วงเวลา 2.5 วินาที
      counter02  = 0;

      if (somm > 1)
      {
        Serial.println(" ");
        LINE.notify("มอเตอร์ หยุด ทำงาน ");
        Serial.print("LINE ส่งค่า หยุด ทำงาน");
        Firebase.setInt(firebaseData, "path",  x0);
        Serial.println("firebaseDataส่งค่า LOW");

        somm = 0;
      }
    }
  }

  if (counter03 > 4 )    ///  ค่า 4 = 1 นาที
  {
     Firebase.setInt(firebaseData, "xyz",  rz);
    Serial.println("firebaseDataส่งค่าแกนZ");
    counter03 = 0;
  }

  delay (3300);
  digitalWrite(SSR02, HIGH);
  counter03++;
  Serial.println("  ");
  Serial.print("รอบการทำงาน =");      Serial.print(startofwork);
  Serial.print("  counter01 โหมดทำงาน =");      Serial.print(counter01);
  Serial.print("  counter02 โหมดไม่ทำงาน =");  Serial.print(counter02);
  Serial.print("  counter03 =");              Serial.println(counter03);
}
