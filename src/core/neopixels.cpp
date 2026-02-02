#include "core/neoPixels.h"
#include "core/hardware.h"
#include "core/timeKeeper.h"
#include "ledMapping/ledMapping.h"
#include "ledMapping/ledDataTimeIs.h"

// See ledMapping/ledMapping.h for more info about the mapping process
static constexpr auto hoursLedMap = getLedMap<HOURS_COLUMN_SIZE, HOURS_LARGEST_ROW>(hours::oldLedMap);
static constexpr auto minutesLedMap = getLedMap<MINUTES_COLUMN_SIZE, MINUTES_LARGEST_ROW>(minutes::oldLedMap);

// Static members
volatile float NeoPixels::ledBrightnessScaler = LED_INITIAL_SCALER; 
bool NeoPixels::isBoot = true;
bool NeoPixels::isSensorActive = true;
bool NeoPixels::_isPitchDark = false;
uint8_t NeoPixels::animationLedBrightness = 1;
uint8_t NeoPixels::ledBrightness = 1;
uint32_t NeoPixels::_currentColor = DEFAULT_CLOCK_COLOR;
TaskHandle_t animationTaskHandle = NULL;

/* 
*   ######### Led layout #########  
*
*   SERPENTINE_LED_LAYOUT true
*   0  >  1  >  2  >  3  >  4 ... 11
*                                    |
*   22 ... 15  <  14  <  13  <  12
*   |
*   23 ...
*
*   SERPENTINE_LED_LAYOUT false
*   0  >  1  >  2  >  3  >  4 ... 11
*   12  >  13  >  14  >  15 ...
*/

#if SERPENTINE_LED_LAYOUT // true
	uint8_t NeoPixels::ledIndexByXY(uint8_t posX, uint8_t posY) 
	{
		uint8_t index = 0;

		// Even rows run forwards
		if (posY % 2 == 0) 
		{
			index = (posY * MATRIX_WIDTH) + posX;
		} 
		else // Odd rows run backwards 
		{
			uint8_t X_reverse = (MATRIX_WIDTH - 1) - posX;
			index = (posY * MATRIX_WIDTH) + X_reverse;		
		}
		
		return index;
	}

	uint8_t NeoPixels::ledIndexByIndex(uint8_t index) 
	{
		uint8_t posX = index % MATRIX_WIDTH;
		uint8_t posY = (index / MATRIX_WIDTH) % MATRIX_HEIGHT;

		return ledIndexByXY(posX, posY);
	}

#else // false 
	uint8_t NeoPixels::ledIndexByXY(uint8_t posX, uint8_t posY) 
	{
		return (posY * MATRIX_WIDTH) + posX;
	}

	uint8_t NeoPixels::ledIndexByIndex(uint8_t index) 
	{
		return index;
	}
	
#endif


void NeoPixels::setup()
{
	FastLED.addLeds<NEOPIXEL, PIN_LED_DATA>(led, TOTAL_LEDS);
	setLedsColor(Color::OFF, true);
	setLedBrightness(ledBrightness);
	delay(10);
}

void NeoPixels::setupLightSensor() 
{	
	if (DEBUG_OVERRIDE_BRIGHTNESS > 0) 
		return;

	if (!sensor.initSensor())
	{
		Hardware::errorLED(true);
		log("Light sensor failed!\n");
		isSensorActive = false;
	}
	else 
	{
		Hardware::errorLED(false);
		log("Light sensor success!\n");
		isSensorActive = true;
	}
}

void NeoPixels::animation_threadWiFi(const bool begin) 
{	
	const int timeout = 3000;

	if (begin) 
	{
		if (animationTaskHandle != NULL) return;

		xTaskCreatePinnedToCore (
			connectWiFiTASK,				// Task function
			"connectWiFiTASK",				// Name
			1500,							// Stack size
			this,							// Parameter
			1,								// Priority
			&animationTaskHandle,			// Task handle
			1								// N core, wifi on core 0
		);
		return;
	}

	if (animationTaskHandle != NULL) 
	{
		vTaskDelete(animationTaskHandle);
		animationTaskHandle = NULL;
	}	
}

