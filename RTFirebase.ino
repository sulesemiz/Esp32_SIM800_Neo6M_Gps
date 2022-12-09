//Select your modem
//SSL/TLS is currently supported only with SIM8xx series
#define TINY_GSM_MODEM_SIM800
#include "RTClib.h"
RTC_Millis rtc;
//Increase RX buffer
#define TINY_GSM_RX_BUFFER 256

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <TinyGsmClient.h> //https://github.com/vshymanskyy/TinyGSM
#include <ArduinoHttpClient.h> //https://github.com/arduino-libraries/ArduinoHttpClient
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
#include <SoftwareSerial.h>
SoftwareSerial gsm(4, 2); // RX, TX

//String tarih = " ";
//String saat = " ";
//String dakika = " ";
  DateTime now = rtc.now(); 
  double zaman= 100*(now.hour())+now.minute();
 // String saat = String(zaman);
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
const char FIREBASE_HOST[]  = "esp32-af323-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH  = "74nQ7rOF7De1fyuHYJkbqsVDD7PD8dfC52BMSOrJ";
const String FIREBASE_PATH  = "neta/a";
const int SSL_PORT          = 443;
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "internet";
char user[] = "";
char pass[] = "";
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
//GSM Module RX pin to ESP32 2
//GSM Module TX pin to ESP32 4
#define rxPin 4
#define txPin 2
HardwareSerial sim800(1);
TinyGsm modem(sim800);
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
//GPS Module RX pin to ESP32 17
//GPS Module TX pin to ESP32 16
#define RXD2 16
#define TXD2 17
HardwareSerial neogps(2);
TinyGPSPlus gps;
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);
//NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

unsigned long previousMillis = 0;
long interval = 10000;
int counter;
int x=10000000;
//**************************************************************************************************
void setup() {
   gsm.begin(9600);
  delay(3000);
  gsm.println("AT+CLTS=1");
  delay(1000);
  gsm.println("AT&W");
  delay(2000);

  Serial.begin(9600);

#ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
#endif
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  
  Serial.begin(115200);
  Serial.println("esp32 serial initialize");
  
  sim800.begin(9600, SERIAL_8N1, rxPin, txPin);
  Serial.println("SIM800L serial initialize");

 neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);
 //neogps.begin(9600);
  Serial.println("neogps serial initialize");
  delay(3000);
  
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //Restart takes quite some time
  //To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  
  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
  
  http_client.setHttpResponseTimeout(90 * 1000); //^0 secs timeout
}
//**************************************************************************************************


//**************************************************************************************************
void loop() {
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //Restart takes quite some time
  //To skip it, call init() instead of restart()
  //Serial.println("Initializing modem...");
  //modem.init();
  //String modemInfo = modem.getModemInfo();
  //Serial.print("Modem: ");
  //Serial.println(modemInfo);
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  /*gsm.println("AT+CCLK?");
  delay(1000);

  if(gsm.find("+CCLK: \"")){
    tarih = gsm.readStringUntil(',');
    saat = gsm.readStringUntil('+');
    //dakika = gsm.readStringUntil(':');
    gsm.readStringUntil('\r');
    
  }*/
  /* Serial.print("tarih: ");
  Serial.println(tarih);
  Serial.print("saat: ");
  Serial.println(saat);
   Serial.print("dakika: ");
  Serial.println(dakika);
  Serial.println();
  delay(10000);
 */
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //Serial.print(F("Waiting for network..."));
  //if (!modem.waitForNetwork()) {
    //Serial.println(" fail");
    //delay(1000);
    //return;
  //}
  //Serial.println(" OK");
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
    
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(1000);
    return;
  }
  Serial.println(" OK");
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  
  http_client.connect(FIREBASE_HOST, SSL_PORT);
  
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  while (true) {
    if (!http_client.connected()) {
      Serial.println();
      http_client.stop();// Shutdown
      Serial.println("HTTP  not connect");
      break;
    }
    else{
      gps_loop();
    }
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}
//**************************************************************************************************

//**************************************************************************************************
void PostToFirebase(const char* method, const String & path , const String & data, HttpClient* http) {
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); // Currently, this is needed for HTTPS
  
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  String url;
  if (path[0] != '/') {
    url = "/";
  }
  url += path + ".json";
  url += "?auth=" + FIREBASE_AUTH;
  Serial.print("POST:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  
  String contentType = "application/json";
  http->put(url, contentType, data);
  
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  // read the status code and body of the response
  //statusCode-200 (OK) | statusCode -3 (TimeOut)
  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
  Serial.print("Response: ");
  Serial.println(response);
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  if (!http->connected()) {
    Serial.println();
    http->stop();// Shutdown
    Serial.println("HTTP POST disconnected");
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}
//**************************************************************************************************


//**************************************************************************************************
void gps_loop()
{
  //int dene = now.unixtime();
  //String path = String("/neta/",dene);
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //Can take up to 60 seconds
  DateTime now = rtc.now(); 
    double zaman= 100*(now.hour())+now.minute();
     int dene = now.unixtime();
 // String path = String("/neta/",dene);
 //String saat = String(zaman);
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;){
    while (neogps.available()){
      if (gps.encode(neogps.read())){
        newData = true;
        break;
      }
    }
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
 int counter=0;
  for (int counter = 0 ; counter <= 100000; counter = counter +1)

  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  //If newData is true
  if(true){
  newData = false;
  
  String alatitude, alongitude;
  //float altitude;
  //unsigned long date, time, speed, satellites;
  
  alatitude = String(gps.location.lat(), 6); // Latitude in degrees (double)
  alongitude = String(gps.location.lng(), 6); // Longitude in degrees (double)
  
  //altitude = gps.altitude.meters(); // Altitude in meters (double)
  //date = gps.date.value(); // Raw date in DDMMYY format (u32)
 // uint32_t zaman = gps.time.value(); // Raw time in HHMMSSCC format (u32)
  //speed = gps.speed.kmph();
  
  Serial.print("Latitude= "); 
  Serial.print(alatitude);
  Serial.print(" Longitude= "); 
  Serial.println(alongitude);
      
  String gpsData = "{";
  gpsData += "\"counter\":";
  gpsData += counter;
  gpsData += ",";
  gpsData +="\"dene\":";
  gpsData += dene;
  gpsData += ",";
  gpsData +="\"zaman\":";
  gpsData += zaman;
  gpsData += ",";
//  gpsData += "\"value\":" + zaman + ",";
  gpsData += "\"lat\":" + alatitude + ",";
  gpsData += "\"lng\":" + alongitude + "";
  gpsData += "}";

  //PUT   Write or replace data to a defined path, like messages/users/user1/<data>
  //PATCH   Update some of the keys for a defined path without replacing all of the data.
  //POST  Add to a list of data in our Firebase database. Every time we send a POST request, the Firebase client generates a unique key, like messages/users/<unique-id>/<data>
  //https://firebase.google.com/docs/database/rest/save-data
  // String yol = String("neta/",dene) 
  
  PostToFirebase("PATCH", "neta4/"+String(dene) , gpsData, &http_client);
  //delay(55000);

  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}
//**************************************************************************************************
