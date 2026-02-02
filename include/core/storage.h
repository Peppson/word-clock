#pragma once
#include <Preferences.h>
#include "config.h"

class Storage 
{
    public:
        // Turn off hardware interrupts on object creation, turn them on when desctructing
        Storage(bool interrupts);
        ~Storage();

        static bool validateTimeInput(const String& timeInput);
        static bool validateWiFiCredentials(const String& ssid, const String& password);
        uint8_t getClockSource(Preferences& memory);
        bool getTimeInput(Preferences& memory);
        bool getWiFiCredentials(Preferences& memory);
        uint32_t getHexColor(Preferences& memory);
        bool setTimeInput(const String& timeInput);
        bool setWiFiCredentials(const String& ssid, const String& password);
        bool setHexColor(uint32_t color);
        bool setClockSource(Preferences& memory);
        static void resetAllMemory();
        void getWiFiCredentials(String& ssid, String& password);

        static String storedTimeInput;
        static String storedWifiSsid;
        static String storedWifiPassword;

    private:
        const bool _toggleInterrupts;
};
