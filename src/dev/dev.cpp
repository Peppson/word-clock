/* #include <Arduino.h>
#include <Wire.h>
#include "dev.hpp"


void setup()
{
	initSerial();
	Serial.println("\n\n\nI2C Scanner starting...");

	// printf("\n#####  Stop right there criminal scum!  ##### \n"); while (1) { delay(1); }
	delay(100);

	bool wireSuccess = Wire.begin();
	Serial.println(wireSuccess ? "Wire initialized successfully." : "Failed to initialize Wire.");
}

void loop()
{
	byte error, address;
	int devicesFound = 0;

	Serial.println("Scanning I2C bus...");

	for (address = 1; address < 127; address++)
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println(" !");
			devicesFound++;
		}
	}

	if (devicesFound == 0)
		Serial.println("No I2C devices found\n");

	delay(2000); // Wait 2 seconds before next scan
} */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "dev.hpp"
#include "FS.h"
#include "SPIFFS.h"
#include <cstdio>

float minLux = MAXFLOAT;
float maxLux = -MAXFLOAT;
const char *luxFile = "/lux.txt";
Adafruit_TSL2591 tsl = Adafruit_TSL2591(1);
void loadLuxValues();
void initSensor();
void resetLuxValues();

void setup()
{
	initSerial();

	if (!SPIFFS.begin(true))
	{
		Serial.println(F("SPIFFS Mount Failed"));
		return;
	}

#if 0
	resetLuxValues();
	log("Values reset!\n");
	loadLuxValues();
	while (1)
	{
	}
#endif

	loadLuxValues();
	delay(10000);
	initSensor();
}

void initSensor()
{
	tsl.setGain(TSL2591_GAIN_HIGH);				  // Mid/High?
	tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS); // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);

	if (tsl.begin())
		Serial.println(F("Found a TSL2591 sensor"));
	else
	{
		Serial.println(F("No sensor found ... check your wiring?"));
		while (1)
			;
	}
}

void saveLuxValues()
{
	File file = SPIFFS.open(luxFile, FILE_WRITE);
	if (!file)
	{
		Serial.println(F("Failed to open file for writing"));
		return;
	}

	file.printf("%.2f %.2f\n", minLux, maxLux);
	file.close();
	Serial.println(F("Lux min/max saved"));
}

void loadLuxValues()
{
	if (!SPIFFS.exists(luxFile))
	{
		Serial.println(F("No previous lux data"));
		return;
	}
	File file = SPIFFS.open(luxFile, FILE_READ);
	if (!file)
	{
		Serial.println(F("Failed to open file for reading"));
		return;
	}

	String content = file.readStringUntil('\n');
	file.close();

	if (sscanf(content.c_str(), "%f %f", &minLux, &maxLux) == 2)
	{
		Serial.printf("Loaded minLux=%.2f maxLux=%.2f\n", minLux, maxLux);
	}
	else
	{
		Serial.println(F("Failed to parse lux data"));
	}
}

void resetLuxValues()
{
	minLux = MAXFLOAT;
	maxLux = -MAXFLOAT;

	if (SPIFFS.exists(luxFile))
	{
		SPIFFS.remove(luxFile);
		Serial.println(F("Lux file deleted, values reset"));
	}
	else
	{
		Serial.println(F("No lux file found, values reset in memory"));
	}
}

void getLuxLevel()
{
	sensors_event_t event;
	tsl.getEvent(&event);

	if ((event.light == 0) || (event.light > 4294966000.0) || (event.light < -4294966000.0))
	{
		Serial.println(F("Invalid data (adjust gain or timing)"));
		return;
	}
	else
	{
		Serial.print(event.light);
		Serial.println(F(" lux"));
	}

	float lux = event.light;
	Serial.printf("Lux: %.2f\n", lux);

	bool updated = false;
	if (lux < minLux)
	{
		minLux = lux;
		updated = true;
	}
	if (lux > maxLux)
	{
		maxLux = lux;
		updated = true;
	}

	if (updated)
	{
		Serial.printf("Updated min=%.2f max=%.2f\n", minLux, maxLux);
		saveLuxValues();
	}
}

void loop()
{
	getLuxLevel();
	delay(2000);
}
