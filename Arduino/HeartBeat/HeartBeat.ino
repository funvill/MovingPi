#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "FastLED.h"
FASTLED_USING_NAMESPACE

#define DATA_PIN 6
#define SETTING_NUM_PIXELS 30

// Define the array of leds
CRGB leds[SETTING_NUM_PIXELS];

int mode;
static const int MODE_OFF = 0;
static const int MODE_SMALL_RISE = 1;
static const int MODE_WAIT = 2;
static const int MODE_LARGE_RISE = 3;

float speed;
static const float SETTING_SPEED_INCREMENT = 0.2f;
static const float SETTING_SPEED_HIGH_LIMIT = 5.0f;
static const float SETTING_SPEED_LOW_LIMIT = 1.0f;

int brightness;
static const int SETTING_BRIGHTNESS_MIN = 30; 
static const int SETTING_BRIGHTNESS_MAX = 255; 

bool rising;
int changeRate;


// Based heartbeat on this photo 
// https://www.researchgate.net/profile/John_Irvine/publication/264837090/figure/fig9/AS:295933746139145@1447567511850/Figure-10-Aligned-heartbeats-from-high-stress-and-low-stress-tasks.png
unsigned long timer_mode_switch;
static const int TIMER_MODE_OFF = 800;
static const int TIMER_MODE_SMALL = 200;
static const int TIMER_MODE_WAIT = 200;
static const int TIMER_MODE_LARGE = 400;

void setup()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, SETTING_NUM_PIXELS);

    mode = MODE_OFF;
    speed = SETTING_SPEED_LOW_LIMIT;
    brightness = 0;
    timer_mode_switch = 0;
    rising = true;

    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
}

void loop()
{
    // Read input
    // ToDo: REad from digital input

    // Increase or decrease the heat rate
    if (false) {
        speed += SETTING_SPEED_INCREMENT;
        if (speed > SETTING_SPEED_HIGH_LIMIT) {
            speed = SETTING_SPEED_HIGH_LIMIT;
        }
    } else {
        speed -= SETTING_SPEED_INCREMENT;
        if (speed < SETTING_SPEED_LOW_LIMIT) {
            speed = SETTING_SPEED_LOW_LIMIT;
        }
    }

    HeartBeat(1.0);
}

void HeartBeat(float tempo)
{
    static unsigned long lastLoop = millis();
    unsigned long famedelay = millis() - lastLoop;
    lastLoop = millis();

    // Debug print
    Serial.print("mode: " + String(mode));
    Serial.print(", brightness:" + String(brightness));
    // Serial.print(", changeRate:" + String(changeRate));
    // Serial.print(", rising:" + String(rising));
    Serial.print(", tempo:" + String(tempo));
    Serial.print(", timer:" + String(timer_mode_switch - lastLoop));
    Serial.print(", fameDelay:" + String(famedelay));

    Serial.println();

    // Check the timer. If the timer has expired then move to the next state
    if (millis() > timer_mode_switch) {
        mode++;
        rising = true;
        brightness = 0;

        switch (mode) {
            default:
            case MODE_OFF:
                Serial.println("Mode=MODE_OFF, ms=" + String(TIMER_MODE_OFF));
                mode = MODE_OFF;
                timer_mode_switch = lastLoop + (TIMER_MODE_OFF/tempo);
                changeRate = 0;
                break;
            case MODE_SMALL_RISE:
                Serial.println("Mode=MODE_SMALL_RISE, ms=" + String(TIMER_MODE_SMALL));
                timer_mode_switch = lastLoop + (TIMER_MODE_SMALL/tempo);
                changeRate = 255 ; // Small spike. 
                break;
            case MODE_WAIT:
                Serial.println("Mode=MODE_WAIT, ms=" + String(TIMER_MODE_WAIT));
                timer_mode_switch = lastLoop + (TIMER_MODE_WAIT/tempo);
                changeRate = 0;
                break;
            case MODE_LARGE_RISE:
                Serial.println("Mode=MODE_LARGE_RISE, ms=" + String(TIMER_MODE_LARGE));
                timer_mode_switch = lastLoop + (TIMER_MODE_LARGE/tempo);
                // We take the entire range and divide it by 2... This give equal time to both the rise and fall.
                // Then we take the total time this sequence takes divided by the frame rate and we get the rate of change pre frame. 
                changeRate = (255 * 2) / (TIMER_MODE_LARGE / famedelay);
                break;
        }
    }

    // Based on mode, change the LEDs
    if (rising) {
        brightness += changeRate;
    } else {
        brightness -= changeRate;
    }

    // Check limits of brightness
    if (brightness > SETTING_BRIGHTNESS_MAX) {
        brightness = SETTING_BRIGHTNESS_MAX;
        rising = false;
    } else if (brightness < SETTING_BRIGHTNESS_MIN) {
        brightness = SETTING_BRIGHTNESS_MIN;
        rising = true;
    }

    
    for (int i = 0; i < SETTING_NUM_PIXELS; i++) {
        leds[i] = CHSV( 0, 255, brightness);
    }
    FastLED.show();    
}
