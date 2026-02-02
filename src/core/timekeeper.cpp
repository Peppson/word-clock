#include <time.h>
#include <WiFi.h> 
#include "core/timeKeeper.h"
#include "core/storage.h"
#include "core/hardware.h"
#include "core/neoPixels.h"

// Static members
uint8_t TimeKeeper::clockSource;
uint32_t TimeKeeper::prevShowedHour = UINT8_MAX;
uint32_t TimeKeeper::prevShowedMinute = UINT8_MAX;
volatile uint32_t TimeKeeper::offsetHours = 0;
volatile uint32_t TimeKeeper::offsetMinutes = 0;


void TimeKeeper::syncSystemTimeAtNight() 
{
    static uint32_t nextSyncTime = 0;
    uint8_t curHour = timeinfo.tm_hour % 24;
    uint32_t curTime = millis();

    // Sync once at 03:00
    if (curHour != 3 || curTime < nextSyncTime ) return;

    Storage storage(true);
    for (size_t i = 0; i < 3; i++) 
    {
        if (syncSystemTimeWiFi(&storage)) 
            break;
        delay(500);
    }

    nextSyncTime = millis() + TWO_HOURS;
}


bool TimeKeeper::syncSystemTimeWiFi(Storage* storage, const bool onBoot) 
{
    // Start new thread with "connecting WiFi" animation, only on boot
    if (onBoot && px) 
        px->animation_threadWiFi(true);

    bool success = syncSystemTimeNTP(storage);
    log("Time synced: %s\nPowerdown WiFi\n", success ? "Success" : "Failed");

    // End "connecting WiFi" animation
    if (onBoot && px) 
    {   
        px->animation_threadWiFi(false);
        delay(3);
        success ?
            px->animation_connectWiFiStatus(px->WiFiConnectionStatus::SUCCESS) : 
            px->animation_connectWiFiStatus(px->WiFiConnectionStatus::FAILED);
        delay(250);
    }

    return success;
}

bool TimeKeeper::syncSystemTimeNTP(Storage* storage) 
{   
    bool success = false;
    String ssid;
    String password;
    storage->getWiFiCredentials(ssid, password);

    // Begin WiFi
    log("Powerup WiFi\nConnecting to \"%s\"\n", ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Sync time via NTP server 
    if (connectWiFi(storage)) 
    {
        configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER);
        if (getLocalTime(&timeinfo)) 
        {  
            resetTimeOffsets();
            success = true; 
        }
    }

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return success; 
}

void TimeKeeper::getSystemTime() 
{
    time_t now;
    time(&now);
    timeinfo = *localtime(&now);
}

bool TimeKeeper::setSystemTime() 
{
    // Format "HH:MM"
    const String hours = Storage::storedTimeInput.substring(0,2);
    const String minutes = Storage::storedTimeInput.substring(3,5);
    
    // Set time to UNIX 0 and add hours and minutes
    timeinfo.tm_hour = hours.toInt();
    timeinfo.tm_min = minutes.toInt();
    timeinfo.tm_year = 70;   
    timeinfo.tm_mon = 0;
    timeinfo.tm_mday = 1;  
    timeinfo.tm_sec = 0;
    
    struct timeval time = {}; 
    time.tv_sec = mktime(&timeinfo);
    if (settimeofday(&time, nullptr) == 0) 
    {
        resetTimeOffsets();
        return true;
    }
    
    log("> TimeKeeper Error: Failure setting time!\n"); 
    return false;
}

void TimeKeeper::printSystemTime(bool showClockSource) 
{
    #if PRINT_ENABLED
        if (showClockSource) 
            PRINT_INFO(MEMORY_SOURCE, "", "", (String)clockSource);

        getSystemTime();
        log("Time: %i:%i:%i \t No offsets time: %i:%i:%i\n", 
            (timeinfo.tm_hour + offsetHours) % 24, (timeinfo.tm_min + offsetMinutes) % 60, timeinfo.tm_sec, 
            timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec
        );
    #endif
}

void TimeKeeper::resetSystemTime() 
{
    struct timeval container = {};
    container.tv_sec = 0;
    settimeofday(&container, nullptr);
}

bool TimeKeeper::connectWiFi(Storage* storage) 
{
    // Try connecting
    bool success = true; 
    uint32_t endTime = millis() + 20*1000;
    
    while (WiFi.status() != WL_CONNECTED) 
    {
        if (millis() > endTime) 
        { 
            success = false;
            break;
        }
        #if PRINT_ENABLED
            Serial.print(". ");
        #endif
        delay(500);
    }

    log("\nWifi: %s \n", success ? "Connected" : "Failed");
    return success;
}
