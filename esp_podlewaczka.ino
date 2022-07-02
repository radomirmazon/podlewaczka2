#include "configConst.h"
#include <ESP8266WiFi.h>
#include "display.h"
#include "ButtonInput.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <EEPROM.h>
#include "weather.h"
#include "sysconfig.h"
#include "SystemLogic.h"
#include "RestApi.h"
#include "TimeSheduler.h"


/* Radomir Mazoń 2022.06 */

/**
 * typedef enum {
    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
} wl_status_t;
 */


uint8_t output[2];

IDisplay* pDisplay = new Display(output);

ButtonInput button;
ESP8266WebServer server(80);
MDNSResponder mdns;
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
Weather weather(&client, pDisplay);
SysConfig config;
SystemLogic systemLogic(&(output[1]), pDisplay, &config);
RestApi restApi(&server, &systemLogic, &config);
TimeScheduler timeScheduler(&systemLogic, &config, &weather, &timeClient);

void setup() {
  // Initialize the serial port
  Serial.begin(115200);
  pinMode(EXT_DATA, OUTPUT);
  pinMode(EXT_LATCH, OUTPUT);
  pinMode(EXT_CLK, OUTPUT);
  output[0] = 0x66;
  output[1] = 0;
  writeExtOutput();
  delay(10);
  writeExtOutput();
  
  WiFi.begin(WIFI_SID, WIFI_PASS);
  Serial.println("Waiting for connection...");
  int tryConnect =0;
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Oh no :(");
    while(1) {
      output[0] = 0x66;
      writeExtOutput();
      delay(100);
      output[0] = 0x00;
      writeExtOutput();
      delay(100);
    }
  }
  
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  if (mdns.begin("podlewaczka", WiFi.softAPIP())) {
    Serial.println("MDNS responder started");
    mdns.addService("http", "tcp", 80);
  }
  
  output[0] = 0;
    
  restApi.bind();
  
  String temp = "Status : ";
  temp += "connected on ";
  temp += WiFi.localIP().toString();
  Serial.println(temp);
  timeScheduler.init();
  weather.init();
}

void writeExtOutput() {
  digitalWrite(EXT_LATCH, LOW);
  shiftOut(EXT_DATA, EXT_CLK, LSBFIRST, output[0]);
  shiftOut(EXT_DATA, EXT_CLK, LSBFIRST, output[1]);
  digitalWrite(EXT_LATCH, HIGH);
}

void fastTick() {
  switch(button.tick()) 
  {
    case BUTTON_PRESS:
      systemLogic.onDeactivate();
      systemLogic.onButtonPress();
      Serial.println("BUTTON_PRESS");
      break;
    case BUTTON_PRESS_LONG:
      systemLogic.onButtonLongPress();
      pDisplay->runAnimation(ANIMATION_LONG_PRESS);
      Serial.println("BUTTON_PRESS_LONG");
      break;
    case BUTTON_EXTRA_PRESS:
      pDisplay->runAnimation(ANIMATION_EXTRA_PRESS); 
      //systemLogic.onExtraButtonPress();
     Serial.println("BUTTON_EXTRA_PRESS");
      break;
    case BUTTON_EXTRA_LONG_PRESS:
      pDisplay->runAnimation(ANIMATION_EXTRA_LONG_PRESS);
      Serial.println("BUTTON_EXTRA_LONG_PRESS");
      break;
    case BUTTON_PRESS_PRE_LONG:
      pDisplay->runAnimation(ANIMATION_PRE_LONG_PRESS);
      Serial.println("BUTTON_PRESS_PRE_LONG");
      break;
    case BUTTON_EXTRA_ACTIVATED:
      systemLogic.onActivate();
      pDisplay->runAnimation(ANIMATION_PRE_LONG2_PRESS);
      Serial.println("BUTTON_EXTRA_ACTIVATED");
      break;
    case BUTTON_EXTRA_PRE_LONG_PRESS:
      pDisplay->runAnimation(ANIMATION_PRE_EXTRA_LONG_PRESS);
      Serial.println("BUTTON_EXTRA_PRE_LONG_PRESS");
      break;
    case BUTTON_EXTRA_DEACTIVATE:
      Serial.println("BUTTON_EXTRA_DEACTIVATE");
      break;  
  }
  
  pDisplay->tick();
  systemLogic.tick();
  writeExtOutput();
}

int secCounter =0;

void oneMinuteTick() {
  weather.oneMinuteTick();
  timeScheduler.oneMinuteTick();
}

void oneHourTick() {
  weather.update();
  timeScheduler.update();
}

void oneSecondTick() {
  secCounter++;
  if (secCounter % 60 == 0) {
    oneMinuteTick();
  }
  if (secCounter == 3600) {
    oneHourTick();
    secCounter =0;
  }
  
  systemLogic.oneSecondTick();
  timeScheduler.oneSecondTick();
  /*switch (WiFi.status()){
      case WL_NO_SSID_AVAIL:
        Serial.println("Configured SSID cannot be reached");
        break;
      case WL_CONNECTED:
        Serial.println("Connection successfully established");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connection failed");
        break;
    }
    Serial.printf("Connection status: %d\n", WiFi.status());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
  */
  /*
      time_t epochTime = timeClient.getEpochTime();
      Serial.print("Epoch Time: ");
      Serial.println(epochTime);
      
      String formattedTime = timeClient.getFormattedTime();
      Serial.print("Formatted Time: ");
      Serial.println(formattedTime);
      */
}

unsigned long ttime = 0;
unsigned long lastTime = 0; 
void loop() {
  server.handleClient();
  MDNS.update();
  ttime = millis();
  if (ttime - lastTime > 1000) {
    oneSecondTick();
    lastTime = ttime;
  }
  if ((ttime - lastTime) % 50 == 0 ) {
    fastTick();
  }
}
