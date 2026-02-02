#pragma once
#include <DNSServer.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "core/snake.h"

class File;

class AccessPoint 
{
    public:
        AccessPoint(NeoPixels* pixel) : px(pixel), snake(pixel), server(80), accesspointIP(192, 168, 4, 1)
        {
            WiFi.mode(WIFI_AP);
        }

        ~AccessPoint() 
        { 
            WiFi.mode(WIFI_OFF);
        }

        void setup();
        void loop();
        void debug();

        static bool isActive;
        static bool isColorPickerActive;
    
    private: 
        NeoPixels* px;
        Snake snake;
        WebServer server;
        DNSServer DNSserver;
        IPAddress accesspointIP;

        bool getWebpageFromMemory(String& html);
        bool getWebpage(File& file);
        void respondToClient(const bool response); 
        StaticJsonDocument<256> getJsonData();
        
        // Server cannot do callbacks with arguments...
        void callbackHome(); 
        void callback404();
        void callbackPing();
        void callbackRestart();
        void callbackFavicon();
        void callbackWiFi();
        void callbackTime();
        void callbackColorStart();
        void callbackColor();
        void callbackColorEnd();
        void callbackSnakeStart();
        void callbackSnakeEnd();
        void callbackSnakeUp();
        void callbackSnakeDown();
        void callbackSnakeLeft();
        void callbackSnakeRight();
    
        String _webPage;
        bool _isSnakeRunning = false;
        bool _exitAccessPoint = false;
        uint8_t _initialLedBrightness = 0;
        const byte DNS_PORT = 53;
        const uint8_t WIFI_CHANNEL = 6;
        const uint8_t HIDE_SSID = 0;        // Hide WiFi SSID broadcasting
        const uint8_t MAX_CONNECTIONS = 1;
};
