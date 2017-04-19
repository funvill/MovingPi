#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define SETTING_NUM_PIXELS 30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(SETTING_NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int mode;
static const int MODE_OFF = 0;
static const int MODE_SMALL_RISE = 1;
static const int MODE_WAIT = 2;
static const int MODE_LARGE_RISE = 3;

int speed;
static const int SETTING_SPEED_INCREMENT = 3;
static const int SETTING_SPEED_HIGH_LIMIT = 100;
static const int SETTING_SPEED_LOW_LIMIT = 20;

int brightness;
bool rising;
int changeRate;

unsigned long timer_mode_switch;
static const int TIMER_MODE_OFF = 500;
static const int TIMER_MODE_SMALL = 200;
static const int TIMER_MODE_WAIT = 200;
static const int TIMER_MODE_LARGE = 400;

void setup()
{
    mode = MODE_OFF;
    speed = SETTING_SPEED_LOW_LIMIT;
    brightness = 0;
    timer_mode_switch = 0;
    rising = true;

    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);

    strip.begin();
    strip.show();
}

void loop()
{
    static unsigned long lastLoop = millis();
    unsigned long famedelay = millis() - lastLoop;
    lastLoop = millis();

    // Debug print
    Serial.print("mode: " + String(mode));
    Serial.print(", brightness:" + String(brightness));
    Serial.print(", changeRate:" + String(changeRate));
    Serial.print(", rising:" + String(rising));
    Serial.print(", timer_mode_switch:" + String(timer_mode_switch - lastLoop));
    Serial.print(", fameDelay:" + String(famedelay));

    Serial.println();

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

    // Check the timer. If the timer has expired then move to the next state
    if (millis() > timer_mode_switch) {
        mode++;
        rising = true;
        brightness = 0;

        switch (mode) {
            default:
            case MODE_OFF:
                Serial.println("Mode=MODE_OFF, ms="+ String(TIMER_MODE_OFF) );
                mode = MODE_OFF;
                timer_mode_switch = lastLoop + TIMER_MODE_OFF;
                changeRate = 0;
                break;
            case MODE_SMALL_RISE:
                Serial.println("Mode=MODE_SMALL_RISE, ms="+ String(TIMER_MODE_SMALL));
                timer_mode_switch = lastLoop + TIMER_MODE_SMALL;
                changeRate = (255 * 2) / (TIMER_MODE_SMALL / famedelay) ;
                break;
            case MODE_WAIT:
                Serial.println("Mode=MODE_WAIT, ms="+ String(TIMER_MODE_WAIT));
                timer_mode_switch = lastLoop + TIMER_MODE_WAIT;
                changeRate = 0;
                break;
            case MODE_LARGE_RISE:
                Serial.println("Mode=MODE_LARGE_RISE, ms="+ String(TIMER_MODE_LARGE));
                timer_mode_switch = lastLoop + TIMER_MODE_LARGE;
                changeRate = (255 * 2) / (TIMER_MODE_LARGE / famedelay) ;
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
    if (brightness > 255) {
        brightness = 255;
        rising = false;
    } else if (brightness < 0) {
        brightness = 0;
        rising = true;
    }

    // Set it to black.
    for (int i = 0; i < SETTING_NUM_PIXELS; i++) {
        strip.setPixelColor(i, brightness, 0, 0);
    }
    strip.show();    

}
