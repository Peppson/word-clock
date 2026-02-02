#include "utility.h"
#include "core/storage.h"
#include "core/hardware.h"
#include "core/snake.h"
#include "core/timeKeeper.h"


void IRAM_ATTR utils::benchmark(const char* name) 
{
    static bool begin = true;
    static uint startTime = 0;

    // Start timer
	if (begin) 
    {
        log("> ");
        begin = false;
		startTime = micros();
        return;
	}
    // Stop timer + output
    uint endTime = micros() - startTime;
    String numStr = String(endTime);

    // Name (if any)
    if (name != nullptr) { log("%s: ", name); }
    
    // Microseconds
    if (numStr.length() < 4) 
    {
        log("%iμs \n", endTime);
    } 
    // Milliseconds
    else if (numStr.length() < 6)  
    {
        float end_ms = (float)endTime / 1000;
        log("%.3fms \n", end_ms);
    }
    else 
    {
        log("%ims \n", endTime / 1000);
    }

    // Reset
    begin = true;
    startTime = 0;	
}

void utils::printAllMemory() 
{
    Preferences memory;
    memory.begin(MEMORY_LOCATION);
    try 
    {  
        log("\n------- printAllMemory() -------\n");
        log("clockSource: \t%s\n", utils::inputToEnum( memory.getUShort("clockSource") )); 
        log("timeInput: \t%s\n", memory.getString("time_input", "").c_str()); 
        log("ssid: \t\t%s\n", memory.getString("ssid", "").c_str());
        log("password: \t%s\n\n", memory.getString("password", "").c_str());

    } 
    catch (...) 
    {
        log("> Storage Error: Failure in printAllMemory()\n"); 
    }

    memory.end();
}

void utils::resetAllMemory() 
{
    Preferences memory;
    memory.begin(MEMORY_LOCATION);
    bool success = true;
    try 
    {
        if (memory.getUShort("clockSource") != ClockSource::NONE)
            memory.putUShort("clockSource", ClockSource::NONE);
        if (memory.getString("time_input") != "00:00")
            memory.putString("time_input", "00:00");
        if (memory.getString("ssid") != "")
            memory.putString("ssid", "");
        if (memory.getString("password") != "")
            memory.putString("password", "");
    } 
    catch (...) 
    {
        success = false;
        log("> Storage Error: Failure in utils::%s() \n", __func__); 
    }

    if (success) log("Successfully reset all memory!\n"); 

    memory.end();
    log("Press any key to continue booting OR reupload\n");
    while (Serial.available() == 0) {}
    Serial.flush();
}

void utils::printSystemTime(TimeKeeper& chrono) 
{
    static uint32_t endTime = 0;

    // Print each second
    if (millis() > endTime) 
    {
        chrono.printSystemTime();
        endTime = millis() + 1000;
    }
}

void utils::initDebugging(Storage* storage, Hardware* hardware, NeoPixels* pixel, TimeKeeper* chrono) 
{
    #if !PRINT_ENABLED
        #warning "Serial print is disable. Enable PRINT_ENABLED in config.h"
    #endif

    #if DEBUG_ACCESSPOINT
        log("\n------- DEBUG_ACCESSPOINT -------\n");
        initAccesspoint(hardware);
    #elif DEBUG_SNAKE
        log("\n------- DEBUG_SNAKE -------\n-- Use WASD \n");
        initSnake(pixel);
    #elif DEBUG_RESET_ALL
        log("\n------- DEBUG_RESET_ALL -------\n");
        resetAllMemory();
    #elif DEBUG_TEST_FULL_BRIGHTNESS 
        log("\n------- DEBUG_TEST_FULL_BRIGHTNESS -------\n");
        pixel->testAllLedsFullBrightness();
    #endif 
} 

void utils::initAccesspoint(Hardware* hardware) 
{
    hardware->buttonAccesspoint = true;
}

void utils::initSnake(NeoPixels* pixel) 
{
    Snake snake(pixel);
    pixel->setLedsColor(pixel->OFF, true);
    snake.newGame();

    while (true) 
    {
        if (snake.debugKeyboardInput()) 
        {
            if (!snake.newFrame()) 
                snake.newGame(); 
        }  
    }
}

void utils::debugPrintInfo(const uint target, 
                            const String& ssid, 
                            const String& password, 
                            const String& timeInput,
                            bool success) 
{
    switch (target) 
    {
        case WIFI_FORM:
            if (!success) { break; }
            log("\n------- WiFi credentials saved -------\n");
            log("ssid: %s\n", ssid.c_str());
            log("pass: %s\n", password.c_str());
            break;
        case TIME_FORM:
            if (!success) { break; }
            log("\n------- Time saved -------\n");
            log("time: %s\n", timeInput.c_str());
            break;
        case MEMORY_WIFI:
            log("\n------- Read WiFi from memory ------- \n"); 
            log("ssid: %s\n", ssid.c_str());
            log("pass: %s\n", password.c_str());
            break;
        case MEMORY_TIME:
            log("\n------- Read time from memory ------- \n"); 
            log("Stored time: %s\n", timeInput.c_str());
            break;
        case MEMORY_SOURCE:
            log("Clock source: %s\n\n", inputToEnum(timeInput));
            break;
        default:
            break;
    }
}

// Why did I do it like this again?
const char* utils::inputToEnum(const String& input) 
{
    if (input == "0")       return "None"; 
    else if (input == "1")  return "time input"; 
    else if (input == "2")  return "WiFi sync";
    else                    return ":("; 
} 

const char* utils::inputToEnum(const int& input) 
{
    if (input == 0)         return "None"; 
    else if (input == 1)    return "time input"; 
    else if (input == 2)    return "WiFi sync";
    else                    return ":(";
} 
