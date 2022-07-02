#ifndef CONST_CON_H_   /* Include guard */
#define CONST_CON_H_

#define EXT_DATA 14
#define EXT_LATCH 12
#define EXT_CLK 13
#define BUTTON 0
#define LED 5

//sendback message
#define NO_MESSAGE            0
#define BUTTON_PRESS          1
#define BUTTON_PRESS_PRE_LONG 2
#define BUTTON_PRESS_LONG     3
#define BUTTON_EXTRA_ACTIVATED 4
#define BUTTON_EXTRA_PRESS    5
#define BUTTON_EXTRA_PRE_LONG_PRESS 6
#define BUTTON_EXTRA_LONG_PRESS 7
#define BUTTON_EXTRA_DEACTIVATE 8

#define ONCHAGE_PR_VALUE 10


#define HOW_LONG_PR_SHOW  800

// Aanimations
#define     ANIMATION_PRESS 0
#define     ANIMATION_LONG_PRESS 1
#define     ANIMATION_EXTRA_PRESS 2
#define     ANIMATION_EXTRA_LONG_PRESS 3
#define     ANIMATION_PRE_LONG_PRESS 4
#define     ANIMATION_PRE_LONG2_PRESS 5
#define     ANIMATION_PRE_EXTRA_LONG_PRESS 6
#define     ANIMATION_CHANNEL_CHANGE 7
#define     ANIMATION_ERROR 8


//LED
#define LED_DELAY 50
#define LED_OFF 0
#define LED_ON 1
#define LED_BLINKING 2

//WIFI:
#define WIFI_SID "yourSID"
#define WIFI_PASS "yourPassword"

//TIME: GMT+2H (for Poland)
#define GMT_OFFSET 7200

//rainfall threshold
#define RAIN_TRESHOLD 2
#define WEATHER_GET "GET /data/2.5/forecast?lat=00.000&lon=00.000&units=metric&cnt=1&APPID=00000000000000000000000000000 HTTP/1.1"


#endif // CONST_CON_H_
