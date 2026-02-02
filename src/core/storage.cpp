#include <Regexp.h>
#include "core/storage.h"
#include "core/hardware.h"
#include "secrets.hpp"

// Static members
String Storage::storedTimeInput;
String Storage::storedWifiSsid;
String Storage::storedWifiPassword;


Storage::Storage(bool interrupts) : _toggleInterrupts(interrupts) 
{
    if (_toggleInterrupts) 
    { 
        Hardware::disableInterrupts(); 
    }
}

Storage::~Storage() 
{
    if (_toggleInterrupts) 
    { 
        Hardware::enableInterrupts(); 
    } 
}

bool Storage::validateTimeInput(const String& timeInput) 
{
    // "HH:MM"
    if (timeInput.length() != 5) return false; 

    // Regex lib doesnt support the expression: ^(0[0-9]|1[0-9]|2[0-3]):[0-5][0-9]$ ...
    const char pattern[] = "^[0-2][0-9]:[0-5][0-9]$"; // close enough... 29:59...
    MatchState regex;

    // Convert time input to suit <Regexp.h> lib
    const char* input = timeInput.c_str();
    regex.Target( const_cast<char*>(input) );

    return regex.Match(pattern);
}

bool Storage::validateWiFiCredentials(const String& ssid, const String& password) 
{
    size_t maxLength = WIFI_CREDENTIALS_MAX_LENGTH;

    // Null
    if (ssid == "" || ssid == NULL || ssid == "null" || 
        password == "" || password == NULL || password == "null") 
    {
        log("> Validation Error: WiFi credentials NULL value \n");
        return false;
    }
    // Length
    if (ssid.length() > maxLength || password.length() > maxLength) 
    { 
        log("> Validation Error: WiFi credentials length must not exceed %zu characters \n", maxLength);
        return false;
    }

    return true;
}

uint8_t Storage::getClockSource(Preferences& memory) 
{
    uint clockSource;
    try 
    {
        clockSource = memory.getUShort("clockSource"); 
    } 
    catch (...) 
    {
        log("> Storage Error: Failure reading time clockSource\n"); 
        return ClockSource::NONE;
    }    

    PRINT_INFO(MEMORY_SOURCE, "", "", (String)clockSource);
    return clockSource;
}

bool Storage::getTimeInput(Preferences& memory) 
{
    String timeInput;

    try 
    {
        timeInput = memory.getString("time_input", ""); 
    } 
    catch (...) 
    {
        log("> Storage Error: Failure reading time input\n"); 
        return false;
    }

    if (validateTimeInput(timeInput)) 
    {
        storedTimeInput = timeInput;
        PRINT_INFO(MEMORY_TIME, "", "", storedTimeInput);
        return true;
    }

    return false;
}

bool Storage::getWiFiCredentials(Preferences& memory) 
{
    String ssid;
    String password;

    try 
    {
        ssid = memory.getString("ssid", ""); 
        password = memory.getString("password", "");
    } 
    catch (...) 
    {
        log("> Storage Error: Failure reading WiFi credentials\n");
        return false;
    }

    if (validateWiFiCredentials(ssid, password)) {
        storedWifiSsid = ssid;
        storedWifiPassword = password;
        PRINT_INFO(MEMORY_WIFI, storedWifiSsid, storedWifiPassword);
        return true;
    }   

    return false;
}

uint32_t Storage::getHexColor(Preferences& memory)
{
    uint32_t color;

    try
    {
        color = memory.getUInt("clock_color");
    } 
    catch (...)
    {
        log("Storage Error: Failure reading HexColor\n");
        color = DEFAULT_CLOCK_COLOR;
    }
        
    if (color == 0) 
    {
        log("HexColor is 0, setting to default\n");
        color = DEFAULT_CLOCK_COLOR;
    }
    
    return color;
}

bool Storage::setTimeInput(const String& timeInput) 
{
    Preferences memory;
    if (!memory.begin(MEMORY_LOCATION)) return false;

    bool success;
    try 
    {  
        memory.putString("time_input", timeInput);
        memory.putUShort("clockSource", ClockSource::TIME_INPUT);
        success = true; 
    } 
    catch (...) 
    {
        log("> Storage Error: Failure writing time input to memory \n"); 
        success = false;
    }
    
    memory.end();
    return success;
}

bool Storage::setWiFiCredentials(const String& ssid, const String& password) 
{
    Preferences memory;
    if (!memory.begin(MEMORY_LOCATION)) return false;

    bool success;
    try 
    {
        memory.putString("ssid", ssid);
        memory.putString("password", password);
        memory.putUShort("clockSource", ClockSource::WIFI);
        success = true;
    } 
    catch (...) 
    {
        log("> Storage Error: Failure writing WiFi credentials to memory \n"); 
        success = false;
    }

    memory.end();
    return success;
}

bool Storage::setHexColor(uint32_t color)
{
    Preferences memory;

    if (!memory.begin(MEMORY_LOCATION)) 
        return false;

    if (color == 0) 
    {
        log("HexColor is 0, setting to default\n");
        color = DEFAULT_CLOCK_COLOR;
    }

	if (!memory.putUInt("clock_color", color))
	{
		log("Failed to save color to memory!\n");
		return false;
	}

	return true;
}

bool Storage::setClockSource(Preferences& memory) 
{
    try 
    {
        if (memory.getUShort("clockSource") != ClockSource::NONE) 
        {
            memory.putUShort("clockSource", ClockSource::NONE);
        }
    }
    catch (...) 
    {
        log("> Storage Error: Failure resetting clockSource to NONE \n"); 
        return false;
    }

    return true;
}

void Storage::resetAllMemory() 
{
    Preferences memory;
    if (!memory.begin(MEMORY_LOCATION)) 
        return; 
    
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
        if (memory.getUInt("clock_color") != DEFAULT_CLOCK_COLOR)
            memory.putUInt("clock_color", DEFAULT_CLOCK_COLOR);
    } 
    catch (...) 
    {
        log("> Storage Error: Failure in %s \n", __func__); 
    }

    memory.end();
}

void Storage::getWiFiCredentials(String& ssid, String& password) 
{
    #if USE_SECRET_CREDENTIALS
        ssid = SECRET_SSID;
        password = SECRET_PASSWORD;
    #else 
        ssid = storedWifiSsid;
        password = storedWifiPassword;   
    #endif   
}
