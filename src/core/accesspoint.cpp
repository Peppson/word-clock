#include <LittleFS.h>
#include "core/accesspoint.h"
#include "core/storage.h"
#include "core/timeKeeper.h"
#include "core/hardware.h"

// Static member
bool AccessPoint::isActive = false;
bool AccessPoint::isColorPickerActive = false;


bool AccessPoint::getWebpageFromMemory(String& html) 
{
    if (!LittleFS.begin()) 
    {
        log("Failed to initialize LittleFS, in %s()\n", __func__);
        html = "<h1 style='font-size: 48px;'>Internal error! Failed to initialize filesystem</h1>";
        return false;
    }
    // Load webpage (root/data/)
    File file;
    if (!getWebpage(file)) 
    {
        html = "<h1 style='font-size: 48px;'>Internal error! Failed to load webpage</h1>";
        LittleFS.end();
        return false;
    }
    // File opened
    char c;
    while (file.available()) 
    {
        c = file.read();
        html += c;
    }

    file.close();
    LittleFS.end();
    return true;
}

bool AccessPoint::getWebpage(File& file) 
{
    // Load webpage, grab backup if broken (root/data/)
    for (const char* path : HTML_FILE_PATHS) 
    {
        file = LittleFS.open(path, "r");
        if (file) 
            return true; 
        log("LittleFS: Failed to open \"%s\" \n", path); 
    }
    
    return false;
}

void AccessPoint::setup() 
{
    if (!getWebpageFromMemory(_webPage)) 
    {
        Hardware::errorLED(true);
    }

    // Init WiFi in Accesspoint mode
    WiFi.softAP(AP_SSID, "", WIFI_CHANNEL, HIDE_SSID, MAX_CONNECTIONS);
    WiFi.softAPConfig(accesspointIP, accesspointIP, IPAddress(255, 255, 255, 0));
    DNSserver.start(DNS_PORT, "*", accesspointIP);
    
    // Server callbacks. Lambdas to get around static server.on() argument types
    server.on("/hotspot-detect.html", HTTP_GET,   [this]() { this->callbackHome();        }); // Ios
    server.on("/generate204",         HTTP_GET,   [this]() { this->callbackHome();        }); // Android
    server.on("/static/hotspot.txt",  HTTP_GET,   [this]() { this->callbackHome();        }); // PC
    server.on("/canonical.html",      HTTP_GET,   [this]() { this->callbackHome();        });
    server.on("/",                    HTTP_GET,   [this]() { this->callbackHome();        });
    server.on("/favicon.ico",         HTTP_GET,   [this]() { this->callbackFavicon();     });
    server.on("/ping",                HTTP_GET,   [this]() { this->callbackPing();        });
    server.on("/restart",             HTTP_GET,   [this]() { this->callbackRestart();     });
    
    server.on("/submit/WiFi-form",    HTTP_POST,  [this]() { this->callbackWiFi();        });
    server.on("/submit/time-form",    HTTP_POST,  [this]() { this->callbackTime();        });

    server.on("/color/start",         HTTP_GET,   [this]() { this->callbackColorStart();  });
    server.on("/color",               HTTP_POST,  [this]() { this->callbackColor();       });
    server.on("/color/end",           HTTP_POST,  [this]() { this->callbackColorEnd();    });
    
    server.on("/snake/start",         HTTP_GET,   [this]() { this->callbackSnakeStart();  });
    server.on("/snake/end",           HTTP_GET,   [this]() { this->callbackSnakeEnd();    });
    server.on("/snake/up",            HTTP_POST,  [this]() { this->callbackSnakeUp();     });
    server.on("/snake/down",          HTTP_POST,  [this]() { this->callbackSnakeDown();   });
    server.on("/snake/left",          HTTP_POST,  [this]() { this->callbackSnakeLeft();   });
    server.on("/snake/right",         HTTP_POST,  [this]() { this->callbackSnakeRight();  });

    server.onNotFound(                            [this]() { this->callback404();         });
    server.begin();
}

void AccessPoint::loop()
{
    log("\n------- Accesspoint Online! -------\n");
    uint32_t endTime = millis() + ACCESSPOINT_UPTIME;
    
    px->ledBrightness = constrain(px->ledBrightness, 30, 254);
    _initialLedBrightness = px->ledBrightness;
    isActive = true;
    
    while (millis() < endTime) 
    {
        DNSserver.processNextRequest();
        server.handleClient();
        
        _isSnakeRunning 
            ? snake.loop(endTime) 
            : px->animation_webServer(isColorPickerActive);
        
        if (Hardware::buttonReset && Hardware::resetBtnPressed(px))
            break;
        
        if (AccessPoint::_exitAccessPoint || !Hardware::buttonAccesspoint)
            break;
    }
}

void AccessPoint::respondToClient(const bool response) 
{
    server.send(response ? 200 : 400, "text/plain", ""); 
}

StaticJsonDocument<256> AccessPoint::getJsonData() 
{
    StaticJsonDocument<256> jsonDoc;
    String payload = server.arg("plain");

    // Validate
    if (payload.length() > JSON_PAYLOAD_MAX_LENGTH) 
    {
        log("Ap: JSON payload exceeds maximum length");
        return StaticJsonDocument<256>(); // Empty
    }
    // Parse 
    DeserializationError error = deserializeJson(jsonDoc, payload); 
    if (error) 
    {
        log("Ap: Failed to parse JSON");
        return StaticJsonDocument<256>(); // Empty
    }

    return jsonDoc;
}

