#include "core/hardware.h"
#include "core/accesspoint.h"
#include "core/storage.h"
#include "core/timeKeeper.h"
#include "core/lightSensor.h"

// Static members
volatile bool Hardware::buttonReset = false;
volatile bool Hardware::buttonAccesspoint = false;
volatile bool Hardware::timeSetBybutton = false;
volatile bool Hardware::brightnessSetBybutton = false;
NeoPixels* Hardware::pixel;


void Hardware::setup(NeoPixels* px) 
{
    // Init I/O buttons with internal + external 4K7 pulldown R
    pinMode(BTN_HOUR, INPUT_PULLDOWN);
    pinMode(BTN_5_MINUTES, INPUT_PULLDOWN);
    pinMode(BTN_BRIGHTNESS, INPUT_PULLDOWN);
    pinMode(BTN_ACCESSPOINT, INPUT_PULLDOWN);
    pinMode(BTN_RESET, INPUT_PULLDOWN);

    // Led
    pinMode(PIN_ERROR_LED, OUTPUT);
    digitalWrite(PIN_ERROR_LED, LOW);

    #if PRINT_ENABLED
        beginSerial();
    #endif

    pixel = px;
    px->setup();
    px->setupLightSensor();
    printDebugInfo(px);
}

void Hardware::setClockFromSource(Storage& storage, TimeKeeper& chrono) 
{
    #if DEBUG_IGNORE_CLOCK_SOURCE
        TimeKeeper::clockSource = ClockSource::NONE;
        log("\n------- DEBUG_IGNORE_CLOCK_SOURCE -------\nClock source = NONE\n");
        return;
    #endif

    Preferences memory;
    memory.begin(MEMORY_LOCATION);
    
    uint8_t clockSource = storage.getClockSource(memory);
    bool success = true;
    
    if (clockSource == ClockSource::TIME_INPUT) 
        success = setClockFromTimeInput(storage, chrono, memory);
    else if (clockSource == ClockSource::WIFI) 
        success = setClockFromWiFiInput(storage, chrono, memory);
    else if (clockSource == ClockSource::NONE)
        chrono.resetSystemTime();

    if (success) 
    {
        TimeKeeper::clockSource = clockSource;
        chrono.printSystemTime();
    } 
    else 
    {
        TimeKeeper::clockSource = ClockSource::NONE;
        log("Error reading clock source! Reverting clock source to \"NONE\" \n");
        storage.setClockSource(memory);
    }

    memory.end();
}

bool Hardware::setClockFromTimeInput(Storage& storage, TimeKeeper& chrono, Preferences& memory) 
{
    if (!storage.getTimeInput(memory))       return false; 
    if (!chrono.setSystemTime())                    return false;
    if (!storage.setClockSource(memory))    return false;

    return true;
}

bool Hardware::setClockFromWiFiInput(Storage& storage, TimeKeeper& chrono, Preferences& memory) 
{
    if (!storage.getWiFiCredentials(memory)) return false;  
       
    // Try sync time via NTP server 
    chrono.syncSystemTimeWiFi(&storage, true);
    return true;
}

void Hardware::beginAccesspoint()
{
    disableInterrupts();
    AccessPoint accesspoint(pixel);
    accesspoint.setup();
    pixel->animation_fadeAll(FADE_OUT); 
    enableAccesspointInterrupt();

    // Loops here until manual exit or after ACCESSPOINT_UPTIME
    accesspoint.loop();

    // Restart device
    disableAccesspointInterrupt();
    delay(1000);
    accesspoint.~AccessPoint();
    reboot();
}

void Hardware::reboot() 
{
    // Makes sure the fadeout begins at whatever brightness was before,
    // if rebooting from accesspoint
    if (AccessPoint::isActive) 
    {
        pixel->ledBrightness = pixel->animationLedBrightness;
    }

    pixel->animation_fadeAll(FADE_OUT);
    delay(500);
    ESP.restart();
}

