#ifndef TIME_SCHEDULER_H_   /* Include guard */
#define CONFIG_H_

#include "SystemLogic.h"

class TimeScheduler {

  SystemLogic* pSystemLogic;
  SysConfig* pSysConfig;
  NTPClient* pTime;
  Weather* pWeather;
  boolean isTimeCorrect = false;
  
  public: 
  TimeScheduler(SystemLogic* pSystemLogic, SysConfig* pSysConfig, Weather* pWeather, NTPClient* pTime) {
    this->pSystemLogic = pSystemLogic;
    this->pSysConfig = pSysConfig;
    this->pTime = pTime; 
    this->pWeather = pWeather;
  }

  void init() {
      pTime->begin();
      pTime->setTimeOffset(GMT_OFFSET);
      update();
      oneMinuteTick();
  }
  
  void oneMinuteTick() {
     time_t epochTime = pTime->getEpochTime();
     struct tm *ptm = gmtime ((time_t *)&epochTime);
     int currentYear = ptm->tm_year+1900;
     isTimeCorrect = currentYear > 2021;
     if (!isTimeCorrect) {
        Serial.print("current year: ");
        Serial.println(currentYear);
        Serial.println("Time is incorrect... updating...");
        pTime->update();
     }
  }

  void showTime() {
    int currentHour = pTime->getHours();  
    int currentMinute = pTime->getMinutes();
    int currentSecond = pTime->getSeconds();
    Serial.print("Time: ");
    Serial.print(currentHour);      
    Serial.print(":");
    Serial.print(currentMinute);
    Serial.print(":");
    Serial.print(currentSecond);
    Serial.println();
  }

  void update() {
    pTime->update();
    showTime();
  }

  void oneSecondTick() {
    uint8_t alarmH = pSysConfig->getAlarmH();
    uint8_t alarmM = pSysConfig->getAlarmM();
    
    int currentHour = pTime->getHours();  
    int currentMinute = pTime->getMinutes();
    int currentSecond = pTime->getSeconds();

    if (isTimeCorrect && !pWeather->isRainy() && currentHour == alarmH && currentMinute == alarmM && currentSecond == 0) {
      Serial.println("Alarm...");
      pSystemLogic->startAuto();
    }
  }
};

#endif 
