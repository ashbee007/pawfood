#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <string.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Its Bees"
#define WIFI_PASSWORD "itsbeesworld"
#define API_KEY "AIzaSyB4jigM_XgDYBtpPTItja0lTIq3eN-orew"
#define DATABASE_URL "https://pawfud-84419-default-rtdb.firebaseio.com/" 


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int feed,water,cnt;
bool autom;
String timeinp;
char realtime[10],giventime[10];

Servo a,b;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup(){
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(300);
  }

  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());


  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.signUp(&config, &auth, "", "");
  config.token_status_callback = tokenStatusCallback;   
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  Firebase.RTDB.setInt(&fbdo, "m", 0);
  Firebase.RTDB.setInt(&fbdo, "n", 0);

  timeClient.begin();
  timeClient.setTimeOffset(19800);
}

void loop(){
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  for(int i=0; i<5;i++)
  {
    realtime[i]=formattedTime[i];
  }
  Serial.println(realtime); 
  
  Firebase.RTDB.getInt(&fbdo, "/m");
  feed = fbdo.intData();
  Firebase.RTDB.getInt(&fbdo, "/n");
  water = fbdo.intData(); 
   
  Firebase.RTDB.get(&fbdo, "/time");
  timeinp = fbdo.stringData();
  for(int i=0; i<5;i++)
  {
    giventime[i]=timeinp[i];
  }
  Serial.println(giventime);
  Firebase.RTDB.get(&fbdo, "/automation");
  autom = fbdo.boolData();

  Firebase.RTDB.getInt(&fbdo, "/cnt");
  cnt = fbdo.intData();

  if(autom == true && strcmp(realtime,giventime)==0)
  {
      Serial.println("Fed");
      a.attach(5);
      a.write(180);
      delay(2000);
      a.detach();
      Firebase.RTDB.setInt(&fbdo, "m", 0);
      Firebase.RTDB.setInt(&fbdo, "cnt", cnt-1);
  }
  
    if(feed==1)
    {
      Serial.println("Fed");
      a.attach(5);
      a.write(180);
      delay(600);
      a.detach();
      Firebase.RTDB.setInt(&fbdo, "m", 0);
      Firebase.RTDB.setInt(&fbdo, "cnt", cnt-1);
    }
    if(water==1)
    {
      Serial.println("Watered");
      a.attach(4);
      a.write(180);
      delay(5000);
      a.write(0);
      Firebase.RTDB.setInt(&fbdo, "n", 0);
    }
}
