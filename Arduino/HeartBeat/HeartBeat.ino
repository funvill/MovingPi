#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "FastLED.h"
FASTLED_USING_NAMESPACE

#define DATA_PIN 6
#define SPEED_PIN 5
#define SETTING_NUM_PIXELS 30

// Define the array of leds
CRGB leds[SETTING_NUM_PIXELS];

int mode;
static const int MODE_OFF = 0;
static const int MODE_SMALL_RISE = 1;
static const int MODE_WAIT = 2;
static const int MODE_LARGE_RISE = 3;

int brightness;
static const int SETTING_BRIGHTNESS_MIN = 30;
static const int SETTING_BRIGHTNESS_MAX = 255;

bool rising;
int changeRate;
float speed;
unsigned long lastLoop;

// Based heartbeat on this photo
// https://www.researchgate.net/profile/John_Irvine/publication/264837090/figure/fig9/AS:295933746139145@1447567511850/Figure-10-Aligned-heartbeats-from-high-stress-and-low-stress-tasks.png
unsigned long timer_mode_switch;
static const int TIMER_MODE_OFF = 800;
static const int TIMER_MODE_SMALL = 200;
static const int TIMER_MODE_WAIT = 200;
static const int TIMER_MODE_LARGE = 400;

void Reset()
{
    mode = MODE_OFF;
    brightness = 0;
    timer_mode_switch = millis();
    rising = true;
    speed = 1.0f;
}

void setup()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, SETTING_NUM_PIXELS);
    Reset();

    pinMode(SPEED_PIN, INPUT);

    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
}

void loop()
{
    if (digitalRead(SPEED_PIN) == HIGH) {
        speed += 0.05;
        // Serial.println("1 speed: " + String(speed));
    } else {
        speed -= 0.05;
        // Serial.println("0 speed: " + String(speed));
    }

    if (speed > 4.0f) {
        speed = 4.0f;
    } else if (speed < 1.0f) {
        speed = 1.0f;
    }

    HeartBeat(speed);
}

void HeartBeat(float tempo)
{
    unsigned long famedelay = millis() - lastLoop;
    lastLoop = millis();

    // Debug print
    Serial.print("mode: " + String(mode));
    Serial.print(", tempo:" + String(tempo));
    Serial.print(", brightness:" + String(brightness));
    // Serial.print(", changeRate:" + String(changeRate));
    // Serial.print(", rising:" + String(rising));
    Serial.print(", timer:" + String(timer_mode_switch - lastLoop));
    Serial.print(", fameDelay:" + String(famedelay));

    // Check the timer. If the timer has expired then move to the next state
    if (millis() > timer_mode_switch) {
        mode++;
        rising = true;
        brightness = 0;
        changeRate = 0;
        unsigned long modeDelay = 0;

        switch (mode) {
            default:
            case MODE_OFF:
                mode = MODE_OFF;
                modeDelay = (TIMER_MODE_OFF / tempo);
                Serial.print(" Mode=MODE_OFF, ms=" + String(modeDelay));
                break;
            case MODE_SMALL_RISE:
                modeDelay = (TIMER_MODE_SMALL / tempo);
                Serial.print(" Mode=MODE_SMALL_RISE, ms=" + String(modeDelay));
                changeRate = 255; // Small spike.
                break;
            case MODE_WAIT:
                modeDelay = (TIMER_MODE_WAIT / tempo);
                Serial.print(" Mode=MODE_WAIT, ms=" + String(modeDelay));
                break;
            case MODE_LARGE_RISE:
                modeDelay = (TIMER_MODE_LARGE / tempo);
                Serial.print(" Mode=MODE_LARGE_RISE, ms=" + String(modeDelay));
                // We take the entire range and divide it by 2... This give equal time to both the rise and fall.
                // Then we take the total time this sequence takes divided by the frame rate and we get the rate of change pre frame.
                changeRate = (255 * 2) / (TIMER_MODE_LARGE / famedelay);
                break;
        }

        timer_mode_switch = lastLoop + modeDelay;
    }

    Serial.println();

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
        leds[i] = CHSV(0, 255, brightness);
    }
    FastLED.show();
}
