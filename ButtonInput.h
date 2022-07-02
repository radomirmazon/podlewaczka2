#ifndef BINPUT_CON_H_   /* Include guard */
#define BINPUT_CON_H_

#include "configConst.h"

#define LONG_COUNT 350
#define LONG2_COUNT 600

#define BUTTON_PRESS_LONG2    66
/**
    This class add virtual button function after long press 2
    It's works 5 second from last click.
**/
class ExtraButtonDecoder {
    public:
        uint8_t decode(uint8_t buttonCode) {
            uint8_t result = buttonCode;
            if (buttonCode == BUTTON_PRESS_LONG2) {
                activeExtra();
            }
            
            if (buttonCode == NO_MESSAGE && isDeactivate) {
              isDeactivate = false;
              return BUTTON_EXTRA_DEACTIVATE; 
            }

            if (isExtraButton()) {
                if (buttonCode != NO_MESSAGE) {
                  activeExtra();
                }
                //decode:
                switch(buttonCode) {
                    case BUTTON_PRESS:
                        result = BUTTON_EXTRA_PRESS;
                        break;
                    case BUTTON_PRESS_PRE_LONG:
                        result = BUTTON_EXTRA_PRE_LONG_PRESS;
                        break;
                    case BUTTON_PRESS_LONG:
                        result = BUTTON_EXTRA_LONG_PRESS;
                        break;    
                }
            } 
            

            return result;
        }

        void tick() {
            if (extraCounterDown > 0) {
                extraCounterDown--;
                if (extraCounterDown == 0) {
                 isDeactivate = true; 
                }
            }
        }

        boolean isExtraButton() {
            return extraCounterDown > 0;
        }

    private:
        void activeExtra() {
            extraCounterDown = HOW_LONG_PR_SHOW;
        }
        
        uint16_t extraCounterDown = 0;
        boolean isDeactivate = false;
};


class ButtonInput {

  public:
    ButtonInput() {
       pinMode(BUTTON, INPUT_PULLUP);
    }

    //tick can simply send-back message, like event... @see configConst.h
    uint8_t tick() {
        //buttonDecoder.tick();
        //return buttonDecoder.decode(readSwitch());
        return readSwitch();
    }

    private:
    uint16_t buttonCounter = 0;
    //ExtraButtonDecoder buttonDecoder;
    
    uint8_t readSwitch() {
        uint8_t result = NO_MESSAGE;
        if (digitalRead(BUTTON) == 0) {
            if (buttonCounter == LONG_COUNT) {
                result = BUTTON_PRESS_PRE_LONG;
            }
            if (buttonCounter == LONG2_COUNT) {
                result = BUTTON_EXTRA_ACTIVATED;
            }
            buttonCounter++;
        } else {
            if (buttonCounter > 10 && buttonCounter < LONG_COUNT) {
                result = BUTTON_PRESS;
            } 
            if (buttonCounter >= LONG_COUNT && buttonCounter < LONG2_COUNT) {
                 result = BUTTON_PRESS_LONG;
            }
            if (buttonCounter >= LONG2_COUNT) {
                result = BUTTON_PRESS_LONG2;
            }
            buttonCounter = 0;
        }

        return result;
    }
};


#endif // INPUT_CON_H_