void NeoPixels::updateLedBrightness(TimeKeeper& chrono, bool setBrightness)
{	
	static bool wasDark = false;

	if (!isSensorActive && DEBUG_OVERRIDE_BRIGHTNESS == 0)
	{
		recoverLightSensor();
		return;
	}

	if (!shouldUpdateInterval())
		return;

	float average = sensor.getAverageLuxLevel();
	
	if (!shouldUpdateBrightness(average)) 
		return;

	// Turn off leds if total darkness 
    if (average < 1.0f)
    {
        setLedsColor(Color::OFF, true);
		_isPitchDark = true;
        wasDark = true;
        return;
    }

	// Force redraw if going back to light
    if (wasDark)
    {
		_isPitchDark = false;
        wasDark = false;
        renderClock(&chrono.timeinfo, chrono.offsetHours, chrono.offsetMinutes, true);
    }

	// Scale Lux to led brightness 
	float normalizedAverage = average / LUX_THRESHOLD_MAX;	
	float mappedAverage = applyCorrectionCurve(normalizedAverage);	
	float scaledAverage = scaleLedBrightness(mappedAverage);		// Scale based on "brightness button" (20% to 100%)
	setScaledBrightness(scaledAverage, setBrightness);

	#if 0
		log("ledBrightness: %3i     Lux avg: %7.2f     Scale: %3.1f \n", NeoPixels::ledBrightness, average, NeoPixels::ledBrightnessScaler);
	#endif
}

bool NeoPixels::shouldUpdateInterval() 
{
	static uint32_t nextTime = 0;
	uint32_t curTime = millis();
	
	if (curTime < nextTime)
		return false;

	nextTime = curTime + 30;
	return true;
}

bool NeoPixels::shouldUpdateBrightness(float average)
{	
	if (Hardware::brightnessSetBybutton) 
	{
		Hardware::brightnessSetBybutton = false;
		return true;
	}
	return true;
}

void NeoPixels::setScaledBrightness(float scaledAverage, bool setBrightness) 
{	
	uint8_t brightness;

	#if DEBUG_OVERRIDE_BRIGHTNESS > 0 
		brightness = constrain(DEBUG_OVERRIDE_BRIGHTNESS, 0, 254);
		brightness = scaleLedBrightness(brightness);
	#else
		brightness = static_cast<uint8_t>(scaledAverage * 254.0f);
	#endif

	ledBrightness = constrain(brightness, LED_MIN_BRIGHTNESS, 254);
	if (setBrightness)
	{
		setLedBrightness(ledBrightness);
	}
}

void NeoPixels::recoverLightSensor()
{	
	log("Light sensor recover attempt!\n");
	setupLightSensor();

	// Backup brightness
	if (!isSensorActive)
	{
		ledBrightness = static_cast<uint8_t>(scaleLedBrightness(254.0f));
		setLedBrightness(ledBrightness);

		log("Setting backup brightness: %3i\n", ledBrightness);
	}
}

float NeoPixels::applyCorrectionCurve(float x) 
{
	return std::pow(x, LUX_CURVE_EXPONENT);
}

float NeoPixels::scaleLedBrightness(float brightness) 
{
    return brightness * ledBrightnessScaler; 
}

void NeoPixels::setLedBrightness(uint8_t brightness)
{	
	brightness = constrain(brightness, LED_MIN_BRIGHTNESS, 254);
	FastLED.setBrightness(brightness);
}

void NeoPixels::setLedBrightnessScaler() 
{	
	ledBrightnessScaler += 0.2f;
	if (ledBrightnessScaler > 1.0f) 
	{
		ledBrightnessScaler = 0.2f;
	}

	Hardware::brightnessSetBybutton = true;
}

