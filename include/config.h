#pragma once
#include <Arduino.h>
#include <stdio.h>
#include <FastLED.h>
#include "utility.h"

// Dev
#define PRINT_ENABLED 0                                 // Print over serial connection 
#define PRINT_SNAKE_ENABLED 0                           // Print snake game inputs
#define PRINT_SYSTEM_TIME 0                             // Print system time each second
#define DEBUG_RESET_ALL 0                               // Full reset
#define DEBUG_ACCESSPOINT 0                             // Boot with AP
#define DEBUG_IGNORE_CLOCK_SOURCE 0                     // Boot with clock soruce = NONE
#define DEBUG_SNAKE 0                                   // Boot with snake   
#define DEBUG_OVERRIDE_BRIGHTNESS 0                     // Set static brightness level uint8. > 0 (Sensor disabled)
#define DEBUG_TEST_FULL_BRIGHTNESS 0                    // For measuring max current consumption
#define USE_SECRET_CREDENTIALS 1                        // Use WiFi credentials from secret.h instead of memory
#define USE_KEYBOARD 0                                  // Serial (USB keyboard) added as buttons
#define FRIEND_CLOCK 0                                  // Different startup animation, nsfw...
#define SERPENTINE_LED_LAYOUT 1                         // See below

/*
*   ######### Led layout #########  
*
*   SERPENTINE_LED_LAYOUT true
*   0  >  1  >  2  >  3  >  4 ... 11
*                                 |
*   22 ... 15  <  14  <  13  <  12
*   |
*   23...
*
*   SERPENTINE_LED_LAYOUT false
*   0  >  1  >  2  >  3  >  4 ... 11
*   12  >  13  >  14  >  15...
*/

constexpr const char* AP_SSID = "Clock-Setup";          // Device name
constexpr const char* NTP_SERVER = "pool.ntp.org";      // NTP server
constexpr const char* MEMORY_LOCATION = "Memory";       // Identifier string
constexpr int GMT_OFFSET_SEC = 3600;                    // GMT +- in seconds                        (Hardcoded for Sweden)
constexpr int DAY_LIGHT_OFFSET_SEC = 0;                 // Daylight saving?                         (Wont be needed with NTP)
constexpr uint WIFI_CREDENTIALS_MAX_LENGTH = 30;        // Max length for ssid and password  
constexpr uint JSON_PAYLOAD_MAX_LENGTH = 200;           // Max payload length           
constexpr uint ACCESSPOINT_UPTIME = 10*60*1000;         // Accesspoint upactive
constexpr uint BUTTON_DEBOUNCE_INTERVAL = 250;          // Debounce delay in ms for buttons
constexpr uint SNAKE_GAME_SPEED = 250;                  // Delay in ms between "frames" (ms = 1000/fps)
constexpr uint SERIAL_BAUD_RATE = 115200;               // Baud TX/RX
constexpr const char* HTML_FILE_PATHS[] = {             // Webpages in /data
    "/webpage.txt", 
    "/webpageBackup.txt"
};

// Lux sensor
constexpr uint8_t LUX_READINGS_SIZE = 4;                // Size of average, essentially the speed of change
constexpr float LUX_THRESHOLD_MAX = 10;                 // Light sensor threshold for max brightness
constexpr float LUX_CURVE_EXPONENT = 2.2f;              // Brightness correction: (normalized luxLevel)^x
constexpr uint LED_MIN_BRIGHTNESS = 5;                  // Min led brightness

// Leds
constexpr bool LEDS_OFF_IN_TOTAL_DARKNESS = true;       // Turn off all leds in total darkness
constexpr float LED_INITIAL_SCALER = 0.4f;              // Boot ledscaler
constexpr uint DEFAULT_CLOCK_COLOR = 0xFFB936;          // Main Color
constexpr uint DEBUG_CLOCK_COLOR = 0xFF4500;            // Debug
constexpr uint16_t MATRIX_WIDTH = 11;                   // Wordclock led width
constexpr uint16_t MATRIX_HEIGHT = 10;                  // Wordclock led height

// Physical I/O
constexpr uint8_t BTN_ACCESSPOINT = 19;                 // Old 16
constexpr uint8_t BTN_RESET = 23;                       // Old 17
constexpr uint8_t BTN_HOUR = 18;                        // Old 18
constexpr uint8_t BTN_5_MINUTES = 16;                   // Old 19
constexpr uint8_t BTN_BRIGHTNESS = 17;                  // Old 23
constexpr uint8_t PIN_ERROR_LED = 13;
constexpr uint8_t PIN_LED_DATA = 12;

// Global enums
enum FadeType : bool
{
    FADE_IN = true,
    FADE_OUT = false
};

enum ClockSource : uint8_t 
{ 
    NONE, 
    TIME_INPUT, 
    WIFI 
};

enum DebugPrintType 
{
    WIFI_FORM,
    TIME_FORM,
    MEMORY_WIFI,
    MEMORY_TIME,
    MEMORY_SOURCE,
};

// Print toggle
#if PRINT_ENABLED
    #define log(...) printf(__VA_ARGS__)
    #define PRINT_INFO(...) utils::debugPrintInfo(__VA_ARGS__)
    #define STOP printf("\n#####  Stop right there criminal scum!  ##### \n"); while (1) { delay(1); }
#else
    #define log(...)
    #define PRINT_INFO(...)
    #define STOP while (1) { delay(1); }
#endif

#if PRINT_SNAKE_ENABLED
    #define PRINT_BTN(...) printf(__VA_ARGS__)
#else 
    #define PRINT_BTN(...)
#endif

// Asserts
constexpr uint16_t TOTAL_LEDS = MATRIX_WIDTH * MATRIX_HEIGHT;
static_assert(TOTAL_LEDS == 110, "Led \"shapes\" in include/ledMapping/ expects TOTAL_LEDS = 110, easily changed");
static_assert(TOTAL_LEDS < 255, "TOTAL_LEDS is greater than 255. Many vars has type uint8_t, things will BREAK!");

// Debug symbols
#if DEBUG_ACCESSPOINT || DEBUG_SNAKE || DEBUG_RESET_ALL || DEBUG_TEST_FULL_BRIGHTNESS
    #define ANY_DEBUG_SYMBOL
#endif
