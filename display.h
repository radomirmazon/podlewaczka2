#ifndef DISPLAY_H_   /* Include guard */
#define DISPLAY_H_

#define ANIMATION_FRAMES 8

/**
 In this case - display mean 8 LEDs connect to 8 arduino ports.
 You can implement any display with IDisplay interface
 **/

class IDisplay {
    public:
    virtual void tick()=0;
    virtual void runAnimation(uint8_t type)=0;
    virtual void show(uint8_t num)=0;
    virtual void showNum(uint8_t num)=0;
    virtual void showLed(uint8_t state)=0;
    //etc...
};

class Display : public IDisplay {
    public:
    Display(uint8_t* db) {
        displayBits = db;
        pinMode(LED, OUTPUT);
    }

    void startAnimation() {
      animationIndex = 0;
      intExecutorState = *displayBits;
    }

    virtual void showLed(uint8_t state) {
      ledState = state;
    }

    void led_process() {
      if (ledState == LED_BLINKING) {
        ledCounter--;
        if (ledCounter ==0) {
          ledCounter = LED_DELAY;
          led = !led;
          if (led) {
            digitalWrite(LED, LOW);
          } else {
            digitalWrite(LED, HIGH);
          }
        }
      } else {
        if (ledState == LED_OFF) {
          digitalWrite(LED, LOW);
        } else {
          digitalWrite(LED, HIGH);
        }
      }
    }

    virtual void tick() {
      led_process();
      if (animationIndex == -1) {
        *displayBits = intExecutorState;
        return;
      }
      
      if (animationTimer > 0) {
        animationTimer--;
        return;
      } 

      if (animationIndex >(ANIMATION_FRAMES*2) || animation[animationIndex] == -1) {
        //end of animation
        animationIndex = -1;
        //restore display
        *displayBits = intExecutorState;
        return;  
      }
      
      //next frame
      *displayBits= animation[animationIndex];
      animationTimer = animation[animationIndex+1];
      
      animationIndex++;
      animationIndex++;
     
    }

    void oneSecondTick() {
    }

    virtual void show(uint8_t num) {
      intExecutorState = num;
    }

    virtual void showNum(uint8_t num) {
      intExecutorState = 1;
      intExecutorState = intExecutorState << num;
    }
    uint8_t* displayBits;
    int animation[ANIMATION_FRAMES*2];
    int animationIndex = -1;
    int animationTimer = 0;
    uint8_t displayBitsInternal = 0;
    uint8_t intExecutorState = 0;

    boolean led = 0;
    uint8_t ledState = LED_BLINKING;
    int ledCounter = LED_DELAY;

    void animationInit() {
      animationIndex =0;
      *displayBits = animation[0];
      animationTimer = animation[1];
    }

    virtual void runAnimation(uint8_t type) {
      uint8_t i =0;
      switch(type) {
        case ANIMATION_PRESS:
          animation[i++] = 0x09;
          animation[i++] = 10;
          animation[i++] = -1;
          animation[i++] = -1;
        break;
        case ANIMATION_LONG_PRESS:
          animation[i++] = 0x06;
          animation[i++] = 20;
          animation[i++] = -1;
          animation[i++] = -1;
        break;
        case ANIMATION_EXTRA_PRESS:
          animation[i++] = 0x0F0;
          animation[i++] = 20;
          animation[i++] = -1;
          animation[i++] = -1;
        break;
        case ANIMATION_EXTRA_LONG_PRESS: 
          animation[i++] = 0x99;
          animation[i++] = 20;
          animation[i++] = -1;
          animation[i++] = -1;
        break; 
        case ANIMATION_PRE_LONG_PRESS:
          animation[i++] = 0x01;
          animation[i++] = 10;
          animation[i++] = 0x02;
          animation[i++] = 10;
          animation[i++] = 0x04;
          animation[i++] = 10;
          animation[i++] = 0x08;
          animation[i++] = 10;
          animation[i++] = -1;
          animation[i++] = -1;
        break;
        case ANIMATION_PRE_LONG2_PRESS:
        animation[i++] = 0x010;
          animation[i++] = 10;
          animation[i++] = 0x020;
          animation[i++] = 10;
          animation[i++] = 0x040;
          animation[i++] = 10;
          animation[i++] = 0x080;
          animation[i++] = 10;
          animation[i++] = -1;
          animation[i++] = -1;
        break;
        case ANIMATION_PRE_EXTRA_LONG_PRESS:
          animation[i++] = 0x066;
          animation[i++] = 20;
          animation[i++] = 0x99;
          animation[i++] = 20;
          animation[i++] = -1;
          animation[i++] = -1;
        break;
        case ANIMATION_CHANNEL_CHANGE:
          animation[i++] = 0xFF;
          animation[i++] = 10;
          animation[i++] = -1;
          animation[i++] = -1;
        break;
        case ANIMATION_ERROR:
          animation[i++] = 0xFF;
          animation[i++] = 30;
          animation[i++] = 0x0;
          animation[i++] = 50;
          animation[i++] = 0xFF;
          animation[i++] = 50;
          animation[i++] = 0x0;
          animation[i++] = 50;
          animation[i++] = 0xFF;
          animation[i++] = 50;
          animation[i++] = -1;
          animation[i++] = -1;
      }
      animationInit();
    }
};

#endif // DISPLAY_H_