void NeoPixels::setLedBrightnessOnBoot()
{	
	if (!isSensorActive && DEBUG_OVERRIDE_BRIGHTNESS == 0)
	{
		recoverLightSensor();
		return;
	}

	float average = sensor.getBrightnessOnStartup();
	float normalizedAverage = average / LUX_THRESHOLD_MAX;
	float mappedAverage = applyCorrectionCurve(normalizedAverage);
	float scaledAverage = scaleLedBrightness(mappedAverage);
	setScaledBrightness(scaledAverage, true);
}

void NeoPixels::setCurrentColorOnBoot(Storage& storage)
{	
	Preferences memory;
	
	if (memory.begin(MEMORY_LOCATION))
	{
		_currentColor = storage.getHexColor(memory);
	}
	else 
	{
		log("Failed to read color from memory @ boot!\n");
		_currentColor = DEFAULT_CLOCK_COLOR;
	}
}

void NeoPixels::setCurrentColor(uint32_t color)
{	
	_currentColor = color;
}

uint32_t NeoPixels::getCurrentColor()
{	
	return _currentColor;
}

void NeoPixels::setLedsColor(uint32_t color, bool show) 
{
	for (uint8_t i = 0; i < TOTAL_LEDS; i++) 
	{
		led[i] = color;
	}
	if (show) FastLED.show();
}

void NeoPixels::setLedsColorFromPicker(uint32_t color, bool show)
{
	_currentColor = color;
    if (show) FastLED.show();
}

void NeoPixels::renderClock(const struct tm* timeinfo, uint32_t offsetHours, uint32_t offsetMinutes, bool forceRedraw) 
{	
	static uint8_t prevHour = UINT8_MAX;
	static uint8_t prevMinute = UINT8_MAX;
	uint8_t hour = (timeinfo->tm_hour + offsetHours) % 12;
	uint8_t minute = ((timeinfo->tm_min + offsetMinutes) % 60) / 5;

	// Fade in on boot
	if (isBoot)
	{	
		renderClockOnBoot(hour, minute);
		prevHour = hour;
		prevMinute = minute;
		return;
	}

	// Last min fix when going from total darkness back to light
	if (forceRedraw) 
	{
		drawTimeStatic(hour, minute, true);
	}

	// Refresh leds in the case of brightness change
	if (hour == prevHour && minute == prevMinute)
	{
		FastLED.show(); 
		return;
	}

	// Write new time, skip fade animation if changed by buttons
	if (Hardware::timeSetBybutton)
	{
		Hardware::timeSetBybutton = false;
		drawTimeStatic(hour, minute, true);
	} 
	else
	{
		drawTimeFade(hour, minute);
	}

	prevHour = hour;
	prevMinute = minute;
}

void NeoPixels::renderClockOnBoot(uint8_t hour, uint8_t minute) 
{	
	drawTimeStatic(hour, minute);
	animation_fadeAll(FADE_IN);
	isBoot = false;
}

uint8_t NeoPixels::getHourOffset(uint8_t minute)
{	
	// From "20 past 8" to "5 to half 9" - swedish notation
	// In 5min steps (5 = 25min) 
	return (minute >= 5) ? 1 : 0;
}

uint8_t NeoPixels::getRandomUniqueTime(TimeUnit timeUnit) 
{
	static_assert(std::is_enum_v<decltype(timeUnit)>, "arg must be of enum type TimeUnit");
	static uint8_t prevMinute = UINT8_MAX;
	static uint8_t prevHour = UINT8_MAX;
	uint8_t time = 0;

	if (timeUnit == HOUR) 
	{
		do 
		{
			time = random8() % 12; 
		} while (time == prevHour);
		prevHour = time;
		return time;
	}

	do 
	{
		time = random8() % 12; 
	} while (time == prevMinute || time == 0); // Minutes [0] = empty
	prevMinute = time;
	return time;
}

