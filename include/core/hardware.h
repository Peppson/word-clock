#pragma once
#include "config.h"

class AccessPoint;
class TimeKeeper;
class Preferences;

class Hardware 
{
    public:
        Hardware() = default;
        
        void setup(NeoPixels* pixel);
        void setClockFromSource(Storage& storage, TimeKeeper& chrono);
        void beginAccesspoint();
        static void reboot();
        static void timerOverflowHandler(TimeKeeper& chrono);
        static void beginSerial();
        static void errorLED(bool error);
        static void resetBtnPressed();
        static bool resetBtnPressed(NeoPixels* pixel);
        static bool isInterrupted();
        static void enableInterrupts();
        static void disableInterrupts();
        static void enableAccesspointInterrupt();
        static void disableAccesspointInterrupt();
        void debugKeyboardInput(TimeKeeper& chrono);

        static volatile bool buttonReset;
        static volatile bool buttonAccesspoint;
        static volatile bool timeSetBybutton;
        static volatile bool brightnessSetBybutton;
         
    private:
        static NeoPixels* pixel;

        // Mock hardware buttons with usb keyboard
        enum DebugButtons : int 
        {
            HOURS = 1,
            FIVE_MINUTES = 2,
            DEBUG_TIME = 3,
            INCREMENT_BRIGHTNESS = 4,
            REBOOT = 5,
            START_ACCESSPOINT = 6,
            RESET_BTN = 7,
            DEBUG_RANDOM = 8
        };
        
        bool setClockFromTimeInput(Storage& storage, TimeKeeper& chrono, Preferences& memory);
        bool setClockFromWiFiInput(Storage& storage, TimeKeeper& chrono, Preferences& memory);
        void debugKeyboardAction(const uint8_t button, TimeKeeper& chrono);
        void printDebugInfo(NeoPixels* px);
        void printKeyboardInfo(); 
        static bool IRAM_ATTR isButtonDebounced();
        static void IRAM_ATTR isr_buttonHour(); 
        static void IRAM_ATTR isr_button5min();
        static void IRAM_ATTR isr_buttonBrightness();
        static void IRAM_ATTR isr_buttonAccesspoint();
        static void IRAM_ATTR isr_buttonReset();
        static void isr_debugIncrementTime();
}; 
