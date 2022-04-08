
/**
 * @file dice_client.h
 * @author Petr Vanek (petr@fotoventus.cz)
 * @brief HTTP getter
 * @version 0.1
 * @date 2022-04-05
 * 
 * @copyright Copyright (c) 2022 Petr Vanek
 * 
 */

#pragma once

#include <M5Atom.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

/**
 * @brief http client for energy status retrieval
 * 
 */
class DiceClient {
private:
    WiFiMulti   _wifiMulti;
    int         _powerScale {0};

public:

    int getPower() { return _powerScale; }

    void init(const char* ssid, const char* passwd) {
        _wifiMulti.addAP(ssid, passwd); 
    }

    bool isConnected() {
        return (_wifiMulti.run() == WL_CONNECTED);
    }

    bool getData(const char* url) {
        bool rc = false;
        HTTPClient  http;
        http.begin(url); 
        auto  httpCode = http.GET(); 
        if(httpCode == HTTP_CODE_OK) {  
            String payload = http.getString();
            auto myObject = JSON.parse(payload);
            if (JSON.typeof(myObject) != "undefined") {
                JSONVar keys = myObject.keys();

                // enumerate all items - for future use   
                for (int i = 0; i < keys.length(); i++) {
                    JSONVar value = myObject[keys[i]];
                    if (strcmp(keys[i], "power") == 0) {
                        _powerScale = int(value);
                        rc = true;
                        break;
                    }
                }
            }
        }

        return rc;
    }
};