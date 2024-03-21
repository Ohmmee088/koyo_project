#include "DHT.h"           
#define DHTPIN D4 
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);
#include <Wire.h>
#include <TridentTD_LineNotify.h>
int cnt = 0;          
int timezone = 7 * 3600; 
int dst = 0; 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>   
#include <AutoConnect.h>     
ESP8266WebServer Server(80);
AutoConnect Portal(Server);
WiFiClient Client1;
#include "FirebaseESP8266.h"
#include <MMA8453_n0m1.h>
#define FIREBASE_HOST "ledesp-6227e-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "EF8H2EeV6pRVg0gl0vVVeLVKRKMzXUdythKiiN8Z"
//#define LINE_TOKEN  "EiF3ikaC2tDdF5EnHLX1sjjUcmOZQ3PDBx5RaDbljKj"  
#define LINE_TOKEN  "akW5zZnKVTXBeuoftRWObreI2NHlnR267rGKh5N5vEc" 
#include <SPI.h>
#include "ThingSpeak.h" 
const char* host = "api.thingspeak.com";
const char* api = "BDS9QPU851TOQL6K";   
//const char* api = "CWY2SDYHKQPMH5A1";     //n
int path;
String url;
MMA8453_n0m1 accel;  
FirebaseData firebaseData;
FirebaseJson json;
String Text_String1 = "ชั่วโมงที่ผ่านมามอเตอร์ทำงาน ";  
String Text_String2 = " นาที";  
String Print_String;  
int som1 = 1 ;
int x1 = 1 ;
int x0 = 0 ;
int AA = 20 ;
int counter01 , counter02 , counter03, startofwork = 0;
float rx , ry, rz , xx , yy , zz ;
int SSR01 = D7; 
int SSR02 = D8; 
int Lon = -4 ;
int Hon =  4 ;
int Loff = -7 ;
int Hoff =  7 ;
int ui;
int cycle0 = 5; 
int cycle1 = 5; 
int cycle2 = 5; /
void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}
void setup()
{ 
  Serial.begin(115200);
  Wire.begin();
  pinMode(SSR01, OUTPUT);   digitalWrite(SSR01, LOW);
  pinMode(SSR02, OUTPUT);   digitalWrite(SSR02, LOW);
  Serial.println(LINE.getVersion());   
  LINE.setToken(LINE_TOKEN);
  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
  Serial.setDebugOutput(true);  //สือสารntp
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov"); //ดึงเวลาจาก Server
  Serial.println("\nLoading time");
  while (!time(nullptr)) {
  Serial.print("*");
  delay(1000);
  }
  Serial.println("");
  accel.setI2CAddr(0x1C); //change your device address if necessary, default is 0x1C  D1=SCL     D2=SDA
  accel.dataMode(true, 2); //enable highRes 10bit, 2g range [2g,4g,8g]
  Serial.println("XYZ Data Example");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}
void loop()
{
  Portal.handleClient();
  time_t now = time(nullptr);   
  struct tm* p_tm = localtime(&now);
  Serial.print(p_tm->tm_hour);
    Serial.print(':');
  Serial.print(p_tm->tm_min);
    Serial.print(':');
  Serial.println(p_tm->tm_sec);
  int hour = p_tm->tm_hour;
  if ( (p_tm->tm_hour == 1 && p_tm->tm_min == 0 ) 
    || (p_tm->tm_hour == 8 && p_tm->tm_min == 0) 
    || (p_tm->tm_hour == 16 && p_tm->tm_min == 0) )
  {
    Serial.println("ESP_Reset..");
    ESP.restart();
  }
  Serial.println(F("DHTxx test!"));
  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F(" C "));
  Serial.print(f);
  Serial.print(F(" F  Heat index: "));
  Serial.print(hic);
  Serial.print(F(" C "));
  Serial.print(hif);
  Serial.println(F(" F"));
  accel.update();
  ui = Firebase.getInt(firebaseData, "path");
  zz = accel.z();
  rz = zz * 0.3214285714285714 ;
  Serial.print(" z: ");
  Serial.println(rz);
  String LineText0;    
  String LineText1;     
  String string1 ="มอเตอร์ทำงาน";
  String string2 ="มอเตอร์หยุดทำงาน";
  LineText0 = string1 ;
  LineText1 = string2 ;
  if( rz > Lon && rz < Hon )
  {
    counter02  = 0;
    counter01++ ;
    if ( counter01 >= cycle0 )  
    {
      Serial.print("มอเตอร์ทำงาน");
      digitalWrite(SSR01, HIGH);                        
      digitalWrite(SSR02, HIGH);                         
      delay (1000);                                      
      counter01  = 0;
      if (som1 >= 1)
      {
        Serial.println(" ");
        LINE.notify(LineText0);                          
        Serial.print("LINE ส่งค่า สถานะทำงาน"); 
        Firebase.setInt(firebaseData, "path",  x1);     
        Serial.println("firebaseDataส่งค่า path = 1");
        path = 1;
        som1 = 0;
      }
    }
  }
  if (rz > (-90) && rz < Loff || rz > Hoff && rz < 90 )           
  {
    counter01  = 0;
    counter02++;
    if ( counter02 >= cycle1 )
    { 
      digitalWrite(SSR01, LOW);                               
      digitalWrite(SSR02, LOW);                          
      Serial.print("มอเตอร์ หยุด ทำงาน ");                       
      delay (1000);                                      
      counter02  = 0;
      if (som1 <= 0)
      {
        Serial.println(" ");
        LINE.notify(LineText1);                
        Serial.print("LINE ส่งค่า หยุด ทำงาน");
        Firebase.setInt(firebaseData, "path",  x0);      
        Serial.println("firebaseDataส่งค่า path = 0");
        som1 = 1;
      }
    }
  }
  if (counter03 >= cycle2 )    
  {
    Firebase.setInt(firebaseData, "xyz",  rz);
    Serial.println("firebaseDataส่งค่าแกนZ");
    counter03 = 0;
  }
  counter03++;                                           
  Serial.println("");
  Serial.print("รอบการทำงาน = ");  Serial.print(startofwork);
  Serial.print("  counter01 โหมดทำงาน ="); Serial.print(counter01);
  Serial.print("  counter02 โหมดไม่ทำงาน ="); Serial.print(counter02);
  Serial.print("  counter03 =");  Serial.println(counter03);
  const int httpPort = 80;
  if (!Client1.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;}
  url = "/update?api_key=" ;
  url += api ;
  url += "&field1=" ;
  url += path;
  url += "&field2=" ;  
  url += rz ;
  url += "&field3=" ;  
  url += t ;
  url += "&field4=" ;  
  url += h ;
  url += "&field5=" ;  
  url += f ;
  url += "&field6=" ;  
  url += cnt ;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  Client1.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  while(Client1.available()){
    String line = Client1.readStringUntil('\r');
    Serial.print(line);
  }
  //Serial.println();
  //Serial.println("End of 1 work cycle");
  cnt++;
  Serial.print("cnt = "); Serial.println(cnt); Serial.println("");
  delay(5000);
}



