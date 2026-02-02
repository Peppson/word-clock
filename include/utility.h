#pragma once
#include "config.h"

class Storage;
class Hardware;
class NeoPixels;
class TimeKeeper;

namespace utils 
{
    void IRAM_ATTR benchmark(const char* name = nullptr);
    void printAllMemory();
    void resetAllMemory();
    void printSystemTime(TimeKeeper& chrono);
    void initDebugging(Storage* storage, Hardware* hardware, NeoPixels* pixel, TimeKeeper* chrono);
    void initAccesspoint(Hardware* hardware);
    void initSnake(NeoPixels* pixel);
    const char* inputToEnum(const String& input);
    const char* inputToEnum(const int& input);
    void debugPrintInfo(const uint target, 
                        const String& ssid = "", 
                        const String& password = "", 
                        const String& timeInput = "",
                        bool success = false
                        );   
}