void Hardware::timerOverflowHandler(TimeKeeper& chrono) 
{
    constexpr uint32_t threshold = (UINT32_MAX - 1) - (24*60*60*1000UL);

    // Timestamp in mS from device boot, overflows every ~ 50 days.
    // Rough solution, just restart the device when near the threshold ¯\_(ツ)_/¯
    if (millis() < threshold) return; 

    // Save the current time to mem, for retreval after reboot
    // Skip if clock source = WiFi
    if (chrono.clockSource != ClockSource::WIFI) 
    {
        Storage storage(true);

        chrono.getSystemTime();
        uint8_t hour = (chrono.timeinfo.tm_hour + chrono.offsetHours) % 24;
        uint8_t minute = (chrono.timeinfo.tm_min + chrono.offsetMinutes) % 60;

        char current_time[6];
        sprintf(current_time, "%02d:%02d", hour, minute);

        if (storage.validateTimeInput(current_time)) 
        {
            storage.setTimeInput(current_time);
        }
    }

    log("millis() overflow in 24h, rebooting...\n");
    reboot();
}

void Hardware::beginSerial() 
{
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && millis() < 3*1000) { }

    delay(50);
    log("\n\n\n\n\n----------- STARTED -----------\n");
}

void Hardware::errorLED(bool error) 
{   
    if (error)
    {
        log("----------- Error -----------\n"); // Great logging :)
        digitalWrite(PIN_ERROR_LED, HIGH);
    }
    else 
    {
        digitalWrite(PIN_ERROR_LED, LOW);
    }
}

void Hardware::resetBtnPressed() 
{   
    digitalWrite(PIN_ERROR_LED, HIGH);
    detachInterrupt(BTN_RESET);
    buttonReset = false;

    // Hold reset btn for 4s to reset
    uint endTime = millis() + 4*1000;
    while (digitalRead(BTN_RESET))
    {
        if (millis() > endTime) 
        {
            log("Reset\n");
            digitalWrite(PIN_ERROR_LED, LOW);
            Storage::resetAllMemory();
            reboot();
        }
    }
    
    digitalWrite(PIN_ERROR_LED, LOW);
    attachInterrupt(BTN_RESET, isr_buttonReset, HIGH);
}

bool Hardware::resetBtnPressed(NeoPixels* px) 
{
    detachInterrupt(BTN_RESET);
    digitalWrite(PIN_ERROR_LED, HIGH);
    buttonReset = false;

    // Hold reset btn for 4s to reset
    uint endTime = millis() + 4*1000; 
    while (digitalRead(BTN_RESET)) 
    {
        if (millis() > endTime) 
        {
            log("Reset\n");
            digitalWrite(PIN_ERROR_LED, LOW);
            Storage::resetAllMemory();
            return true;
        }
        px->animation_webServer(false);
    }

    digitalWrite(PIN_ERROR_LED, LOW);
    attachInterrupt(BTN_RESET, isr_buttonReset, HIGH);
    return false;
}

// Button interrupts handler, stored in ESP32 iram. Gotta go fast!
bool IRAM_ATTR Hardware::isButtonDebounced() 
{
    static uint nextTime = 0;
    const uint curTime = millis();

    // Debounce
    if (curTime > nextTime) 
    {
        nextTime = curTime + BUTTON_DEBOUNCE_INTERVAL;
        return true;
    }

    return false;
}

void IRAM_ATTR Hardware::isr_buttonHour() 
{
    if (!isButtonDebounced()) return;

    TimeKeeper::offsetHours = (TimeKeeper::offsetHours + 1) % 24;
    timeSetBybutton = true;
}

void IRAM_ATTR Hardware::isr_button5min() 
{   
    using T = TimeKeeper;
    if (!isButtonDebounced()) return;

    T::offsetMinutes += 5;
    if (T::offsetMinutes >= 60)
    {
        T::offsetHours++;
        T::offsetHours %= 12;
        T::offsetMinutes = 0;
    }

    timeSetBybutton = true;
}

void IRAM_ATTR Hardware::isr_buttonBrightness() 
{
    if (!isButtonDebounced()) return;

    NeoPixels::setLedBrightnessScaler();
} 

void IRAM_ATTR Hardware::isr_buttonAccesspoint() 
{
    if (!isButtonDebounced()) return;

    buttonAccesspoint = !buttonAccesspoint;
}

void IRAM_ATTR Hardware::isr_buttonReset() 
{
    if (!isButtonDebounced()) return;

    buttonReset = true;
}

bool Hardware::isInterrupted() 
{ 
    return timeSetBybutton || 
           brightnessSetBybutton || 
           buttonReset || 
           buttonAccesspoint;
}

