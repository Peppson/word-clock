#pragma once
#include <FastLED.h>
#include <Preferences.h>
#include "storage.h"
#include "core/lightSensor.h"
#include "config.h"

class NeoPixels
{
    public:
        NeoPixels() = default;

        CRGB led[TOTAL_LEDS];
        LightSensor sensor;
        TaskHandle_t animationTaskHandle;

        enum Color : uint32_t
        {
            OFF = 0x000000,
            RED = 0xFF0000,
            GREEN = 0x008000,
            LIGHT_GREEN = 0x7CFC00,
            LIGHT_BLUE = 0xDDA0DD,                  
            ORANGE = 0xFF4500, 
            BLUE = 0x0000FF,
            YELLOW = 0xFFFF00,
            WHITE = 0xFFFFFF,
            DEBUG_COLOR = DEBUG_CLOCK_COLOR
        };

        enum StartupShape : int 
        { 
            DOTS, FLAG, HEART, CHECKMARK, FAIL, NONE
        };

        enum WiFiConnectionStatus : int 
        { 
            CONNECT, SUCCESS, FAILED 
        };

        void setup();
        void setupLightSensor();
        static void setLedBrightness(uint8_t brightness);
        static float scaleLedBrightness(float brightness);
        static void setLedBrightnessScaler();
        static float applyCorrectionCurve(float x);
        void setLedBrightnessOnBoot();
        void setCurrentColorOnBoot(Storage& storage);
        void setCurrentColor(uint32_t color);
        uint32_t getCurrentColor();
        void updateLedBrightness(TimeKeeper& chrono, bool setBrightness = true);
        bool shouldUpdateInterval();
        bool shouldUpdateBrightness(float average); 
        void setLedsColor(uint32_t color, bool show = false);
        void setLedsColorFromPicker(uint32_t color, bool show = false);
        void renderClock(const struct tm* timeinfo, uint32_t offsetHours, uint32_t offsetMinutes, bool forceRedraw = false);
        void renderClockOnBoot(uint8_t hour, uint8_t minute);
        uint8_t ledIndexByXY(uint8_t Xpos, uint8_t Ypos);
        uint8_t ledIndexByIndex(uint8_t index);
        void testAllLedsFullBrightness();
        static void testRandom(); 

        void animation_startup();
        void animation_threadWiFi(const bool begin);
        void animation_gameOfLife(uint32_t maxDuration = 15*1000);
        void animation_spiral(uint32_t color, uint16_t delay);
        void animation_waterDrops();
        void animation_startupFillIn(StartupShape shape, int delay = 0, uint32_t color = DEFAULT_CLOCK_COLOR);
        void animation_startupFillInStatic(StartupShape shape, int delay_mS, uint32_t color = DEFAULT_CLOCK_COLOR); 
        void animation_startupFadeIn(StartupShape shape = NONE, bool allLeds = false);
        void animation_flag(StartupShape shape);
        void animation_fadeAll(const bool fadeIn, uint32_t delay_uS = 0);
        void animation_webServer(bool isPaused, bool resetAnimation = false);
        void animation_connectWiFi();
        void animation_connectWiFiStatus(WiFiConnectionStatus status);
        void animation_fireworks(uint8_t X, uint8_t height, Color color, uint8_t speed);
        
        static bool isBoot;
        static bool isSensorActive;
        static uint8_t ledBrightness;
        static uint8_t animationLedBrightness;
        static volatile float ledBrightnessScaler;
        static constexpr uint8_t COL = MATRIX_HEIGHT;
        static constexpr uint8_t ROW = MATRIX_WIDTH;

    private:
        enum TimeUnit 
        { 
            HOUR, MINUTE
        };

        enum class AnimationWebServerStates 
        {
            INIT, FADE_IN, WAIT, FADE_OUT 
        };
        
        static bool _isPitchDark;
        static uint32_t _currentColor;

        void drawPixel(uint8_t Xpos, uint8_t Ypos, uint32_t color, uint16_t delay = 0, bool show = true);
        void drawPixel(uint16_t pos_index, uint32_t color, uint16_t delay = 0, bool show = true);
        void drawTheTimeIs();
        void drawTimeStatic(uint8_t hour, uint8_t minute, bool show = false);
        void drawTimeStaticHourMinute(uint8_t hour, uint8_t minute);
        void drawTimeFade(uint8_t hour, uint8_t minute);
        void drawTimeFadeBoth(uint8_t hour, uint8_t minute);
        void drawTimeFadeMinute(uint8_t minute);
        void FadeInHour(uint8_t hour, uint8_t fadeAmount);
        void FadeInMinute(uint8_t minute, uint8_t fadeAmount);
        void FadeOutHour(uint8_t hour, uint8_t fadeAmount, uint8_t target);
        void FadeOutMinute(uint8_t minute, uint8_t fadeAmount, uint8_t target);
        uint8_t getHourOffset(uint8_t minute);
        uint8_t getRandomUniqueTime(TimeUnit timeType);
        void setScaledBrightness(float scaledAverage, bool setBrightness);
        void recoverLightSensor();
        bool shouldUpdateWebServerAnimation(uint& nextTime);        
        AnimationWebServerStates webServerInit(uint8_t& hour, uint8_t& minute);
        AnimationWebServerStates webServerFadeIn(uint& fadeAmount);
        AnimationWebServerStates webServerWait(uint& count);
        AnimationWebServerStates webServerFadeOut(uint& fadeAmount);
        static void connectWiFiTASK(void* ptr);

        // Game of life helpers
        void calculateNewState(bool (&curState)[COL][ROW], bool (&newState)[COL][ROW]);
        uint8_t getAliveNeighbours(bool (&state)[COL][ROW], uint8_t Xpos, uint8_t Ypos);
        void setRandomState(bool (&state)[COL][ROW]);
        bool isArrayEqual(bool (&state1)[COL][ROW], bool (&state2)[COL][ROW]);
        void drawGameOfLife(bool (&state)[COL][ROW]);

        const uint32_t END_OF_ARRAY = TOTAL_LEDS + 1;
};  
