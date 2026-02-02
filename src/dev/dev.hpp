// Quick file for mocking around
#pragma once
#include "config.h" 


void initSerial() 
{
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && millis() < 5*1000) { }
    log("\n\n\n\n\n----------- DEV STARTED -----------\n"); 
}
