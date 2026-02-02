#pragma once
#include "config.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

class LightSensor 
{
    public:
        LightSensor() = default;
        
        bool initSensor();
        float getAverageLuxLevel();
        float getBrightnessOnStartup();
    
    private:
        Adafruit_TSL2591 tsl2591;
        sensors_event_t event;
        
        float getLuxLevel();

        bool _wireSuccess = false;
        float _lastLuxLevel = 0.0f;
        float _luxReadings[LUX_READINGS_SIZE] = {};
};
