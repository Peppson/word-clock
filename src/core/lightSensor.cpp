#include "core/lightSensor.h"

bool LightSensor::initSensor()
{   
    std::fill_n(_luxReadings, LUX_READINGS_SIZE, 1.0f);

    tsl2591 = Adafruit_TSL2591(1);
    tsl2591.setGain(TSL2591_GAIN_HIGH);
	tsl2591.setTiming(TSL2591_INTEGRATIONTIME_200MS);

    return tsl2591.begin();
}

float LightSensor::getLuxLevel()
{   
    #if DEBUG_OVERRIDE_BRIGHTNESS > 0
        return 42.0f;
    #else
        tsl2591.getEvent(&event);

        if ((event.light == 0) || (event.light > 4294966000.0) || (event.light <-4294966000.0))
        {
            log("LightSensor: Invalid data");
            return _lastLuxLevel;
        }
        
        float luxLevel = constrain(event.light, 1, LUX_THRESHOLD_MAX);
        
        // Gaurd against total darkness (NaN)
        if (isnan(luxLevel)) 
        {
            luxLevel = (LEDS_OFF_IN_TOTAL_DARKNESS) ? 0.0f : 1.0f;
        }
        _lastLuxLevel = luxLevel;

        return luxLevel;
    #endif
}

float LightSensor::getAverageLuxLevel() 
{
    static size_t index = 0;
    
    // Update the moving average
    _luxReadings[index] = getLuxLevel();
    index = (index + 1) % LUX_READINGS_SIZE;

	float sum = 0;
    for (size_t i = 0; i < LUX_READINGS_SIZE; i++) 
    {
        sum += _luxReadings[i];
    }

	return sum / LUX_READINGS_SIZE;
}

float LightSensor::getBrightnessOnStartup()
{
	float sum = 0;
    for (size_t i = 0; i < LUX_READINGS_SIZE; i++)
    {   
        _luxReadings[i] = getLuxLevel();
        sum += _luxReadings[i];
    }
    
    return sum / LUX_READINGS_SIZE;
}
