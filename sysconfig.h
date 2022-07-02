#ifndef CONFIG_H_   /* Include guard */
#define CONFIG_H_

class SysConfig {

  private:
  uint8_t channelEnabledMask = 0xFF;
  uint8_t repeat = 2;
  uint8_t alarmH = 23;
  uint8_t alarmM = 30;
  uint8_t channelTimes[8];

  public:
    SysConfig() {
      for(int i=0; i<8; i++) {
        channelTimes[i] = 100;
      }
    }
  
    boolean canActivateChannel(uint8_t channelNumber) {
      return (1<<channelNumber & channelEnabledMask) > 0;
    }
    uint8_t getChannelEnabledMask() {
      return channelEnabledMask;
    }
    int getTimeForChannel(uint8_t channelNumber) {
      if (channelNumber <=7 && channelNumber >=0) {
        return channelTimes[channelNumber] * 10;
      }
      return 0;
    }
    uint8_t getRepeat() {
      return repeat;
    }

    uint8_t getAlarmH() {
      return alarmH;
    }
    
    uint8_t getAlarmM() {
      return alarmM;
    }

    void store(uint8_t* data) {
      data[0] = channelEnabledMask;
      data[1] = repeat;
      data[2] = alarmH;
      data[3] = alarmM;
      for (uint8_t i=0; i<8; i++) {
        data[i+4] = channelTimes[i];
      }
    }

    void restore(uint8_t* data) {
      channelEnabledMask = data[0];
      repeat = data[1];
      alarmH = data[2];
      alarmM = data[3];
      for (uint8_t i=0; i<8; i++) {
        channelTimes[i] = data[i+4];
      }
    }

    char* getConfig() {
      static char result[50];
      result[0] = 0;

      snprintf(result, 50, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u", channelEnabledMask, repeat, alarmH, alarmM, 
         channelTimes[0], channelTimes[1],
         channelTimes[2], channelTimes[3],
         channelTimes[4], channelTimes[5],
         channelTimes[6], channelTimes[7]
         );
      return result;
    }

    boolean setConfig(String csdata) {
      Serial.println("New config...");
      //char sdata[] = "95,2,18,20,150,150,150,150,150,150,150,150";
      char sdata[50];
      csdata.toCharArray(sdata, 50);
      uint8_t result[12] = {};
      char *token;
      int index = 0;
      char delimiters[] = ",";
      token = strtok(sdata, delimiters);

      while (token  != NULL && index < 12) {
        result[index] = (uint8_t) atoi(token);
        Serial.print("config[");
        Serial.print(index);
        Serial.print("] = ");
        Serial.println(result[index++]);
        token = strtok(NULL, delimiters);
      }
      if (index == 12) {
        restore(result);
        Serial.println("Configuration updated...");
        return true;
      }
      Serial.println("Configuration was incorrect.");
      return false;
    }
};


#endif // CONFIG_H_
