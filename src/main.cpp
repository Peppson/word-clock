#include "config.h"
#include "core/storage.h"
#include "core/timeKeeper.h"
#include "core/lightSensor.h"
#include "core/neoPixels.h"
#include "core/hardware.h"

NeoPixels pixel;
Hardware hardware;
TimeKeeper chrono(&pixel);

void setup()
{
    Storage storage(true);
    hardware.setup(&pixel);
    pixel.setCurrentColorOnBoot(storage);
    pixel.animation_startup();
    pixel.setLedBrightnessOnBoot();

    #if defined(ANY_DEBUG_SYMBOL)
        utils::initDebugging(&storage, &hardware, &pixel, &chrono);
    #endif

    ///@note Clock source could be either:
    // 1. No time is set. Time starts at 0
    // 2. Manual time set. Update time accordingly (set from AP)
    // 3. WiFi credentials set. Sync time through NTP server (set from AP)
    hardware.setClockFromSource(storage, chrono);
    hardware.enableInterrupts();
}

void loop()
{
    if (Hardware::buttonAccesspoint)
        hardware.beginAccesspoint();

    if (Hardware::buttonReset)
        hardware.resetBtnPressed();

    if (chrono.clockSource == ClockSource::WIFI)
        chrono.syncSystemTimeAtNight();

    // Set led brightness based on ambient light + brightness button scaling
    pixel.updateLedBrightness(chrono);
    chrono.getSystemTime();
    pixel.renderClock(&chrono.timeinfo, chrono.offsetHours, chrono.offsetMinutes);

    // Boeing is jealous
    hardware.timerOverflowHandler(chrono);

    #if USE_KEYBOARD
        hardware.debugKeyboardInput(chrono);
    #endif
    #if PRINT_SYSTEM_TIME
        utils::printSystemTime(chrono);
    #endif
}