void NeoPixels::drawPixel(uint8_t posX, uint8_t posY, uint32_t color, uint16_t delay_mS, bool show) 
{
	led[ ledIndexByXY(posX, posY) ] = color;

	if (show) FastLED.show();			
	delay(delay_mS);	
}

void NeoPixels::drawPixel(uint16_t index, uint32_t color, uint16_t delay_mS, bool show) 
{
	led[ ledIndexByIndex(index) ] = color;

	if (show) FastLED.show();			
	delay(delay_mS);	
}

void NeoPixels::drawTheTimeIs() 
{	
	for (size_t i = 0; i < MATRIX_WIDTH; i++)
	{
		if (theTimeIsLedMap[i])
			led[ ledIndexByIndex(i) ] = _currentColor;
	} 
}

void NeoPixels::drawTimeStatic(uint8_t hour, uint8_t minute, bool show) 
{	
	hour = (hour + getHourOffset(minute)) % 12;
	TimeKeeper::prevShowedHour = hour;
	TimeKeeper::prevShowedMinute = minute;

	setLedsColor(Color::OFF);
	drawTheTimeIs();
	drawTimeStaticHourMinute(hour, minute);

	if (show) FastLED.show();
}

void NeoPixels::drawTimeStaticHourMinute(uint8_t hour, uint8_t minute)
{	
	for (size_t i = 0; i < HOURS_LARGEST_ROW; i++) 
	{	
		if (hoursLedMap[hour][i] == END_OF_ARRAY)
			break;
		led[ hoursLedMap[hour][i] ] = _currentColor;
	}

	for (size_t i = 0; i < MINUTES_LARGEST_ROW; i++) 
	{	
		if (minutesLedMap[minute][i] == END_OF_ARRAY) 
			break;
		led[ minutesLedMap[minute][i] ] = _currentColor;
	}
}

void NeoPixels::drawTimeFade(uint8_t hour, uint8_t minute)
{	
	// Spaghetti solution to not draw new time when in total darkness
	if (_isPitchDark) return;

	hour = (hour + getHourOffset(minute)) % 12;
	drawTheTimeIs();

	if (TimeKeeper::prevShowedHour != hour)
	{	
		drawTimeFadeBoth(hour, minute);
	} 
	else
	{	
		drawTimeFadeMinute(minute);
	}

	TimeKeeper::prevShowedHour = hour;
	TimeKeeper::prevShowedMinute = minute;
}

void NeoPixels::drawTimeFadeBoth(uint8_t hour, uint8_t minute)
{	
	// Makes sure fade takes the same amount of time regardless of ledBrightness
	uint totalDelay_mS = 1000 - (3 * ledBrightness);
	uint dynamicDelay_uS = (totalDelay_mS * 1000) / ledBrightness;

	// Fade out
	uint8_t fadeAmount;
	for (uint8_t i = 0; i <= ledBrightness; i++)
	{	
		fadeAmount = map(i, 0, ledBrightness, 0, 255);
		FadeOutHour(TimeKeeper::prevShowedHour, fadeAmount, i);
		FadeOutMinute(TimeKeeper::prevShowedMinute, fadeAmount, i);
		FastLED.show();
		delayMicroseconds(dynamicDelay_uS);
	}

	delay(150);

	// Fade in
	for (uint8_t i = 0; i <= ledBrightness; i++)
	{	
		fadeAmount = map(i, 0, ledBrightness, 0, 255);
		FadeInHour(hour, fadeAmount);
		FadeInMinute(minute, fadeAmount);
		FastLED.show();
		delayMicroseconds(dynamicDelay_uS);
	}
}

