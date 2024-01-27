#include <TridentTD_LineNotify.h>                                   //เรียกใช้ไลบราลี่ <TridentTD_LineNotify.h>  
#define SSID        "NICA14_WIFI2.4GHz"                                      // ชื่อ Wifi ที่จะเชื่อมต่อ
#define PASSWORD    "nicaonline"                                      //  รหัส Wifiที่จะเชื่อมต่อ
#define LINE_TOKEN  "j4TLbsrt8GHLAy2azq0OZFNPwAxYKMVY29sP6Nf5B5n"  // บรรทัดที่ 13 ใส่ รหัส TOKEN ที่ได้มาจากข้างบน
//#define LINE_TOKEN  "MT62XKfEFnq7LGpMdBHxWdfKMMDSMnNsn1CP3FQKao6"
volatile int NbTopsFan;                                            //การวัดขอบที่เพิ่มขึ้นของสัญญาณ
int Calc;                                                          //กำหนดค่า Calc เป็นตัวแปร int
int hallsensor = 5;                                                //กำหนดขาการทำงานของเซนเซอร์เป็นขาที่5
unsigned int abnormal=0 ;
unsigned int normal=0 ;

void rpm ()                                                         // ฟังก์ชั้นย่อยของโปรแกรมชื่อ rpm
{
NbTopsFan++;                                                        //This function measures the rising and falling edge of the hall effect sensors signal
}

void setup()                                                      // ฟังก์ชั้นส่วนต้นของโปรแกรม
{
  Serial.begin(115200); Serial.println();                         // ตั้งความเร็วสื่อสารที่ 115200
  Serial.println(LINE.getVersion());                              //แสดงเวอร์ชั่นของไลน์
  WiFi.begin(SSID, PASSWORD);                                     //เข้ารหัส wifi
  Serial.printf("WiFi connecting to %s\n",  SSID);                //แสดงข้อความ "WiFi connecting to %s\n" ในซีเรียลมอนิเตอร์
  while (WiFi.status() != WL_CONNECTED)                           //แสดงข้อความ "WL_CONNECTED" ในซีเรียลมอนิเตอร์เมื่อมีการเชื่อมต่ออินเตอร์เน็ต        
  {
    Serial.print(".");                                            //แสดงข้อความ " . " ในซีเรียลมอนิเตอร์
    delay(400);                                                   // หน่วงเวลา 0.4 วินาที
  }

  Serial.printf("\nWiFi connected\nIP : ");                       //แสดงข้อความ "nWiFi connected\nIP : " ในซีเรียลมอนิเตอร์
  Serial.println(WiFi.localIP());                                 //แสดง local IP เชื่อมต่ออินเตอร์เน็ตในซีเรียลมอนิเตอร์
  LINE.setToken(LINE_TOKEN);                                  // กำหนด Line Token
  Serial.print("ok");                                         //แสดงข้อความ "ok" ในซีเรียลมอนิเตอร์
 
pinMode(hallsensor, INPUT);                                   //เซตขาที่ 5 เป็นขาอินพุด
attachInterrupt(5, rpm, RISING);                              //and the interrupt is attached
  pinMode(13, OUTPUT);                                        // เซตขาที่ 13 เป็นขาเอ้าพุด
  pinMode(12, OUTPUT);                                        // เซตขาที่ 12 เป็นขาเอ้าพุด

}
 void loop ()
{
NbTopsFan = 0;                                                //Set NbTops to 0 ready for calculations
//sei();                                                        //Enables interrupts
delay (1000);                                                 //Wait 1 second
//cli();                                                        //Disable interrupts
Calc = 0;//(NbTopsFan * 60 / 7.5);                                // คำนวนค่า cale
Serial.print (Calc, DEC);                                     //แสดงค่า calc ในซีเรียลมอนิเตอร์
Serial.println (" m/s \r");
   
 if(Calc < 10)                                                //เงือนไข ถ้าค่า Calc > 30 ให้ทำใน {}
  {
     if(abnormal < 1 )   {                                              
     LINE.notify(" No.3_Finfish1_Error ");                    // ส่งข้อความแจ้งเตือน "ระบบลมในบ่อเลี้ยงมีปัญหา" ไปยัง LINE
      delay (500);                                            // หน่วงเวลา 0.5 วินาที
      normal = 10 ; }

    if(abnormal > 300 )   {                                            
   
     LINE.notify(" No.3_Finfish1_Error ");                    // ส่งข้อความแจ้งเตือน "ระบบลมในบ่อเลี้ยงมีปัญหา" ไปยัง LINE
      delay (500);                                            // หน่วงเวลา 0.5 วินาที
      abnormal = 1 ;
      normal = 10 ;
      }
     
     abnormal++;
     Serial.println (" off ");                                //แสดงค่า calc ในซีเรียลมอนิเตอร์
     digitalWrite(13, HIGH);                                  // สั่งให้ลีเลย์ตัว 2 ทำงาน
     digitalWrite(12, HIGH);                                  // สั่งให้ลีเลย์ตัว 1 ทำงาน
     Serial.print ("abnormal=");
     Serial.println (abnormal);
     Serial.print ("normal=");
     Serial.println (normal);
     
      }
 
  else                                                        //เงือนไข ถ้าค่า Calc < 30 ให้ทำใน {}
  {

if(normal > 9 )  {                                          
     abnormal = 0 ;
     normal = 0 ;
     LINE.notify(" No.3_Finfish1_Normal ");                    // ส่งข้อความแจ้งเตือน "ระบบลมในบ่อเลี้ยงมีปัญหา" ไปยัง LINE
     delay (500);   }                                         // หน่วงเวลา 0.5 วินาที
 
   
   Serial.println (" on ");
    delay (500);
   digitalWrite(13, LOW);                                    // สั่งให้ลีเลย์ตัว 2 ไม่ทำงาน
   digitalWrite(12, LOW);                                    // สั่งให้ลีเลย์ตัว 1 ไม่ทำงาน
    //normal++ ;
    abnormal = 0 ;
    normal = 0 ;
  }
}