// Server callbacks
void AccessPoint::callbackHome() 
{
    log("/home \n");
    for (size_t i = 0; i < 3; i++) 
    {
        server.send(200, "text/html", _webPage); // Just spam it        
    }
    isColorPickerActive = false;
}

void AccessPoint::callback404() 
{
    log("/404 \"%s\"\n", (server.uri().c_str()));
    server.send(404, "text/plain", "File not found");
}

void AccessPoint::callbackPing() 
{
    log("/ping \n");
    respondToClient(true);
}

void AccessPoint::callbackRestart()
 {
    log("/restart \n");
    _exitAccessPoint = true;
    respondToClient(true);
}

void AccessPoint::callbackFavicon() 
{
    log("/favicon.ico\n");

    if (!LittleFS.begin()) 
    {
        log("Failed to initialize LittleFS");
        server.send(500, "text/plain", "Failed to initialize filesystem");
        return;
    }

    File file = LittleFS.open("/favicon.ico", "r");
    if (file) 
    {
        server.streamFile(file, "image/x-icon");
        file.close();
    }
    else 
    {
        server.send(500, "text/plain", "favicon.ico not found");
    }

    LittleFS.end();
}

void AccessPoint::callbackWiFi() 
{
    log("/submit/WiFi-form\n");

    Hardware::disableAccesspointInterrupt();
    Storage storage(false);
    bool success = true;

    StaticJsonDocument<256> jsonDoc = getJsonData();
    String ssid = jsonDoc["data"]["ssid"];
    String password = jsonDoc["data"]["password"]; 
    
    // Validate and set WiFi credentials
    if (!storage.validateWiFiCredentials(ssid, password)) 
    {
        success = false;
    } 
    else if (!storage.setWiFiCredentials(ssid, password)) 
    { 
        success = false; 
    }

    respondToClient(success);
    PRINT_INFO(WIFI_FORM, ssid, password, "0", success);
    Hardware::enableAccesspointInterrupt();
}

void AccessPoint::callbackTime() 
{
    log("/submit/time-form\n");

    Hardware::disableAccesspointInterrupt();
    Storage storage(false);
    bool success = true;
    
    StaticJsonDocument<256> jsonDoc = getJsonData();
    String timeInput = jsonDoc["data"]["timeManual"];

    if (!storage.validateTimeInput(timeInput)) 
    {
        success = false; 
    } 
    else if (!storage.setTimeInput(timeInput)) 
    {
        success = false;
    }

    respondToClient(success);
    PRINT_INFO(TIME_FORM, "0", "0", timeInput, success);
    Hardware::enableAccesspointInterrupt();
}

void AccessPoint::callbackColorStart()
{
    log("/color/start \n");
    respondToClient(true);
    isColorPickerActive = true;

    log("Current brightness: %d. initialBrightness: %d \n", px->ledBrightness, _initialLedBrightness);

    px->setLedBrightness(_initialLedBrightness);
} 

void AccessPoint::callbackColor()
{
    String hexColor = server.arg("plain");
    uint32_t color = (uint32_t)strtoul(hexColor.c_str(), NULL, 16);
    px->setLedsColorFromPicker(color, false);
}  

void AccessPoint::callbackColorEnd()
{   
    Storage storage(true);
    String hexColor = server.arg("plain"); // Who needs validation?
    uint32_t color = (uint32_t)strtoul(hexColor.c_str(), NULL, 16);

    if (color != 0)
        px->setCurrentColor(color);
    
    log("/color/end - %s \n", hexColor.c_str());

    bool isSaved = storage.setHexColor(color);
    respondToClient(isSaved);
    isColorPickerActive = false;
} 

// Snake 
void AccessPoint::callbackSnakeStart() 
{
    log("/snake/start \n");
    respondToClient(true);
    _isSnakeRunning = true;

    // Reset brightness to previous value
    px->setLedBrightness(px->ledBrightness); 

    snake.newGame();
}

void AccessPoint::callbackSnakeEnd() 
{   
    const bool resetAnimation = true;
    log("/snake/end \n");
    
    respondToClient(true);
    _isSnakeRunning = false;
    px->animation_fadeAll(FADE_OUT);
    px->animation_webServer(resetAnimation);
}

void AccessPoint::callbackSnakeUp() 
{
    PRINT_BTN("/snake/up \n");
    snake.setDirection(snake.UP);
}

void AccessPoint::callbackSnakeDown() 
{
    PRINT_BTN("/snake/down \n");
    snake.setDirection(snake.DOWN);
}

void AccessPoint::callbackSnakeLeft() 
{
    PRINT_BTN("/snake/left \n");
    snake.setDirection(snake.LEFT);
}

void AccessPoint::callbackSnakeRight() 
{
    PRINT_BTN("/snake/right \n");
    snake.setDirection(snake.RIGHT);
}

void AccessPoint::debug() 
{
    //snake.gameWon();
}
