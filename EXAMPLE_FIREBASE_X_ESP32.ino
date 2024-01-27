#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST "ledesp-6227e-default-rtdb.firebaseio.com" //Without http:// or https:// schemes
#define FIREBASE_AUTH "EF8H2EeV6pRVg0gl0vVVeLVKRKMzXUdythKiiN8Z"
#define WIFI_SSID     "NICA14_WIFI2.4GHz"
#define WIFI_PASSWORD "nicaonline"

//int redpin = D12; //select the pin for the red LED
//int bluepin = D14; // select the pin for the  blue LED
int greenpin = 23;// select the pin for the green LED
FirebaseData firebaseData;
FirebaseData fbdo;
FirebaseJson json;
//String path = "/Pump";

int workk  = 0;

void setup()
{
  //  pinMode(redpin, OUTPUT);
  //  pinMode(bluepin, OUTPUT);
  //  pinMode(greenpin, OUTPUT);
  Serial.begin(115200);
  pinMode(greenpin, OUTPUT);
  digitalWrite(greenpin, LOW);
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

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");

  //optional, set the decimal places for float and double data to be stored in database

  //  Firebase.setFloatDigits(2);
  //  Firebase.setDoubleDigits(6);
  //
  //Serial.println("------------------------------------");
  //Serial.println("Set double test...");

  //  for (uint8_t i = 0; i < 10; i++)
  //  {
  //    //Also can use Firebase.set instead of Firebase.setDouble
  //    if (Firebase.setDouble(fbdo, path + "/Double/Data" + (i + 1), ((i + 1) * 10) + 0.123456789))
  //    {
  //      Serial.println("PASSED");
  //      Serial.println("PATH: " + fbdo.dataPath());
  //      Serial.println("TYPE: " + fbdo.dataType());
  //      Serial.println("ETag: " + fbdo.ETag());
  //      Serial.print("VALUE: ");
  //      printResult(fbdo);
  //      Serial.println("------------------------------------");
  //      Serial.println();
  //    }
  //    else
  //    {
  //      Serial.println("FAILED");
  //      Serial.println("REASON: " + fbdo.errorReason());
  //      Serial.println("------------------------------------");
  //      Serial.println();
  //    }
  //  }
}

void loop()
{
  if (Firebase.getInt(firebaseData, "/path"))
  {
    if (firebaseData.dataType() == "int")
    {
      //      Serial.println(firebaseData.intData());
      workk = firebaseData.intData();
      Serial.println(workk);
    }
  }

  if (workk == 1)
  {
    digitalWrite(greenpin, HIGH);
    Serial.println("มอเตอร์ทำงาน");
    delay(3000);
  }
  if (workk == 0)
  {
    digitalWrite(greenpin, LOW);
    Serial.println("มอเตอร์หยุดทำงาน");
    delay(3000);
  }
}
