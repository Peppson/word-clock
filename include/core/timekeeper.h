#pragma once
#include "core/neoPixels.h"
#include "config.h"

class TimeKeeper 
{
    public:
        TimeKeeper(NeoPixels* pixel) : px(pixel) {};

        struct tm timeinfo;

        static void resetTimeOffsets() __attribute__((always_inline)) 
        {
            offsetHours = 0;
            offsetMinutes = 0;
        }
        
        void syncSystemTimeAtNight();
        bool syncSystemTimeWiFi(Storage* storage, const bool onBoot = false); 
        bool syncSystemTimeNTP(Storage* storage);
        void resetSystemTime();
        void getSystemTime();
        bool setSystemTime();
        void printSystemTime(bool status = false);

        static uint8_t clockSource;
        static uint32_t prevShowedHour;
        static uint32_t prevShowedMinute;
        static volatile uint32_t offsetHours;
        static volatile uint32_t offsetMinutes;

    private:
        NeoPixels* px;

        bool connectWiFi(Storage* storage);
        
        const uint32_t TWO_HOURS = 2*60*60*1000UL;
};
