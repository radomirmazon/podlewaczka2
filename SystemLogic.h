#ifndef SYSTEM_LOGIC_H_   /* Include guard */
#define SYSTEM_LOGIC_H_

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "weather.h"
#include "sysconfig.h"


class SystemLogic {
  private:
    uint8_t* output;
    IDisplay* pDisplay;
    SysConfig* pConfig;
    int channelTimer= -1;
    int channelisAutomationNo = -1;
    uint8_t repeatCounter = 0;
    boolean channelNext = false;
    int channelNo = -1;
    boolean isAutomation = false;

    public:
    uint8_t getOutput() {
      return *output;
    }

    void takeOff() {
      channelNo = -1;
      channelTimer = -1;
      channelNext = false;
      *output =0;
      pDisplay->show(*output);
    }

    void setChannel(int chNo) {
      if (!pConfig->canActivateChannel(chNo) || chNo < 0 || chNo > 7) {
        return;
      }
      channelTimer = pConfig->getTimeForChannel(chNo);
      channelNext = false;
      *output = 1 << chNo;
      pDisplay->show(*output);
    }

    void resetChannel(int chNo) {
      if (chNo < 0 || chNo > 7) {
        return;
      }
      *output = *output & ~((uint8_t)(1<<chNo));
      if (chNo == channelNo) { 
        takeOff();
      }
      pDisplay->show(*output);
    }

    void startAuto() {
      *output = 0;
      repeatCounter = pConfig->getRepeat();
      channelNext = true;
      setNextChannel();
    }

    void letsPlayAgain() {
      if (repeatCounter == 0) {
        return;
      }
      repeatCounter--;
      setNextChannel();
    }

    void setNextChannel() {
      channelTimer= -1;
      *output = 0;
      while(channelNo <8) {
        channelNo++;
        if (pConfig->canActivateChannel(channelNo)) {
          break;
        }
      }
      if (channelNo == 8) {
        channelNo = -1;
        letsPlayAgain();
      } else {
        *output = 1 << channelNo;
        channelTimer= pConfig->getTimeForChannel(channelNo);
      }
      pDisplay->show(*output);
    }
    

    
  public: 
    SystemLogic(uint8_t* output, IDisplay* pDisplay, SysConfig* pConfig) {
      this->output = output;
      this->pDisplay = pDisplay;
      this->pConfig = pConfig;
    }

    void onButtonLongPress() {
      startAuto();
    }
    
    void onButtonPress() {
      repeatCounter = 0;
      setNextChannel();
    }
    
    char* getStatus() {   
      return pConfig->getConfig();
    }

    void setChannel(int channelNumber, boolean channelValue) {
      if (channelNumber > 7 || channelNumber < 0 || 
           (channelValue && !pConfig->canActivateChannel((uint8_t)channelNumber))) 
      {
        pDisplay->runAnimation(ANIMATION_ERROR);
        return; 
      }
      *output = 0;
      if (channelValue) {
        setChannel(channelNumber);
      } else {
        resetChannel(channelNumber);
      }
      
      pDisplay->runAnimation(ANIMATION_CHANNEL_CHANGE);
    }

    void tick() {
       
    }

   
    void oneSecondTick() {
      if (isAutomation) {
        pDisplay->runAnimation(ANIMATION_PRE_LONG2_PRESS);
      }
      
      if (channelTimer == -1) {
        return;
      }

      channelTimer--;

      if (channelTimer == 0) {
        if (channelNext) {
          setNextChannel();
        } else {
          takeOff();
        }
      }

    }

    void onActivate() {
      isAutomation = true;
    }

    void onDeactivate() {
      isAutomation = false;
    }
};

#endif // SYSTEM_LOGIC_H_