void Hardware::enableInterrupts() 
{ 
    attachInterrupt(BTN_HOUR, isr_buttonHour, RISING);
    attachInterrupt(BTN_5_MINUTES, isr_button5min, HIGH);
    attachInterrupt(BTN_BRIGHTNESS, isr_buttonBrightness, HIGH);
    attachInterrupt(BTN_ACCESSPOINT, isr_buttonAccesspoint, HIGH);
    attachInterrupt(BTN_RESET, isr_buttonReset, HIGH);
}

void Hardware::disableInterrupts() 
{
    detachInterrupt(BTN_HOUR);
    detachInterrupt(BTN_5_MINUTES);
    detachInterrupt(BTN_BRIGHTNESS);
    detachInterrupt(BTN_ACCESSPOINT);
    detachInterrupt(BTN_RESET); 
}

void Hardware::enableAccesspointInterrupt() 
{
    attachInterrupt(BTN_BRIGHTNESS, isr_buttonBrightness, HIGH);
    attachInterrupt(BTN_ACCESSPOINT, isr_buttonAccesspoint, HIGH);
    attachInterrupt(BTN_RESET, isr_buttonReset, HIGH);  
}

void Hardware::disableAccesspointInterrupt() 
{
    detachInterrupt(BTN_BRIGHTNESS);
    detachInterrupt(BTN_ACCESSPOINT);
    detachInterrupt(BTN_RESET); 
}

void Hardware::printDebugInfo(NeoPixels* px) 
{
    #if USE_SECRET_CREDENTIALS 
        log("Using WiFi credentials from secrets.hpp \n");
    #else
        log("Using real WiFi credentials, it's live \n");
    #endif

    #if DEBUG_OVERRIDE_BRIGHTNESS > 0
        log("Using DEBUG_OVERRIDE_BRIGHTNESS set @ %i \n", constrain(DEBUG_OVERRIDE_BRIGHTNESS, 0, 254));
    #endif

    #if USE_KEYBOARD
        printKeyboardInfo();
    #endif 

    log("Current Color: #%06X \n", px->getCurrentColor());
}


// ################  Debug  ################
#if USE_KEYBOARD
    #include "core/neoPixels.h"
    extern NeoPixels pixel;


    void Hardware::debugKeyboardInput(TimeKeeper& chrono) 
    {
        if (Serial.available() > 0) 
        {
            uint RxSerial = (Serial.read() - 48);

            if (RxSerial > 0 && RxSerial < 9)
                debugKeyboardAction(RxSerial, chrono);
            else
                printKeyboardInfo();
        }
        Serial.flush();
    }

    void Hardware::isr_debugIncrementTime() 
    {
        uint8_t hour = TimeKeeper::offsetHours;
        uint8_t minute = TimeKeeper::offsetMinutes;

        minute += 5;
        if (minute >= 60)
        {
            hour++;
            hour %= 12;
            minute = 0;
        }
        
        TimeKeeper::offsetHours = hour;
        TimeKeeper::offsetMinutes = minute;
    }

    void Hardware::printKeyboardInfo() 
    {
        log("Only keys 1...8 is supported...\n");
        log("1:Hour++  2:Min++  3:Time++  4:BrightScaler  5:Reboot  6:AP  7:Reset  8:testRandom()\n");   
    }

    void Hardware::debugKeyboardAction(const uint8_t button, TimeKeeper& chrono)
    {
        switch (button) 
        {
            case HOURS:
                TimeKeeper::offsetHours = (TimeKeeper::offsetHours + 1) % 24;
                timeSetBybutton = true;
                chrono.printSystemTime();
                break;
            case FIVE_MINUTES:
                timeSetBybutton = true;
                isr_debugIncrementTime();
                chrono.printSystemTime();
                break;
            case DEBUG_TIME:
                isr_debugIncrementTime();
                chrono.printSystemTime();
                break;
            case INCREMENT_BRIGHTNESS:
                isr_buttonBrightness();
                log("ledBrightnessScaler: %.1f\n", NeoPixels::ledBrightnessScaler);
                break;
            case REBOOT:
                log("Rebooting...\n");
                reboot();
                break;
            case START_ACCESSPOINT:
                log("Accesspoint...\n");
                isr_buttonAccesspoint();
                break;   
            case RESET_BTN:
                log("Reset...\n");
                isr_buttonReset();
                break;
            case DEBUG_RANDOM:
                log("testRandom() \n");
                NeoPixels::testRandom();
                break;
            default:
                break;
        } 
    }
#endif // USE_KEYBOARD
