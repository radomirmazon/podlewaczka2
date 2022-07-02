#ifndef WEATHER_H_   /* Include guard */
#define WEATHER_H_

#include "display.h"
#include <ArduinoJson.h>

// 3h*8 = 24h - 24h average
#define WEATHER_MEMORY_SIZE 8

class Weather {
  private:
  WiFiClient* pClient;
  String text;
  uint8_t index=0;
  String timestampArray[WEATHER_MEMORY_SIZE];
  float  rainArray[WEATHER_MEMORY_SIZE];
  float  lastValueRain=0;
  uint8_t seconds = 0;
  boolean hasErrors = false;
  IDisplay* pDisplay;
  
  public:
    Weather(WiFiClient* pClient, IDisplay* pDisplay) {
      this->pClient = pClient;
      this->pDisplay = pDisplay;
    }
    
    void init() {
      text.reserve(2500);
      for (uint8_t i=0; i<WEATHER_MEMORY_SIZE; i++) {
        timestampArray[i] = "";
        rainArray[i] = 0;
      }
      update();
    }


    void oneMinuteTick() {
      if (hasErrors) { 
        update();
      }
    }

    boolean isRainy() {
      return getAverage() > RAIN_TRESHOLD || lastValueRain > RAIN_TRESHOLD;
    }

    private:
    void parseJson(const char * jsonString) {
      StaticJsonDocument<2000> doc;
    
      DeserializationError error = deserializeJson(doc, jsonString);
      if (error) {
        Serial.println("parseObject() failed");
        Serial.println(error.c_str());
        return;
      }
      JsonObject root = doc.as<JsonObject>();
      JsonArray list = root["list"];
      JsonObject nowT = list[0];
      String timestamp = nowT["dt_txt"];  
      JsonObject rainNode = nowT["rain"];
      
      boolean isSuccess = false;
      if (rainNode) {
        String rain = rainNode["3h"];
        if (rain) {
          saveRain(rain.toFloat(), timestamp);
          isSuccess = true;
        }
      }
      if (!isSuccess) {
        saveRain(0, timestamp);
      }
    }

    void saveRain(float rain, String timestamp) {
      hasErrors = false;
      lastValueRain = rain;
      if (timestampArray[index] != timestamp) {
        incIndex();
        timestampArray[index] = timestamp;
        rainArray[index] = rain;
        Serial.println("New record was added.");
      } else {
        Serial.println("Timestamp already exists.");
      }
      Serial.println(timestamp);
      Serial.println(rain);
      Serial.println("average:");
      Serial.println(getAverage());
      
      if (isRainy()) {
        pDisplay->showLed(LED_ON);
      } else {
        pDisplay->showLed(LED_OFF);
      }
    }

    void incIndex() {
      index++;
      if (index >= WEATHER_MEMORY_SIZE) {
        index = 0;
      }
    }

    float getAverage() {
      float result =0;
      for(uint8_t i=0; i<WEATHER_MEMORY_SIZE; i++) {
        result += rainArray[i];
      }
      return result / WEATHER_MEMORY_SIZE;
    }

    public:
    void update() {
      pClient->stop();
      pDisplay->showLed(LED_BLINKING);
      Serial.println(">>> get weather");
      if (pClient->connect("api.openweathermap.org", 80)) {
        pClient->println(WEATHER_GET);
        pClient->println("Host: api.openweathermap.org");
        pClient->println("User-Agent: ArduinoWiFi/1.1");
        pClient->println("Connection: close");
        pClient->println();
        
        unsigned long timeout = millis();
        while (pClient->available() == 0) {
          if (millis() - timeout > 10000) {
            Serial.println(">>> Client Timeout !");
            pClient->stop();
            hasErrors = true;
            return;
          }
        }

        while (pClient->available()) {
          text = pClient->readStringUntil('\n');
          if (!text.startsWith("{")) {
               continue;
          }
          parseJson(text.c_str());
        }
      }
      else {
        Serial.println("connection failed");
        hasErrors = true;
        return;
      }
    }
};


#endif // WEATHER_H_