void NeoPixels::drawTimeFadeMinute(uint8_t minute)
{
	// Makes sure fade takes the same amount of time regardless of ledBrightness
	uint totalDelay_mS = 1000 - (3 * ledBrightness);
	uint dynamicDelay_uS = (totalDelay_mS * 1000) / ledBrightness;
	
	// Fade out 
	uint8_t fadeAmount;
	for (uint8_t i = 0; i <= ledBrightness; i++)
	{	
		fadeAmount = map(i, 0, ledBrightness, 0, 255);
		FadeOutMinute(TimeKeeper::prevShowedMinute, fadeAmount, i);
		FastLED.show();
		delayMicroseconds(dynamicDelay_uS);
	}

	delay(150);

	// Fade in
	for (uint8_t i = 0; i <= ledBrightness; i++)
	{	
		fadeAmount = map(i, 0, ledBrightness, 0, 255);
		FadeInMinute(minute, fadeAmount);
		FastLED.show();
		delayMicroseconds(dynamicDelay_uS);
	}
}

void NeoPixels::FadeInHour(uint8_t hour, uint8_t fadeAmount)
{	
	for (uint8_t i = 0; i < HOURS_LARGEST_ROW; i++)
	{	
		if (hoursLedMap[hour][i] == END_OF_ARRAY) 
			break;
		led[ hoursLedMap[hour][i] ] = blend(Color::OFF, _currentColor, fadeAmount);
	}		
}

void NeoPixels::FadeInMinute(uint8_t minute, uint8_t fadeAmount)
{	
	// Don't fade into nothing
	if (minute == 0) return;

	for (uint8_t i = 0; i < MINUTES_LARGEST_ROW; i++)
	{	
		if (minutesLedMap[minute][i] == END_OF_ARRAY) 
			break;
		led[ minutesLedMap[minute][i] ] = blend(Color::OFF, _currentColor, fadeAmount);
	}
}

void NeoPixels::FadeOutMinute(uint8_t minute, uint8_t fadeAmount, uint8_t target)
{	
	// Don't fade from nothing
	if (minute == 0) return;

	for (uint8_t i = 0; i < MINUTES_LARGEST_ROW; i++)
	{	
		if (minutesLedMap[minute][i] == END_OF_ARRAY) 
			break;

		// Set lED brightness
		if (target < ledBrightness)
			led[ minutesLedMap[minute][i] ] = blend(_currentColor, Color::OFF, fadeAmount);
		else
			led[ minutesLedMap[minute][i] ] = Color::OFF;
	}
}

void NeoPixels::FadeOutHour(uint8_t hour, uint8_t fadeAmount, uint8_t target)
{	
	for (uint8_t i = 0; i < HOURS_LARGEST_ROW; i++)
	{	
		if (hoursLedMap[hour][i] == END_OF_ARRAY)
			break;

		// Set lED brightness
		if (target < ledBrightness)
			led[ hoursLedMap[hour][i] ] = blend(_currentColor, Color::OFF, fadeAmount); 
		else 
			led[ hoursLedMap[hour][i] ] = Color::OFF;
	}
}

bool NeoPixels::shouldUpdateWebServerAnimation(uint& nextTime)
{
	uint32_t curTime = micros();
	if (curTime < nextTime) return false;

	uint totalDelay_mS = 1024 - (4 * ledBrightness);
	uint dynamicDelay_uS = (totalDelay_mS * 1000) / ledBrightness; 
	dynamicDelay_uS *= 2;
	nextTime = curTime + dynamicDelay_uS;
	
	return true;
}


// ################  Debug  ################
void NeoPixels::testAllLedsFullBrightness() 
{
	uint8_t initialLedBrightness = ledBrightness;
	ledBrightness = 254;
	
    setLedsColor(Color::WHITE, false);
	animation_fadeAll(FADE_IN);
    delay(10000);
	animation_fadeAll(FADE_OUT);

	ledBrightness = initialLedBrightness;
	setLedBrightness(ledBrightness);
	
	log("Test complete!\n");
	while (true) { delay(100); }
}
